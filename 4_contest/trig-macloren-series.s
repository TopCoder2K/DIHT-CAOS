/*Реализуйте на языке ассемблера x86 (IA-32) или x86-64 функцию с сигнатурой:

extern double my_sin(double x)

которая вычисляет значение sin(x).
Запрещено использовать встроенные тригонометрические инструкции.
Для вычислений используйте известный вам из курса Математического анализа способ разложения функции в ряд.
Точность результата должна быть маскимально возможной для типа данных double.*/

.intel_syntax noprefix
.extern printf
.global my_sin

.text
my_sin:
    push rbp
    mov rbp, rsp

    # xmm0 = x, xmm1 = counter, xmm2 = x^n, xmm3 = n!, xmm4 = current_summand
    # xmm5 = result, rax = 1 or 0, xmm6 = 0.0 (const, for comparison), xmm7 = 1.0 (const)
    # xmm8 = 1.0 or 0.0 (is flag for (-1)^(n - 1)) 
    mov rax, 0
    cvtsi2sd xmm5, rax
    cvtsi2sd xmm6, rax
    mov rax, 1
    cvtsi2sd xmm1, rax
    movsd xmm2, xmm0
    cvtsi2sd xmm3, rax
    cvtsi2sd xmm7, rax
    cvtsi2sd xmm8, rax


begin_calc:
    # xmm4 = xmm2/xmm3
    movsd xmm4, xmm2
    divsd xmm4, xmm3

    comisd xmm4, xmm6
    jz end_calc

    # xmm5 += xmm4 or xmm5 -=xmm4
    comisd xmm8, xmm6
    jz substraction
    jmp addition

    substraction:
        subsd xmm5, xmm4
        movsd xmm8, xmm7
        jmp continue
    addition:
        addsd xmm5, xmm4
        movsd xmm8, xmm6

continue:
    # xmm2 *= x^2, xmm3 *= n * (n + 1)
    mulsd xmm2, xmm0
    mulsd xmm2, xmm0

    # xmm7 = 1.0 => xmm1 += 1.0 two times, xmm3 *= xmm1 two times.
    addsd xmm1, xmm7
    mulsd xmm3, xmm1
    addsd xmm1, xmm7
    mulsd xmm3, xmm1

    jmp begin_calc


end_calc:
    movsd xmm0, xmm5

    pop rbp
    ret
