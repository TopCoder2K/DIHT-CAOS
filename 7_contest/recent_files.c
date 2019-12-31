/*Программе передаются два аргумента:
имя каталога и три целых числа, - год, месяц и день.

Вывести на стандартный поток вывода список имен файлов указанного каталога,
которые были модифицированы начиная с указанной даты (по местному часовому поясу).*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>



int32_t check_time(time_t m_time, int32_t year, int32_t month, int32_t day) {
    struct tm *file_time = localtime(&m_time);
    int32_t f_t_year = file_time->tm_year + 1900,
            f_t_month = file_time->tm_mon + 1,
            f_t_day = file_time->tm_mday;
    // printf("%d %d %d\n", f_t_year, f_t_month, f_t_day);

    int32_t result = 0;
    if (f_t_year >= year && f_t_month >= month && f_t_day >= day)
        result = 1;

    return result;
}

void get_recent_files(DIR *dir, int32_t dir_fd, int32_t year, int32_t month, int32_t day) {
    struct stat stat_struct;
    struct dirent *dir_entry;

    dir_entry = readdir(dir);
    // Iterating through files and finding info.
    while (dir_entry != NULL) {
        // printf("%s\n", dir_entry->d_name);
        int32_t file_fd = openat(dir_fd, dir_entry->d_name, O_RDONLY);
        int32_t ret = fstat(file_fd, &stat_struct);
        if (ret != 0) {
            perror("stat failed");
            return;
        }

        if (check_time(stat_struct.st_mtime, year, month, day))
            printf("%s\n", dir_entry->d_name);

        close(file_fd);
        dir_entry = readdir(dir);
    }
}

int32_t main(int32_t argc, char **argv) {
    int32_t dir_fd = openat(AT_FDCWD, argv[1], O_RDONLY);
    DIR *dir = fdopendir(dir_fd);
    int32_t year = atoi(argv[2]), month = atoi(argv[3]), day = atoi(argv[4]);
    // printf("%s %d %d %d\n", argv[1], year, month, day);

    get_recent_files(dir, dir_fd, year, month, day);

    closedir(dir);
    close(dir_fd);

    return 0;
}
