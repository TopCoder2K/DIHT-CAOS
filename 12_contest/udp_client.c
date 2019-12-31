/*Аргументом программы является целое число - номер порта на сервере localhost.

Программа читает со стандартного потока ввода целые числа в текстовом формате,
и отправляет их в бинарном виде (little-endian) на сервер как UDP-сообщение.

В ответ сервер отправляет целое число (также в бинарном виде, little-endian),
которое необходимо вывести на стандартный поток вывода.*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h> // To convert LE <-> BE and parsing ip address.
#include <signal.h>



int32_t main(int32_t argc, char **argv) {
    const char ip4_addr[] = "127.0.0.1";
    in_port_t port = htons(atoi(argv[1]));

    signal(SIGPIPE, SIG_IGN);

    int32_t sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sd == -1) {
        perror("Socket creation failed");
        return 1;
    }

    struct sockaddr_in full_addr;
    full_addr.sin_family = AF_INET;
    full_addr.sin_port = port;
    full_addr.sin_addr.s_addr = inet_addr(ip4_addr);
    bind(sd, (const struct sockaddr*) &full_addr, sizeof(full_addr));

    int32_t to_send_num = 0;
    int32_t to_recv_num = 0;
    while (scanf("%d", &to_send_num) != EOF) {
        sendto(sd, &to_send_num, sizeof(to_send_num), 0,
        (const struct sockaddr *) &full_addr, sizeof(full_addr));

        recvfrom(sd, &to_recv_num, sizeof(to_recv_num), 0, NULL, NULL);

        printf("%d ", to_recv_num);
        fflush(stdout);
    }
    close(sd);

    return 0;
}
