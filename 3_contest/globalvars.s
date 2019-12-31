/*Реализуйте функцию calculate, которая вычисляет значение выражения: R=(A*B)+(C*D),
где A, B, C, и D - это глобальные переменные типа uint32_t,
объявленные во внешнем модуле компиляции,
а R - глобальная переменная типа uint32_t в текущем модуле компиляции. */
    
    .data    

    .word A
    .word B
    .word C
    .word D

    .globl R
R:

    .text

    .globl calculate
calculate:
    // r4 = A, r1 = B, r2 = C, r3 = D, r0 = R
    push {r4}

    ldr r4, =A
    ldr r4, [r4]

    ldr r1, =B
    ldr r1, [r1]

    ldr r2, =C
    ldr r2, [r2]

    ldr r3, =D
    ldr r3, [r3]

    // r1 = A*B, r2 = C*D
    mul r1, r1, r4
    mul r2, r2, r3

    // r0 = A*B + C*D
    add r0, r1, r2

    // r1 = &R
    ldr r1, =R
    str r0, [r1]
    //mov r0, r1

    pop {r4}

    bx lr
