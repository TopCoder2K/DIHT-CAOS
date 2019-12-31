/*Программе в качестве аргументов передаются N имен текстовых файлов.

Программа должна обрабатывать множество сигналов от SIGRTMIN до SIGRTMAX,
причем номер сигнала в диапазоне от SIGRTMIN+1 определяет порядковый номер файла из аргументов:

x = signo - SIGRTMIN; // SIGRTMIN <= signo <= SIGRTMAX
                      // 1 <= x <= SIGRTMAX-SIGRTMIN

При получении очередного сигнала необходимо прочитать
одну строку из определенного файла и вывести её на стандартный поток вывода.

При получении сигнала с номером SIGRTMIN, т.е. при номере аргумента, равным 0,
- корректно завершить свою работу с кодом 0.

Все остальные сигналы нужно игнорировать.

Если для вывода используются высокоуровневые функции стандартной библиотеки Си,
то необходимо выталкивать буфер обмена после вывода каждой строки.*/

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/signalfd.h>



const int32_t BUF_SIZE = 4096;

void print_file(FILE *from) {
    char cur_str[BUF_SIZE];
    memset(cur_str, 0, BUF_SIZE);

    fgets(cur_str, BUF_SIZE, from);
    fputs(cur_str, stdout);
    fflush(stdout);
}

int32_t main(int32_t argc, char **argv) {
    FILE *files[argc - 1];
    for (int32_t i = 1; i < argc; ++i) {
        files[i - 1] = fopen(argv[i], O_RDONLY);
    }

    sigset_t block_all;
    sigfillset(&block_all);
    sigprocmask(SIG_BLOCK, &block_all, NULL);

    sigset_t rt_mask;
    sigemptyset(&rt_mask);
    for (int32_t i = 0; i < argc; ++i) {
        sigaddset(&rt_mask, SIGRTMIN + i);
    }
    int32_t fd = signalfd(-1, &rt_mask, 0);

    // FOR TESTING.
    printf("%d\n", getpid());
    fflush(stdout);

    struct signalfd_siginfo sig_info;
    int32_t index = 0;
    while (1) {
        read(fd, &sig_info, sizeof(sig_info));
        index = sig_info.ssi_signo - SIGRTMIN;

        if (index == 0) {
            break;
        }
        print_file(files[index - 1]);
    }

    close(fd);
    for (int32_t i = 1; i < argc; ++i) {
        fclose(files[i - 1]);
    }

    return 0;
}
