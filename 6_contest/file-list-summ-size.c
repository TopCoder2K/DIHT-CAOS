/*Программе на стандартном потоке ввода передаётся список имён файлов.

Необходимо посчитать суммарный размер в байтах всех файлов, которые являются регулярными.*/

#define _FILE_OFFSET_BITS 64    // To work with files which are bigger than 2 GB.

#include <stdio.h>
#include <stdint.h>
#include <unistd.h> // Provides access to POSIX's API.
// #include <fcntl.h>  // To use open.
#include <sys/types.h>
#include <sys/stat.h>
#include <endian.h> // To transform chars in int.
#include <string.h> // To memset.
#include <linux/limits.h> // To max path length.



uint64_t get_reg_file_size(const char *file_name) {
    struct stat st;

    lstat(file_name, &st);
    // printf("%s ", file_name);

    if (S_ISREG(st.st_mode)) {
        return st.st_size;
    }

    return 0;
}

uint64_t get_file_name() {
    uint64_t summary_size = 0;

    char file_name[PATH_MAX + 1];
    memset(file_name, 0, PATH_MAX + 1);

    while(fgets(file_name, PATH_MAX + 1, stdin) != NULL) {
        int32_t real_size = strlen(file_name);
        if (file_name[real_size - 1] == '\n')
            file_name[real_size - 1] = '\000';

        summary_size += get_reg_file_size(file_name);

        memset(file_name, 0, PATH_MAX + 1);
        // printf("%llu\n", summary_size);
    }

    return summary_size;
}

int32_t main(int32_t argc, char **argv) {
    printf("%llu", get_file_name());

    return 0;
}
