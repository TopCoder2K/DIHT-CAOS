/*На стандартном потоке ввода задается строка текста,
которая состоит из слов (последовательности непробельных символов),
между которыми может быть произвольное количество пробельных символов,
включая перевод строки.

Необходимо посчитать количество слов, если изветно, что их не больше, чем 255,
и вывести это значение на стандартный поток вывода.

Используйте создание новых процессов таким образом,
чтобы каждый процесс читал не более одного слова, например, c помощью scanf("%s", ...).

Вывод результата возможен только из того процесса,
который запущен самым первым (т.е. из исходной программы).

Итоговая программа должна вернуться с кодом возврата 0.

Размер каждого слова не превышает 4096 байт.*/

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>



const int32_t MAX_NAME_SIZE = 4098;

int32_t main(int32_t argc, char **argv) {
    pid_t pid;
    char string[MAX_NAME_SIZE];
    int32_t counter = 0;

    memset(string, 0, MAX_NAME_SIZE);

    while (scanf("%s", string) != EOF) {
        pid = fork();

        // Parent.
        if (pid) {
            int32_t wstatus;
            waitpid(pid, &wstatus, 0);

            // If we are in the first process.
            if (counter == 0) {
                printf("%d\n", WEXITSTATUS(wstatus));
                fflush(stdout);

                return 0;
            }

            return WEXITSTATUS(wstatus);
        }

        ++counter;
        memset(string, 0, MAX_NAME_SIZE);
    }

    return counter;
}