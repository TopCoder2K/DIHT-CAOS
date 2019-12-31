/*Программе в качестве аргумента передается имя файла программы на языке Си.
Необходимо попытаться её скомпилировать с помощью штатного компилятора gcc,
после чего вывести на стандартный поток вывода:
количество строк программы с ошибками (error),
и количество строк программы с предупреждениями (warning).
В одной строке может быть найдено несколько ошибок или предупреждений,
- нужно вывести именно количество строк.

Запрещено создавать временные файлы для сохранения вывода ошибок компилятора.
Используйте передачу текста от компилятора через каналы.*/

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>



const int32_t BUF_SIZE = 4096;
const int32_t MAX_STR_NUM = 10001;

int32_t main(int32_t argc, char **argv) {
    const char *cmd = argv[1];

    // We will connect two programs by pipe.
    int32_t pipe_pair[2];
    pipe(pipe_pair);

    pid_t pid = fork();

    if (pid == 0) {
        // Child won't read from the pipe.
        close(pipe_pair[0]);

        dup2(pipe_pair[1], STDERR_FILENO);
        close(pipe_pair[1]);

        execlp("gcc", "gcc", cmd, NULL);
        perror("Execlp1 failed");
    } else {
        // Parent won't write to the pipe.
        close(pipe_pair[1]);

        dup2(pipe_pair[0], STDIN_FILENO);
        close(pipe_pair[0]);

        int32_t w_count = 0;
        int32_t bad_strings_w[MAX_STR_NUM];
        memset(bad_strings_w, -1, MAX_STR_NUM);
        int32_t e_count = 0;
        int32_t bad_strings_e[MAX_STR_NUM];
        memset(bad_strings_e, -1, MAX_STR_NUM);

        const char warning_[] = "warning:";
        const char error_[] = "error:";
        const char fatal_[] = "fatal";
        int32_t str_num = -1;
        int32_t pos_in_str = -1;
        const char position_[] = ":%d:%d:";

        char cur_word[BUF_SIZE];
        memset(cur_word, 0, BUF_SIZE);
        char format[strlen(cmd) + strlen(position_) + 1];
        format[0] = '\0';
        strcat(format, cmd);
        strcat(format, position_);


        while (scanf("%s", cur_word) != EOF) {
            int32_t result = sscanf(cur_word, format, &str_num, &pos_in_str);

            if (result == 2) {
                // printf("%s ", cur_word);
                scanf("%s", cur_word);

                if (strcmp(cur_word, warning_) == 0 && bad_strings_w[str_num] == -1) {
                    ++w_count;
                    bad_strings_w[str_num] = str_num;
                }
                if ((strcmp(cur_word, error_) == 0 || strcmp(cur_word, fatal_) == 0)
                    && bad_strings_e[str_num] == -1) {
                    ++e_count;
                    bad_strings_e[str_num] = str_num;
                }

                // printf("%d %d\n", str_num, pos_in_str);
            }
            
            str_num = -1;
            // memset(cur_word, 0, BUF_SIZE + 2);
        }

        waitpid(pid, NULL, 0);

        printf("%d %d", e_count, w_count);
    }

    return 0;
}
