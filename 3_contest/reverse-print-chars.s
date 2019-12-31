/*Реализуйте программу, которая считывает символы со стандартного потока ввода,
а затем выводит их в обратном порядке.

Можно использовать функции стандартной библиотеки языка Си: fgetc, fputc, realloc и free.

В качестве точки входа можно считать функцию main.*/

.extern getchar
.extern putchar
.extern realloc

.data
buffer_size: .word 8
size: .word 0
pointer: .word 0
symbol: .word 0

.text
.globl main

main:
    push {lr}

    b loop_start

loop_start:
    bl getchar
    // Сompare with EOF (-1).
    cmp r0, #-1
    beq print_reversed

    // Save symbol.
    ldr r1, =symbol
    str r0, [r1]

    // Check if reallocation is needed.
    // Compare size with buffer_size.
    ldr r1, =size
    ldr r1, [r1]
    ldr r2, =buffer_size
    ldr r2, [r2]
    cmp r1, r2
    beq reallocate

    // If size is zero we also have to reallocate.
    cmp r1, #0
    beq reallocate

    b continue


reallocate:
    // Save pointer.
    ldr r0, =pointer
    ldr r0, [r0]

    // Double the buffer size.
    ldr r2, =buffer_size
    mov r1, r2
    ldr r1, [r1]
    add r1, r1, r1
    str r1, [r2]

    bl realloc
    // Save new pointer.
    ldr r1, =pointer
    str r0, [r1]


continue:
    // Write new symbol.
    ldr r2, =pointer
    ldr r2, [r2]
    ldr r1, =symbol
    ldr r1, [r1]
    ldr r3, =size
    ldr r3, [r3]
    str r1, [r2, r3]

    // Increase size.
    add r3, r3, #1
    ldr r1, =size
    str r3, [r1]

    b loop_start


print_reversed:
    // Decrease size.
    ldr r1, =size
    mov r0, r1
    ldr r0, [r0]
    sub r0, r0, #1
    str r0, [r1]


// Start printing.
print_loop_start:
    // Compare size with 0.
    ldr r1, =size
    ldr r1, [r1]
    cmp r1, #0

    // If it's less then zero, stop.
    blt print_loop_end

    ldr r0, =pointer
    ldr r0, [r0]
    ldr r0, [r0, r1]
    bl putchar

    b print_reversed
    
    
print_loop_end: 
    pop {lr}

    bx lr
