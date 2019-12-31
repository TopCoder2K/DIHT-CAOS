/*Программе в аргументе командной строки передается имя файла с бинарными данными в Little-Endian.

Файл хранит внутри себя односвязный список элементов:

struct Item
{
  int value;
  uint32_t next_pointer;
};

Поле value храние значение элемента списка,
поле next_pointer - позицию в файле (в байтах), указывающую на следующий элемент.
Признаком последнего элемента является значение next_pointer, равное 0.

Расположение первого элемента списка (если он существует) -
строго в нулевой позиции в файле, расположение остальных - случайным образом.

Выведите на экран значения элементов в списке в текстовом представлении.

Для работы с файлом использовать только низкоуровневый ввод-вывод POSIX.*/

#include <stdio.h>
#include <stdint.h>
#include <unistd.h> // Provides access to POSIX's API.
#include <fcntl.h>  // To use open.
#include <sys/types.h>
#include <sys/stat.h>
#include <endian.h> // For transformations between LE and BE.
#include <string.h> // To memset.



const int32_t k_buffer_size = 8;

void read_list(int32_t fd) {
    int32_t cur_val = 0, next_offset = -1;
    char buffer[k_buffer_size];
    off_t cur_offset = 0;

    memset(buffer, 0, k_buffer_size);

    while (next_offset != 0) {
        next_offset = read(fd, &buffer, k_buffer_size);
        if (!next_offset) break;

        void *buf_ptr = buffer;
        cur_val = le32toh(*((uint32_t *)(buf_ptr)));
        next_offset = le32toh(*((uint32_t *)(buf_ptr + 4)));

        printf("%d ", cur_val);

        lseek(fd, next_offset, SEEK_SET);
    }
}

int32_t main(int32_t argc, char **argv) {
    int32_t fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("Failed to open input file.");
        return 1;
    }

    read_list(fd);

    close(fd);

    return 0;
}
