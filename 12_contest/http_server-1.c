/*Необходимо реализовать программу-сервер, которой передаются два аргумента:
номер порта и полный путь к каталогу с данными.

Программа должна прослушивать TCP-соединение на сервере localhost и указанным номером порта.

После получения сигнала SIGTERM или SIGINT
сервер должен закончить обработку текущего соединения, если оно есть,
после чего корректно завершить свою работу.

Внимание: в этой задаче признаком конца строк считается пара символов "\r\n",
а не одиночный символ '\n'.

Каждое соединение должно обрабатываться следующим образом:

    Клиент отправляет строку вида GET ИМЯ_ФАЙЛА HTTP/1.1
    Клиент отправляет произвольное количество непустых строк
    Клиент отправляет пустую строку

После получения пустой строки от клиента, сервер должен отправить клиенту следующие данные:

    Строку HTTP/1.1 200 OK, если файл существует,
    или HTTP/1.1 404 Not Found, если файл не существует,
    или HTTP/1.1 403 Forbidden, если файл существует, но не доступен для чтения;
    Строку Content-Length: %d, где %d - размер файла в байтах
    Пустую строку
    Содержимое файла as-is

После отправки ответа клиенту,
нужно закрыть соединение и не нужно ждать от клиента следующих запросов.*/

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h> // To convert LE <-> BE and parsing ip address.



const int32_t BUF_SIZE = 4096;


int32_t check_path(const char *path) {
    // Check file existance.
    int32_t result = access(path, F_OK);
    if (result == -1) {
        return -1;
    }

    // Check the ability to read the file.
    result = access(path, R_OK);
    if (result == -1) {
        return -2;
    }

    return 0;
}

int64_t get_file_size(const char *path) {
    struct stat stat_struct;
    int32_t res = lstat(path, &stat_struct);
    if (res == -1) {
        perror("Stat failed");
        exit(1);
    }

    return stat_struct.st_size;
}

void send_http(const int32_t sd, int32_t result, const char *path) {
    // Form the first string.
    char http_header[BUF_SIZE];
    memset(http_header, 0, BUF_SIZE);
    strcat(http_header, "HTTP/1.1 ");

    switch (result) {
        case -1:
            strcat(http_header, "404 Not Found\r\n");
            break;
        case -2:
            strcat(http_header, "403 Forbidden\r\n");
            break;
        default:
            strcat(http_header, "200 OK\r\n");
    }
    // printf("First string was formed: %s\n", http_header);
    write(sd, http_header, strlen(http_header));
    memset(http_header, 0, BUF_SIZE / 100);         // There aren't more than 40 bytes.

    if (result < 0) {
        strcat(http_header, "Content-Length: 0\r\n\r\n");
        dprintf(sd, "%s", http_header);
        // printf("%s", http_header);
        return;
    }

    // Form the second string.
    int32_t fd = open(path, O_RDONLY);
    if (fd == -1) {
        perror("Failed to open file");
        exit(1);
    }

    int64_t file_size = 0;
    file_size = get_file_size(path);

    strcat(http_header, "Content-Length: %ld\r\n\r\n");
    dprintf(sd, http_header, file_size);
    // printf(http_header, file_size);
    memset(http_header, 0, BUF_SIZE / 100);         // There aren't more than 40 bytes.

    // Print the file if it exists and it's permitted to read.
    int32_t was_read = 0;
    while ((was_read = read(fd, http_header, BUF_SIZE)) > 0) {
        write(sd, http_header, was_read);
        // printf("%s\n", http_header);
    }

    close(fd);
}

void process_conn(const int32_t sd, const char *path_to_dir) {
    char cur_str[BUF_SIZE];
    char file_name[BUF_SIZE];
    memset(cur_str, 0, BUF_SIZE);
    memset(file_name, 0, BUF_SIZE);
    int32_t path_to_dir_len = strlen(path_to_dir);
    memcpy(file_name, path_to_dir, path_to_dir_len);
    file_name[path_to_dir_len] = '/';   // Because edjudge sends without it the file name.

    // The file descriptor is not dup'ed, and will be closed
    // when the stream created  by  fdopen()  is  closed.
    int32_t sd_copy = dup(sd);
    FILE *sock_stream = fdopen(sd_copy, "r");

    // Parse the first string and get the file name.
    fscanf(sock_stream, "GET %s HTTP/1.1", cur_str);
    strcat(file_name, cur_str);
    // printf("File name: %s\n", file_name);

    // Notify about closing the reading.
    shutdown(sd, SHUT_RD);
    // Others string are just read.
    /*fscanf(sock_stream, "\r\n\r\n");
    printf("Information was read.\n");*/

    int32_t result = check_path(file_name);
    // printf("Path was checked: %d\n", result);
    send_http(sd, result, file_name);

    // printf("Information was sent.\n");
    // Closing the sending.
    shutdown(sd, SHUT_WR);
    fclose(sock_stream);    // sd_copy closed.
}
//---------------------------------------------------------------------------------------

sig_atomic_t stop = 1;
sig_atomic_t is_in_access = 0;

void int_handler(int32_t sig_num) {
    if (!is_in_access) {
        stop = 0;
    } else {
        _exit(0);
    }
}

void term_handler(int32_t sig_num) {
    if (!is_in_access) {
        stop = 0;
    } else {
        _exit(0);
    }
}

void setup_signals(void) {
    struct sigaction sig_act;
    memset(&sig_act, 0, sizeof(sig_act));

    sig_act.sa_handler = int_handler;
    sig_act.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sig_act, NULL);

    sig_act.sa_handler = term_handler;
    sig_act.sa_flags = SA_RESTART;
    sigaction(SIGTERM, &sig_act, NULL);
}
//---------------------------------------------------------------------------------------

int32_t main(int32_t argc, char **argv) {
    in_port_t port = htons(atoi(argv[1]));
    const char *path_to_dir = argv[2];
    in_addr_t ip4_addr = inet_addr("127.0.0.1");

    int32_t lstning_sd = socket(AF_INET, SOCK_STREAM, 0);
    if (lstning_sd == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    struct sockaddr_in full_addr;
    full_addr.sin_family = AF_INET;
    full_addr.sin_port = port;
    full_addr.sin_addr.s_addr = ip4_addr;

    if (bind(lstning_sd, (const struct sockaddr*) &full_addr, sizeof(full_addr)) != 0) {
        perror("Bind failed");
        exit(1);
    }
    if (listen(lstning_sd, SOMAXCONN) != 0) {
        perror("Listening failed");
        exit(1);
    }

    // Before getting the connected socket we should take into account signals.
    setup_signals();

    while (stop) {
        socklen_t addr_len = sizeof(full_addr);
        is_in_access = 1;
        int32_t conn_sd = accept(lstning_sd, (struct sockaddr*) &full_addr, (socklen_t *) &addr_len);
        if (conn_sd == -1) {
            perror("Acception failed");
            exit(1);
        }
        is_in_access = 0;

        process_conn(conn_sd, path_to_dir);
        close(conn_sd);
    }

    close(lstning_sd);

    return 0;
}
