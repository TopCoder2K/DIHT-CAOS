/*Реализуйте на языке ассемблера ARM функцию с сигнатурой:

int summ(int x0, int N, int *X)

Функция должна вычислять значение x0+∑xi, где 0<e;i<N.*/
    
    .text
    .global summ

summ:
    // r0 = x0, r1 = N, r2 = int *X, r3 = counter, r4 = tmp_element
    mov r3, #0
    push {r4}
    b loop_start

loop_start:
    // Loop condition.
    cmp r1, r3
    beq loop_end

    // Add new element to the sum.
    ldr r4, [r2, r3, LSL#2]
    add r0, r4

    // Move further.
    add r3, #1

    b loop_start

loop_end:
    pop {r4}
    bx lr
