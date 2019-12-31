/*Программе передается аргумент - целое число N > 0.

Необходимо создать N-1 дополнительных процессов таким образом,
чтобы у каждого процесса было не более одного дочернего процесса.

Каждый из процессов должен вывести на стандартный поток ввода
ровно одно число таким образом, чтобы вы выходе получилась строка:

1 2 3 4 ... N

Внимание! В этой задаче будет сравнение с учетом пробельных символов.
Между числами должен быть ровно один пробел, а завершается строка символом перевода строки.*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>



int32_t main(int32_t argc, char **argv) {
    pid_t pid;
    int32_t counter = 0;
    int32_t N = atoi(argv[1]);

    while (counter < N - 1) {
        pid = fork();

        // Parent.
        if (pid) {
            waitpid(pid, NULL, 0);

            return 0;
        }

        ++counter;
        printf("%d ", counter);
        fflush(stdout);
    }

    ++counter;
    printf("%d\n", counter); 

    return 0;
}
