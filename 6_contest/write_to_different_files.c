/*Программе в аргументах командной строки передаются три имени файла.
Первый аргумент - входной файл, два остальных - выходные.
Реализуйте программу, которая читает символы из первого файла,
во второй файл записывает только цифры, а в третий - всё остальное.

Разрешается использовать только низкоуровневый ввод-вывод POSIX.
Если входной файл не существует, то нужно завершить работу с кодом 1.
Если не возможно создать один из выходных файлов, то завершить работу с кодом 2.
При возникновении других ошибок ввода-вывода - завершить работу с кодом 3.*/

#include <stdio.h>
#include <stdint.h>
#include <unistd.h> // Provides access to POSIX's API.
#include <fcntl.h>  // To use open.
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h> // To memset.


const int32_t k_buffer_size = 4096;


int32_t read_and_write(int32_t from, int32_t to1, int32_t to2) {
    int32_t count_of_symbols = 0,
            count_of_numbers = 0,
            count_of_letters = 0;
    char buffer[k_buffer_size],
         numbers[k_buffer_size],
         letters[k_buffer_size];

    memset(buffer, 0, k_buffer_size);
    memset(numbers, 0, k_buffer_size);
    memset(letters, 0, k_buffer_size);

    do {
        count_of_symbols = read(from, &buffer, k_buffer_size);

        int32_t i1 = 0, i2 = 0;
        for (int32_t i = 0; i < count_of_symbols; ++i) {
            if (buffer[i] >= '0' && buffer[i] <= '9') {
                numbers[i1++] = buffer[i];
            } else {
                letters[i2++] = buffer[i];
            }
        }

        count_of_numbers = write(to1, &numbers, i1);
        count_of_letters = write(to2, &letters, i2);

        if (count_of_symbols == -1 || count_of_numbers == -1 || count_of_letters == -1) {
            return 3;   // There was an error, so according to the task, return 3.
        }
    } while (count_of_symbols > 0);
    
    return 0;
}

int32_t main(int32_t argc, char **argv) {
    int32_t fd_in = -1, fd_out1 = -1, fd_out2 = -1;

    if (argc < 2) {
        return 1;   // There is no file to read.
    }

    fd_in = open(argv[1], O_RDONLY);
    if (fd_in == -1) {
        perror("Failed to open input file.");
        return 1;
    }

    fd_out1 = open(argv[2], O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    fd_out2 = open(argv[3], O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd_out1 == -1 || fd_out2 == -1) {
        perror("Failed to open output file.");
        return 2;
    }

    int32_t answer = read_and_write(fd_in, fd_out1, fd_out2);
    close(fd_in);
    close(fd_out1);
    close(fd_out2);

    return answer;
}
