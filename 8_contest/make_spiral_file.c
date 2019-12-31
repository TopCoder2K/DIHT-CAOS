/*Программе передаются три аргумента: имя файла, положительное целое число N - размер квадратной матрицы,
и положительное число W - ширина клетки матрицы в символах.

Необходимо в указанном файле сформировать текст, который содержит матрицу,
состоящую из чисел от 1 до N^2, таким образом, чтобы числа заполняли её по спирали, по часовой стрелке.

Числа должны быть выровнены по правой границе ячейки матрицы.

Используйте mmap для записи в файл.

Можно использовать функции форматного вывода для преобразования числа в строку.*/

#define _GNU_SOURCE

#include <stdio.h>
#include <stdint.h>
#include <string.h>     // memset()
#include <stdlib.h>     // atoi()
#include <sys/mman.h>   // mmap()
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>



int32_t open_file(const char *name) {
    remove(name);

    int32_t fd = open(name, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("Failed to open input file");
        return 1;
    }

    return fd;
}

void *reallocate_file(int32_t fd, int32_t N, int32_t W) {
    struct stat stat_struct;
    fstat(fd, &stat_struct);

    int32_t necessary_size = N * (N * W + 1);
    fallocate(fd, 0, 0, necessary_size);
    return mmap(NULL, necessary_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
}

void fill_array(int32_t **array, int32_t N) {
    int32_t border = 0;

    array[0][0] = 1;
    while(border <= N - border - 1) {
        for(int32_t i = border + 1; i < N - border; ++i)
            array[border][i] = array[border][i - 1] + 1;

        for(int32_t i = border + 1;i < N - border; ++i)
            array[i][N - border - 1] = array[i - 1][N - border - 1] + 1;

        for(int32_t i = N - border - 2; i >= border; --i)
            array[N - border - 1][i] = array[N - 1 - border][i + 1] + 1;

        for(int32_t i = N - border - 2; i >= border + 1; --i)
                array[i][border] = array[i + 1][border] + 1;

        ++border;
        if (N > 1)
            array[border][border] = array[border][border - 1] + 1;
    }
    if (N > 1)
        array[border][border] = array[border - 1][border] + 1;
}

int32_t main(int32_t argc, char **argv) {
    int32_t N = atoi(argv[2]), W = atoi(argv[3]);

    int32_t **array = malloc(N * sizeof(int32_t *));
    for (int32_t i = 0; i < N; ++i) {
        array[i] = malloc(N * sizeof(int32_t));
        memset(array[i], 0, N * sizeof(int32_t));
    }

    if (N > 0)
        fill_array(array, N);

    int32_t fd = open_file(argv[1]);
    void *file_address = reallocate_file(fd, N, W); 
    for(int32_t i = 0; i < N; ++i) {
        for(int32_t j = 0; j < N; ++j) {
            sprintf(file_address + i * (N * W + 1) + j * W, "%*d", W, array[i][j]);

        }
        sprintf(file_address + i * (N * W + 1) + N * W, "\n");
    }

    munmap(file_address, N * (N * W + 1));
    close(fd);

    for (int32_t i = 0; i < N; ++i) {
        free(array[i]);
    }
    free(array);
    
    return 0;
}
