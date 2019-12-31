/*Программе передаётся два аргумента: CMD1 и CMD2.
Необходимо запустить два процесса, выполняющих эти команды,
и перенаправить стандартный поток вывода CMD1 на стандартный поток ввода CMD2.

В командной строке это эквивалентно CMD1 | CMD2.

Родительский процесс должен завершаться самым последним!*/

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>



const int32_t BUF_SIZE = 4096;

int32_t main(int32_t argc, char **argv) {
    char *cmd1 = argv[1];
    char *cmd2 = argv[2];

    // We will connect two programs by pipe.
    int32_t pipe_pair[2];
    pipe(pipe_pair);

    pid_t pid = fork();

    if (pid == 0) {
        // Child won't read from the pipe.
        close(pipe_pair[0]);

        dup2(pipe_pair[1], STDOUT_FILENO);
        close(pipe_pair[1]);

        execlp(cmd1, cmd1, NULL);
        perror("Execlp1 failed");
    } else {
        // Parent won't write to the pipe.
        close(pipe_pair[1]);

        pid_t second_pid = fork();
        if (second_pid == 0) {           
            dup2(pipe_pair[0], STDIN_FILENO);
            close(pipe_pair[0]);

            execlp(cmd2, cmd2, NULL);
            perror("Execlp2 failed");
        } else {
            close(pipe_pair[0]);
            waitpid(pid, NULL, 0);
            waitpid(second_pid, NULL, 0);
        }
    }

    return 0;
}
