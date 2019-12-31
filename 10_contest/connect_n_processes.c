/*Программе передаётся произвольное количество аргументов: CMD1, CMD2, ..., CMDN.

Необходимо реализовать эквивалент запуска их командной строки: CMD1 | CMD2 | ... | CMDN.

Родительский процесс должен завершаться самым последним!*/

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>



const int32_t BUF_SIZE = 4096;

void set_in_and_out(int32_t fd_in, char *file_nm_out) {
    dup2(fd_in, STDIN_FILENO);
    lseek(fd_in, 0, SEEK_SET);
    close(fd_in);

    int32_t fd2 = open(file_nm_out, O_CREAT | O_RDWR | O_TRUNC, 0777);
    dup2(fd2, STDOUT_FILENO);
    close(fd2);
}

void set_last_in(int32_t fd1, int32_t fd2) {
    close(fd2);

    dup2(fd1, STDIN_FILENO);
    lseek(fd1, 0, SEEK_SET);
    close(fd1);
}

int32_t main(int32_t argc, char **argv) {
    // We will have two files.
    // The plan is CMD1->(file1)->CMD2->(file2)->CMD3->(file1)->CMD4->...
    char file_name1[] = "tmp_fd1.txt";
    char file_name2[] = "tmp_fd2.txt";
    int32_t fd1 = open(file_name1, O_CREAT | O_RDWR | O_TRUNC, 0777);
    int32_t fd2 = open(file_name2, O_CREAT | O_RDWR | O_TRUNC, 0777);

    if (fd1 == -1 || fd2 == -2) {
        perror("Open failed");
    }

    for (int32_t i = 1; i < argc; ++i) {
        char *cmd_i = argv[i];

        pid_t pid = fork();

        if (pid == 0) {
            // cmd_1 won't read from the files, but others will.
            if (i == 1) {
                close(fd1);

                dup2(fd2, STDOUT_FILENO);
                close(fd2);
            } else if (i == argc - 1) {
                if (i % 2 != 0) {
                    set_last_in(fd1, fd2);
                } else {
                    set_last_in(fd2, fd1);
                }
            } else {
                if (i % 2 != 0) {
                    set_in_and_out(fd1, file_name2);
                } else {
                    set_in_and_out(fd2, file_name1);
                }
            }

            execlp(cmd_i, cmd_i, NULL);
            perror("Execlp failed");
        } else {
            waitpid(pid, NULL, 0);
            // sleep(10);
        }
    }

    close(fd1);
    close(fd2);

    return 0;
}

