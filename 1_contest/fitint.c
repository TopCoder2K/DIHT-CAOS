/*Реализуйте функцию с прототипом:

extern int
check_int(uint32_t u32); // retval: 0 - false, 1 - true

которая проверяет, может ли число u32 быть представлено типом float:

float f = u32;
uint64_t y = f;
assert(u32==y);
      

Функция должна вернуть 1, если число может быть представлено типом float, или 0, - если не может.

Допускается исользовать только побитовые операции.*/

#include <stdio.h>
#include <stdint.h>



extern int check_int(uint32_t u32) {
    uint32_t max_mantissa_val = ((uint32_t)1 << 24) - 1;

    // We have two variants when it's possible to put u32 in float:
    // first 9 bits are zero and last 9 bits are zero.
    int32_t counter = 0;
    while(!(1 & u32)) {
        ++counter;
        u32 >>= 1;

        if (counter > 8)
            break;
    }

    int result = 0;
    if (u32 <= max_mantissa_val) {
        result = 1;
    }
    
    return result;
}

int main(int argc, char **argv) {
    uint32_t x = 1823781102;

    printf("%d", check_int(x));

    return 0;
}