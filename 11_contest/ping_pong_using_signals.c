/*Программа должна обрабатывать сигнал SIGRTMIN,
вместе с которым передается некоторое беззнаковой 32-битное целое число N.

Все остальные сигналы нужно игнорировать.

При получении очередного сигнала нужно уменьшить это число на единицу
и отправить его обратно тому процессу, который его послал (используя тот же самый сигнал).

Взаимодействие останавливается при получении значения N==0, после чего нужно корректно завершить работу.

Запрещается использовать signalfd, программа будет компилироваться без поддержки Linux-специфичных расширений.*/

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>



volatile sig_atomic_t N = 0;
volatile pid_t pid = 0;

void rtmin_handler(int32_t sig_num, siginfo_t *sig_info, void *env) {
    N = sig_info->si_value.sival_int;
    // printf("%d\n", N);
    --N;
    pid = sig_info->si_pid;
    // printf("%d\n", pid);
}

int32_t main(int32_t argc, char **argv) {
    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGRTMIN);
    sigprocmask(SIG_BLOCK, &mask, NULL);

    struct sigaction sig_act;
    memset(&sig_act, 0, sizeof(sig_act));
    sig_act.sa_sigaction = rtmin_handler;
    sig_act.sa_mask = mask;
    sig_act.sa_flags = SA_RESTART | SA_SIGINFO;
    sigaction(SIGRTMIN, &sig_act, NULL);

    // FOR TESTING.
    /*printf("%d\n", getpid());
    fflush(stdout);*/

    union sigval s_val;

    while (1) {
        pause();

        if (N == 0) {
            break;
        }

        s_val.sival_int = N;
        sigqueue(pid, SIGRTMIN, s_val);
    }

    return 0;
}
