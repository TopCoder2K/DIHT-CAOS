/*Реализуйте программу, которая вычисляет сумму двух целых десятичных чисел.
Размер типа равен размеру машинного слова.

Можно использовать функции стандартной библиотеки языка Си scanf и printf.

В качестве точки входа можно считать функцию main.*/

.extern printf
.extern scanf

.data
input_param:
    .ascii "%d %d\000"
output_param:
    .ascii "%d\000"

.text
.globl main

main:
    push {lr}

    sub sp, sp, #4
    mov r1, sp

    sub sp, sp, #4
    mov r2, sp

    ldr r0, =input_param
    bl scanf

    ldr r2, [sp]
    add sp, sp, #4
    ldr r1, [sp]
    add sp, sp, #4

    ldr r0, =output_param
    add r1, r1, r2
    bl printf

    pop {lr}

    bx lr
