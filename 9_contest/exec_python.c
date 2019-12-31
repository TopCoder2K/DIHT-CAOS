/*Программе на стандартный поток ввода задается некоторое арифметическое выражение
в синтаксисе языка python3.

Необходимо вычислисть это выражение, и вывести результат.

Использовать дополнительные процессы запрещено.*/

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>



const int32_t MAX_BUF_SIZE = 4096;

int32_t main(int32_t argc, char **argv) {
    char expr[MAX_BUF_SIZE];

    memset(expr, 0, MAX_BUF_SIZE);
    fgets(expr, MAX_BUF_SIZE, stdin);

    char expr_to_execute[MAX_BUF_SIZE];
    sprintf(expr_to_execute, "print(%s)", expr);

    execlp("python3", "python3", "-c", expr_to_execute, NULL);

    perror("Failed to execute");

    return 1;
}