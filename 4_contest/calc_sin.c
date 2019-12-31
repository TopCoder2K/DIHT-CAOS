/*Реализуйте на языке ассемблера x86 (IA-32) или x86-64 функцию с сигнатурой:

extern double my_sin(double x)

которая вычисляет значение sin(x).
Запрещено использовать встроенные тригонометрические инструкции.
Для вычислений используйте известный вам из курса Математического анализа способ разложения функции в ряд.
Точность результата должна быть маскимально возможной для типа данных double. */


#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>



extern double my_sin(double x);

int32_t main(int32_t argc, char **argv) {
    double angle = 3.1415926535 * 11/6;
    double answer1 = my_sin(angle);
    double answer2 = 0.0;//sin(angle);

    printf("%f %f %f", angle, answer1, answer2);

    return 0;
}
