/*Программе передаются два аргумента:
строка с IPv4-адресом в стандартной десятичной записи (четыре числа, разделенные точкой), и номер порта.

Программа должна установить соединение с указанным сервером,
после чего читать со стандартного потока ввода целые знаковые числа в текстовом формате,
и отправлять их в бинарном виде на сервер. Порядок байт - Little Endian.

В ответ на каждое число полученное число, сервер отправляет целое число (в таком же формате),
и все эти числа необходимо вывести на стандартный поток вывода в текстовом виде.

Если сервер по своей инициативе закроет соединение, то нужно завершить работу с кодом возврата 0.*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h> // To convert LE <-> BE and parsing ip address.



int32_t main(int32_t argc, char **argv) {
    in_addr_t ip4_addr = inet_addr(argv[1]);
    in_port_t port = htons(atoi(argv[2]));

    struct sockaddr_in full_addr;
    full_addr.sin_family = AF_INET;
    full_addr.sin_port = port;
    full_addr.sin_addr.s_addr = ip4_addr;

    int32_t sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd == -1) {
        perror("Socket creation failed");
        return 1;
    }

    if (connect(sd, (const struct sockaddr*) &full_addr, sizeof(full_addr)) != 0) {
        perror("Connection failed");
        close(sd);
        return 1;
    }

    int32_t to_send_num = 0;
    int32_t to_recv_num = 0;
    while (scanf("%d", &to_send_num) != EOF) {
        if (write(sd, &to_send_num, sizeof(to_send_num)) <= 0) {
            // Server closed connection.
            break;
        }
        if (read(sd, &to_recv_num, sizeof(to_recv_num)) <= 0) {
            // Server closed connection.
            break;
        }

        printf("%d ", to_recv_num);
        fflush(stdout);
    }
    close(sd);

    return 0;
}
