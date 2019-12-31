/*Реализуйте функцию solve, с прототипом:

int solve(int A, int B, int C, int D);
      

которая находит минимальное значение x ∈[0,254], которое является одним из корней уравнения: Ax3+Bx2+Cx+D=0.
Все аргументы и значение выражения представимы 32-разрядными знаковыми целыми числами.
Не забывайте про сохраняемые регистры; можно использовать для этого память на стеке.
Гарантируется, что на тестовых данных существует целочисленное решение. */
    
    .text
    .global solve_qubic

solve_qubic:
    // r0 = a, r1 = b, r2 = c, r3 = d
    // r4 = counter, r5 = x degree, r6 = polynomial value
    // r7 = result of multiplication
    push {r4, r5, r6, r7}
    mov r4, #0
    b loop_start

loop_start:
    // Loop condition.
    cmp r4, #255
    beq loop_end

    // Calculate the value of the polynomial and compare it with 0.
    // d
    mov r6, r3

    // d + c*x
    mov r5, r4
    mul r7, r5, r2
    add r6, r6, r7

    // d + c*x + b*x^2
    mul r5, r5, r4
    mul r7, r5, r1
    add r6, r6, r7

    // d + c*x + b*x^2 + a*x^3
    mul r5, r5, r4
    mul r7, r5, r0
    add r6, r6, r7

    // Comparison
    cmp r6, #0
    beq loop_end

    add r4, #1
    b loop_start

loop_end:
    mov r0, r4
    pop {r4, r5, r6, r7}
    bx lr
