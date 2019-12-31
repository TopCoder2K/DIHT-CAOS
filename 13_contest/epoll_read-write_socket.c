/*Программе задается единственный аргумент - номер TCP-порта.

Необходимо принимать входящие соединения на TCP/IPv4 для сервера localhost,
читать данные от клиентов в текстовом виде,
и отправлять их обратно в текстовом виде,
заменяя все строчные буквы на заглавные.
Все обрабатываемые символы - из кодировки ASCII.

Одновременных подключений может быть много.
Использовать несколько потоков или процессов запрещено.

Сервер должен корректно завершать работу при получении сигнала SIGTERM.

Указание: используйте неблокирующий ввод-вывод.*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <arpa/inet.h>



const int32_t MAX_PENDING_EVENTS = 10000;
const int32_t MAX_BUF = 4096;

//---------------------------------------------------------------------------------------
sig_atomic_t stop = 1;
sig_atomic_t is_in_accept = 0;

void int_handler(int32_t sig_num) {
    if (!is_in_accept) {
        stop = 0;
    } else {
        _exit(0);
    }
}

void term_handler(int32_t sig_num) {
    if (!is_in_accept) {
        stop = 0;
    } else {
        _exit(0);
    }
}

void setup_signals(void) {
    struct sigaction sig_act;
    memset(&sig_act, 0, sizeof(sig_act));

    sig_act.sa_handler = int_handler;
    sig_act.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sig_act, NULL);

    sig_act.sa_handler = term_handler;
    sig_act.sa_flags = SA_RESTART;
    sigaction(SIGTERM, &sig_act, NULL);
}
//---------------------------------------------------------------------------------------

void
create_event_data(const int32_t epoll_fd, int32_t fd) {
    int32_t flags = fcntl(fd, F_GETFL);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);

    struct epoll_event event_ready_to_read;
    memset(&event_ready_to_read, 0, sizeof(struct epoll_event));
    event_ready_to_read.events = EPOLLIN;
    event_ready_to_read.data.fd = fd;

    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event_ready_to_read);
}

void
delete_event_data(const int32_t epoll_fd, int32_t fd) {
    struct epoll_event event_ready_to_read;
    memset(&event_ready_to_read, 0, sizeof(struct epoll_event));
    event_ready_to_read.events = EPOLLIN;
    event_ready_to_read.data.fd = fd;

    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, &event_ready_to_read);
}

bool
process_event(struct epoll_event *event, const int32_t server_sd) {
    uint32_t mask = event->events;
    int32_t fd = event->data.fd;
    bool is_epoll_fd = 0;

    if (mask & EPOLLIN) {
        if (fd == server_sd) {
            is_epoll_fd = 1;
        }
    }

    return is_epoll_fd;
}

void
make_accept(struct sockaddr *full_addr, const int32_t lstning_sd, const int32_t epoll_fd) {
    socklen_t addr_len = sizeof(full_addr);
    is_in_accept = 1;
    int32_t conn_sd = accept(lstning_sd, full_addr, (socklen_t *) &addr_len);
    if (conn_sd == -1) {
        perror("Acception failed");
        exit(1);
    }
    is_in_accept = 0;

    printf("New connection was created: fd = %d.\n", conn_sd);
    create_event_data(epoll_fd, conn_sd);
}

void
make_upper_case(char *buf, size_t size) {
    for (int32_t i = 0; i < size; ++i) {
        if (buf[i] < 'A' || buf[i] > 'Z') {
            buf[i] = (char)toupper(buf[i]);
        }
    }
}

void
receive_and_send(int32_t sd, const int32_t epoll_fd) {
    char recieved[MAX_BUF];
    memset(recieved, 0, MAX_BUF);

    int32_t was_read = 0;
    while ((was_read = read(sd, recieved, MAX_BUF)) > 0) {
        make_upper_case(recieved, was_read);
        write(sd, recieved, was_read);
    }
    if (was_read == 0) {
        printf("Connection was ended.\n");
        shutdown(sd, SHUT_RDWR);
        delete_event_data(epoll_fd, sd);
        close(sd);
    }
}
//---------------------------------------------------------------------------------------

int32_t main(int32_t argc, char **argv) {
    // Before we start we should take into account signals.
    setup_signals();


    // Create the listning socket.
    in_port_t port = htons(atoi(argv[1]));
    in_addr_t ip4_addr = inet_addr("127.0.0.1");

    int32_t lstning_sd = socket(AF_INET, SOCK_STREAM, 0);
    if (lstning_sd == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    struct sockaddr_in full_addr;
    full_addr.sin_family = AF_INET;
    full_addr.sin_port = port;
    full_addr.sin_addr.s_addr = ip4_addr;

    if (bind(lstning_sd, (const struct sockaddr*) &full_addr, sizeof(full_addr)) != 0) {
        perror("Bind failed");
        exit(1);
    }
    if (listen(lstning_sd, SOMAXCONN) != 0) {
        perror("Listening failed");
        exit(1);
    }


    int32_t epoll_fd = epoll_create1(0);
    create_event_data(epoll_fd, lstning_sd);

    struct epoll_event pending_events[MAX_PENDING_EVENTS];
    memset(pending_events, 0, MAX_PENDING_EVENTS * sizeof(struct epoll_event));
    printf("Start looping.\n");
    while (stop) {
        int32_t cur_events_num = epoll_wait(epoll_fd, pending_events, MAX_PENDING_EVENTS, -1);
        printf("Number of new events: %d.\n", cur_events_num);

        for (int32_t i = 0; i < cur_events_num; ++i) {
            bool result = process_event(&pending_events[i], lstning_sd);
            printf("Result: %d.\n", (int32_t)result);
            if (result) {
                make_accept((struct sockaddr *) &full_addr, lstning_sd, epoll_fd);
            } else {
                receive_and_send(pending_events[i].data.fd, epoll_fd);
                printf("String was sent.\n");
            }
        }
    }

    close(epoll_fd);
    close(lstning_sd);

    return 0;
}
