/*Напишите программу,
которая читает со стандартного потока ввода последовательность символов
и выводит только те из них, которые являются десятичными цифрами от 0 до 9.

Можно использовать функции стандартной библиотеки языка Си: fgetc и fputc.

В качестве точки входа можно считать функцию main.*/

.extern getchar
.extern putchar

.text
.globl main

main:
    push {lr}

    b loop_start

loop_start:
    bl getchar
    // Сompare with EOF (-1).
    cmp r0, #-1
    beq loop_end

    // Compare ascii code with numeric range.
    cmp r0, #'0'
    // Check lower bound and in case it's >= 48 compare with 57.
    bge upper_bound

    b loop_start

upper_bound:
    cmp r0, #'9'
    ble print_sym

    // Otherwise, read nest symbol.
    b loop_start

print_sym:
    bl putchar
    b loop_start

loop_end:
    pop {lr}

    bx lr
