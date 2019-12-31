/*Программе на стандартном потоке ввода задается выражение в синтаксисе языка Си.

Необходимо вычислить значение этого выражения
(итоговый результат представим типом int) и вывести его на стандартный поток вывода.*/

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>



const int32_t MAX_BUF_SIZE = 4096;

int32_t main(int32_t argc, char **argv) {
    char expr[MAX_BUF_SIZE + 2];

    fgets(expr, MAX_BUF_SIZE, stdin);

    // If input is just "\n" or empty, program will work incorrectly.
    char *new_line = strchr(expr, '\n');
    if (new_line) {
        *new_line = 0;
    }

    if (!strlen(expr)) return 0;

    // We have to make a new expr.c file.
    char *program_name = "calc_expr.c";
    char *exe_name = "exec";
    pid_t pid = fork();

    if (!pid) {
        remove(program_name);

        int32_t fd = open(program_name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
        char *program = "#include <stdio.h>\n"
                    "\n"
                    "int main() {\n"
                    "   int res = (%s);\n"
                    "   printf(\"%%d\", res);\n"
                    "   return 0;\n"
                    "}\n";

        dprintf(fd, program, expr);
        close(fd);

        // Compile.
        remove(exe_name);
        execlp("gcc", "gcc", program_name, "-o", exe_name, NULL);

        perror("Failed to compile");
        return 1;
    } else {
        waitpid(pid, NULL, 0);

        // Execute.
        pid = fork();
        if (!pid) {
            char *exe_n = "./exec";
            execlp(exe_n, exe_n, NULL);
            
            perror("Failed to execute");
            return 1;
        } else {
            waitpid(pid, NULL, 0);

            return 0;
        }
    }

    // clean up?

    return 0;
}
