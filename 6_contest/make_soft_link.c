/*Программе на стандартном потоке ввода передаётся список имён файлов.

Если файл является символической ссылкой,
то нужно вывести абсолютное имя того файла, на который эта ссылка указывает.

Если файл является регулярным,
то необходимо создать символическую ссылку в текущем каталоге,
приписав в названии файла префикс link_to_.*/

#define _FILE_OFFSET_BITS 64    // To work with files which are bigger than 2 GB.

#include <stdio.h>
#include <stdint.h>
#include <unistd.h> // Provides access to POSIX's API.
// #include <fcntl.h>  // To use open.
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h> // To memset.
#include <linux/limits.h> // To max path length.
#include <stdlib.h> // To realpath() function.



void make_soft_link(const char *file_name, uint64_t real_size) {
    int32_t offset = 8;
    const char *prefix = "link_to_";

    char exact_file_name[PATH_MAX + 1];
    memset(exact_file_name, 0, PATH_MAX + 1);

    // Get exact file name.
    strcpy(exact_file_name, prefix);
    for (int i = real_size - 1; i >= 0; --i) {
        if (file_name[i] == '/') break;

        exact_file_name[i + offset] = file_name[i];
    }

    printf("%d\n", real_size);
    printf("Exact name: %s\n", exact_file_name);

    symlink(file_name, exact_file_name);
}

void print_resolved_path(const char *file_name) {
    char resolved_path[PATH_MAX + 1];

    realpath(file_name, resolved_path);
    printf("%s\n", resolved_path);
}

void get_files_names_and_check() {
    char file_name[PATH_MAX + 1];
    memset(file_name, 0, PATH_MAX + 1);

    while(fgets(file_name, PATH_MAX + 1, stdin) != NULL) {
        uint64_t real_size = strlen(file_name);
        if (file_name[real_size - 1] == '\n') {
            file_name[real_size - 1] = '\000';
            --real_size;
        }

        struct stat st;
        lstat(file_name, &st);
        if (S_ISREG(st.st_mode)) {
            make_soft_link(file_name, real_size);
        } else if (S_ISLNK(st.st_mode)) {
            print_resolved_path(file_name);
        }

        memset(file_name, 0, PATH_MAX + 1);
    }
}

int32_t main(int32_t argc, char **argv) {
    get_files_names_and_check();

    return 0;
}
