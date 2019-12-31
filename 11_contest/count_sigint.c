/*Программа при запуске сообщает на стандартный поток вывода свой PID,
выталкивает буфер вывода с помощью fflush,
после чего начинает обрабатывать поступающие сигналы.

При поступлении сигнала SIGTERM необходимо вывести на стандартный поток вывода целое число:
количество ранее поступивших сигналов SIGINT и завершить свою работу.

Семантика повединия сигналов (Sys-V или BSD) считается не определенной.*/

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>



volatile sig_atomic_t int_counter = 0;
volatile sig_atomic_t must_exit = 0;

void sigint_handler(int32_t sig_num) {
    ++int_counter;
}

void sigterm_handler(int32_t sig_num) {
    must_exit = 1;
}

int32_t main(int32_t argc, char **argv) {
    struct sigaction sig_act;
    memset(&sig_act, 0, sizeof(sig_act));

    sig_act.sa_handler = sigint_handler;
    sig_act.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sig_act, NULL);

    sig_act.sa_handler = sigterm_handler;
    sig_act.sa_flags = SA_RESTART;
    sigaction(SIGTERM, &sig_act, NULL);

    printf("%d\n", getpid());
    fflush(stdout);

    while (!must_exit) {
        pause();
    }

    // What if signal?
    printf("%d\n", int_counter);

    return 0;
}
