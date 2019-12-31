/*Программе в аргументе командной строки передается имя файла с бинарными данными в Little-Endian.

Файл хранит внутри себя односвязный список элементов:

struct Item {
  int value;
  uint32_t next_pointer;
};

Поле value хранит значение элемента списка,
поле next_pointer - позицию в файле (в байтах), указывающую на следующий элемент.
Признаком последнего элемента является значение next_pointer, равное 0.

Расположение первого элемента списка (если он существует) - строго в нулевой позиции в файле,
расположение остальных - случайным образом.

Выведите на экран значения элементов в списке в текстовом представлении.

Используйте отображение содержимого файла на память.*/

#include <stdio.h>
#include <stdint.h>
#include <sys/mman.h>   // mmap()
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <endian.h>



const int32_t k_buffer_size = 8;

void read_list(int32_t fd) {
    struct stat stat_struct;
    fstat(fd, &stat_struct);

    if (stat_struct.st_size == 0) return;

    void *file_address = mmap(NULL, stat_struct.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

    int32_t cur_val = 0, next_offset = 0;

    do {
        cur_val = le32toh(*((uint32_t *)(file_address + next_offset)));
        next_offset = le32toh(*((uint32_t *)(file_address + next_offset + 4)));

        printf("%d ", cur_val);
    } while (next_offset != 0);

    munmap(file_address, stat_struct.st_size);
}

int32_t main(int32_t argc, char **argv) {
    int32_t fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("Failed to open input file");
        return 1;
    }

    read_list(fd);

    close(fd);

    return 0;
}
