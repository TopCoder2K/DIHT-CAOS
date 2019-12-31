/*Реализуйте упрощённый аналог команды ls -l.

Программе в качестве первого аргумента передаётся имя каталога.
Необходимо вывести содержимое каталога в виде:

АТТРИБУТЫ КОЛ_ВО_ССЫЛОК ВЛАДЕЛЕЦ ГРУППА РАЗМЕР ИМЯ

Если в качестве аргумента указано имя файла, то вывести информацию только об этом единственном файле.
Для символических ссылок выведите ещё и имена файлов, на которые они ссылаются.
В случае ошибки - вывести текст ошибки на стандартный поток ошибок и завершить работу с кодом 1.*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <grp.h>
#include <pwd.h>
#include <linux/limits.h>



const int32_t access_size = 8;

void print_file_type(mode_t mode) {
    char type = '-';
    if (S_ISDIR(mode)) {
        type = 'd';
    } else if (S_ISBLK(mode)) {
        type = 'b';
    } else if (S_ISLNK(mode)) {
        type = 'l';
    } else if (S_ISSOCK(mode)) {
        type = 's';
    } else if (S_ISFIFO(mode)) {
        type = 'p';
    } else if (S_ISCHR(mode)) {
        type = 'c';
    }

    printf("%c", type);
}

void print_access(mode_t mode) {
    char accesses[] = "rwxrwxrwx";

    for (int32_t i = 0; i <= access_size; ++i) {
        if (!(mode & (1 << (access_size - i))))
            accesses[i] = '-';
    }

    for (int32_t i = 0; i <= access_size; ++i) {
        printf("%c", accesses[i]);
    }
    printf(" ");
}

void print_file_info(struct stat *stat_struct) {
    print_file_type(stat_struct->st_mode);
    
    print_access(stat_struct->st_mode);
    printf("%lu ", stat_struct->st_nlink);

    // Get group and owner names.
    struct passwd *user_info;
    user_info = getpwuid(stat_struct->st_uid);
    printf("%s ", user_info->pw_name);

    struct group *gr_info;
    gr_info = getgrgid(stat_struct->st_gid);
    printf("%s ", gr_info->gr_name);

    printf("%ld ", stat_struct->st_size);
}

void get_file_name(const char *path, char *file_name) {
    memset(file_name, 0, NAME_MAX + 1);

    int32_t size = strlen(path);
    int32_t index = size - 1;
    while (index >= 0) {
        if (path[index] == '/') break;

        --index;
    }
    ++index;
    memcpy(file_name, path + index, size - index);
}

void check_if_symlink(mode_t mode, const char *name) {
    if (!S_ISLNK(mode)) {
        printf("\n");
        return;
    }

    char file_path[PATH_MAX + 1];
    memset(file_path, 0, PATH_MAX + 1);
    realpath(name, file_path);

    char real_file_name[NAME_MAX + 1];
    get_file_name(file_path, real_file_name);

    printf(" -> %s\n", real_file_name);
}

int32_t get_files_info_from_dir(int32_t dir_fd) {
    DIR *dir = fdopendir(dir_fd);

    struct stat stat_struct;
    struct dirent *dir_entry;

    dir_entry = readdir(dir);
    // Iterating through files and finding info.
    while (dir_entry != NULL) {
        if (strcmp(dir_entry->d_name, ".") && strcmp(dir_entry->d_name, "..")) {
            int32_t ret = fstatat(dir_fd, dir_entry->d_name, &stat_struct, AT_SYMLINK_NOFOLLOW);
            if (ret != 0) {
                perror("Stat failed");
                return -1;
            }

            print_file_info(&stat_struct);
            printf("%s", dir_entry->d_name);

            check_if_symlink(stat_struct.st_mode, dir_entry->d_name);
        }

        dir_entry = readdir(dir);
    }

    return 0;
}

int32_t main(int32_t argc, char **argv) {
    char *path = ".";
    if (argc > 1) {
        path = argv[1];
    }

    // Get stat of directory or file.
    struct stat stat_struct;
    int32_t ret = lstat(path, &stat_struct);
    if (ret != 0) {
        perror("Stat failed");
        return 1;
    }

    int32_t file_fd = openat(AT_FDCWD, path, O_RDONLY);

    if (S_ISDIR(stat_struct.st_mode)) {
        if (get_files_info_from_dir(file_fd) == -1) {
            perror("Failed during reading files");
            return -1;
        }
    } else {
        print_file_info(&stat_struct);

        char file_name[NAME_MAX + 1];
        get_file_name(path, file_name);
        printf("%s", file_name);

        check_if_symlink(stat_struct.st_mode, path);
    }

    file_fd = close(file_fd);
    if (file_fd == -1) {
        perror("Close failed");
        return 1;
    }

    return 0;
}
