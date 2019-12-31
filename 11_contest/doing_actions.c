/*Программа при запуске сообщает на стандартный поток вывода свой PID,
после чего читает со стандартного потока вывода целое число - начальное значение,
которое затем будет изменяться.

При поступлении сигнала SIGUSR1 увеличить текущее значение на 1
и вывести его на стандартный поток вывода.

При поступлении сигнала SIGUSR2 - умножить текущее значение на -1
и вывести его на стандартный поток вывода.

Семантика повединия сигналов (Sys-V или BSD) считается не определенной.

Не забывайте выталкивать буфер вывода.*/

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>



volatile sig_atomic_t answer = 0;
volatile sig_atomic_t must_exit = 0;
volatile sig_atomic_t caught_sig = 0;

void usr1_handler(int32_t sig_num) {
    ++answer;
    caught_sig = 1;
}

void usr2_handler(int32_t sig_num) {
    answer *= -1;
    caught_sig = 1;
}

void term_handler(int32_t sig_num) {
    must_exit = 1;
    caught_sig = 0;
}

int32_t main(int32_t argc, char **argv) {
    struct sigaction sig_act;
    memset(&sig_act, 0, sizeof(sig_act));

    sig_act.sa_handler = usr1_handler;
    sig_act.sa_flags = SA_RESTART;
    sigaction(SIGUSR1, &sig_act, NULL);

    sig_act.sa_handler = usr2_handler;
    sig_act.sa_flags = SA_RESTART;
    sigaction(SIGUSR2, &sig_act, NULL);

    sig_act.sa_handler = term_handler;
    sig_act.sa_flags = SA_RESTART;
    sigaction(SIGTERM, &sig_act, NULL);

    printf("%d\n", getpid());
    fflush(stdout);
    scanf("%d", &answer);

    while (!must_exit) {
        pause();

        if (caught_sig == 1) {
            printf("%d\n", answer);
            fflush(stdout);

            caught_sig = 0;
        }
    }

    return 0;
}
