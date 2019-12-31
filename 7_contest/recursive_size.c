/*Программе в качестве аргумента строки передается имя каталога.
Необходимо посчитать суммарный размер всех регулярных файлов в этом каталоге и во всех вложенных подкаталогах.
Результат вывести на стандартный поток вывода.*/

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <linux/limits.h>

int64_t calc_rec_size(const char *file) {
    struct stat stat_struct;
    lstat(file, &stat_struct);

    // If it's regular file, return its size.
    if (S_ISREG(stat_struct.st_mode)) {
        return stat_struct.st_size;
    }

    // If it's directory, call recursively.
    if (S_ISDIR(stat_struct.st_mode)) {
        DIR *dir = opendir(file);
        struct dirent *dir_entry = readdir(dir);
        int64_t sum_size = 0;

        while (dir_entry != NULL) {
            if (strcmp(dir_entry->d_name, ".") && strcmp(dir_entry->d_name, "..")) {
                char next_path[PATH_MAX];
                sprintf(next_path, "%s/%s", file, dir_entry->d_name);

                sum_size += calc_rec_size(next_path);
            }

            dir_entry = readdir(dir);
        }

        int32_t check = closedir(dir);
        if (check == -1) {
            perror("Failed during closing");
            return -1;
        }

        return sum_size;
    }

    // Otherwise, don't take into account.
    return 0;
}

int32_t main(int32_t argc, char **argv) {
    int64_t recursive_size = calc_rec_size(argv[1]);
    if (recursive_size == -1) {
        return -1;
    }

    printf("%ld\n", recursive_size);

    return 0;
}
