/*Программе передаются два аргумента: имя файла, и строка для поиска.

Необходимо найти все вхождения строки в текстовом файле,
используя отображение на память с помощью системного вызова mmap.

На стандартный поток вывода вывести список всех позиций (с нуля) в файле, где втречаются строка.*/
#define _GNU_SOURCE

#include <stdio.h>
#include <stdint.h>
#include <sys/mman.h>   // mmap()
#include <string.h>     // memmem()
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int32_t main (int32_t argc, char **argv) {
    char *file_name = argv[1];
    char *pattern = argv[2];

    int32_t fd = open(file_name, O_RDONLY);
    struct stat stat_struct;
    fstat(fd, &stat_struct);
    int32_t file_size = stat_struct.st_size;

    void *file_address = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    int32_t pattern_size = strlen(pattern);
    void *entry_ptr = memmem(file_address, file_size, pattern, pattern_size);
    /*for (int32_t i = 0; i < file_size; ++i) {
        printf("%c", *((char*)(file_address + i)));
    }*/

    while(entry_ptr != NULL) {
        int64_t offset = entry_ptr - file_address;
        printf("%ld\n", offset);
        entry_ptr = memmem(entry_ptr + 1, file_size - (offset + 1), pattern, pattern_size);
    }

    munmap(file_address, file_size);
    close(fd);

    return 0;
}
