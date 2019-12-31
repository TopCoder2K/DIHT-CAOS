/*Реализуйте программу, которая принимает два аргумента:
CMD - команда для запуска, IN - имя файла, направляемого на ввод.

Программа должна запускать указанную команду,
передавая ей на вход содежимое файла IN.

На стандартный поток вывода вывести количество байт,
которое было записано запущенной командой в стандартный поток вывода.
Вывод самой команды никуда выводить не нужно.*/

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>



const int32_t BUF_SIZE = 4096;

int32_t main(int32_t argc, char **argv) {
    char *cmd = argv[1];
    char *input_name = argv[2];

    // As we shouldn't print in stdout output of the program, we will write it the pipe.
    int32_t pipe_pair[2];
    pipe(pipe_pair);

    pid_t pid = fork();

    if (pid == 0) {
        // Child won't read from the pipe.
        close(pipe_pair[0]);

        int32_t fd_in = open(input_name, O_RDONLY | O_CLOEXEC);
        dup2(fd_in, STDIN_FILENO);
        close(fd_in);

        dup2(pipe_pair[1], STDOUT_FILENO);
        close(pipe_pair[1]);

        execl(cmd, cmd, NULL);
        perror("Execlp failed");
    } else {
        // Parent won't write to the pipe.
        close(pipe_pair[1]);

        size_t total_bytes_count = 0;
        size_t cur_bytes_count = 0;
        char tmp_buf[BUF_SIZE];

        while ((cur_bytes_count = read(pipe_pair[0], tmp_buf, BUF_SIZE)) > 0) {
            total_bytes_count += cur_bytes_count;
        }
        wait(NULL);

        printf("%lu", total_bytes_count);
    }

    return 0;
}
