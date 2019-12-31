/*Реализуйте функцию с сигнатурой:

          extern size_t
          read_data_and_count(size_t N, int in[N])
        

которая читает данные из файловых дескрипторов in[X] для всех 0 ≤ X < N ,
и возвращает суммарное количество прочитанных байт из всех файловых дескрипторов.

Скорость операций ввода-вывода у файловых дескрипторов - случайная.
Необходимо минимизировать суммарное астрономическое время чтения данных.

По окончании чтения необходимо закрыть все файловые дескрипторы.

Указание: используйте неблокирующий ввод-вывод. Для тестирования можно использовать socketpair.*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/wait.h>



const int32_t MAX_PENDING_EVENTS = 10000;
const int32_t MAX_BUF = 4096;

typedef struct {
    int32_t fd;
    size_t bytes_read;
    bool done;
} fd_data_t;



void
create_event_data(int32_t epoll_fd, int32_t fd, fd_data_t *data) {
    int32_t flags = fcntl(fd, F_GETFL);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);

    data->fd = fd;

    struct epoll_event event_ready_to_read;
    event_ready_to_read.events = EPOLLIN;
    event_ready_to_read.data.ptr = data;

    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event_ready_to_read);
}

void
read_all(fd_data_t *data) {
    char buffer[MAX_BUF];
    memset(buffer, 0, MAX_BUF);

    int32_t cur_bytes = read(data->fd, buffer, sizeof(buffer));
    // As we have level-triggered.
    if (cur_bytes > 0) {
        data->bytes_read += cur_bytes;
    } else if (0 == cur_bytes) {
        data->done = true;
        close(data->fd);
    }
}

void
process_event(struct epoll_event *event, size_t *remaining_fds) {
    uint32_t mask = event->events;
    fd_data_t *data = event->data.ptr;

    if (mask & EPOLLIN) {
        read_all(data);
    }

    if (data->done) {
        --(*remaining_fds);
    }
}

extern size_t
read_data_and_count(size_t N, int32_t in[N]) {
    int32_t epoll_fd = epoll_create1(0);

    fd_data_t *files_data = calloc(N, sizeof(fd_data_t));    
    for (int32_t i = 0; i < N; ++i) {
        create_event_data(epoll_fd, in[i], &files_data[i]);
    }

    struct epoll_event pending_events[MAX_PENDING_EVENTS];
    memset(pending_events, 0, MAX_PENDING_EVENTS * sizeof(struct epoll_event));
    size_t remaining_fds = N;
    while (remaining_fds > 0) {
        int32_t cur_events_num = epoll_wait(epoll_fd, pending_events, MAX_PENDING_EVENTS, -1);
        for (int32_t i = 0; i < cur_events_num; ++i) {
            process_event(&pending_events[i], &remaining_fds);
        }
    }

    close(epoll_fd);
    size_t sum_bytes = 0;
    for (int32_t i = 0; i < N; ++i) {
        sum_bytes += files_data[i].bytes_read;
    }
    free(files_data);

    return sum_bytes;
}

int32_t
main(int32_t argc, char **argv) {
    int32_t fd1 = open("test1.txt", O_CREAT | O_TRUNC | O_RDWR);
    int32_t fd2 = open("test2.txt", O_CREAT | O_TRUNC | O_RDWR);
    int32_t in[] = {fd1, fd2};
    printf("Descriptors are ready.\n");

    int32_t pid = fork();
    if (pid == 0) {
        size_t sum_bytes = read_data_and_count(2, in);
        printf("%ld\n", sum_bytes);
        fflush(stdout);
        return 0;
    } else {
        write(fd1, "Hello, this is the first file!", 31);
        sleep(1);
        write(fd2, "Hello, this is the second one!", 31);
        waitpid(pid, NULL, 0);
    }

    return 0;
}