/*Реализуйте на языке ассемблера x86 (IA-32) или x86-64 функцию с сигнатурой:

extern float dot_product(int N, const float *A, const float *B);

которая вычисляет скалярное произведение векторов A и B.
Гарантируется, что N больше нуля, но не гарантируется, что N кратно четырём.
Значения исходных векторов хранятся в произвольных адресах памяти,
поэтому выравнивание для них не гарантируется.
Необходимо минимизировать количество выполняемых вычислительных инструкций,
используя векторные вычисления. Доступен набор команд SSE 4.2.
Для архитектуры x86-64 также доступен набор команд AVX.*/

.intel_syntax noprefix
.global dot_product

.text
dot_product:
    push ebp
    mov ebp, esp
    push ebx

    # eax = float ptr1, ebx = float ptr2, ecx = counter (0 at the beginning), edx = N (const)
    # xmm2 = result
    mov ecx, 0
    cvtsi2ss xmm2, ecx
    mov ecx, 4

    mov edx, [ebp + 8]
    mov eax, [ebp + 12]
    mov ebx, [ebp + 16]


begin_product:
    cmp ecx, edx
    jge read_the_rest

    # xmm0 = ptr1[counter], xmm1 = ptr2[counter]
    sub ecx, 4
    movaps xmm0, [eax + 4 * ecx]
    movaps xmm1, [ebx + 4 * ecx]

    # xmm0 = (a1, a2, a3, a4), xmm1 = (b1, b2, b3, b4)
    # after mulps: xmm0 = (a1*b1, a2*b2, a3*b3, a4*b4)
    mulps xmm0, xmm1
    # after haddps: xmm0 = (a1*b1 + a2*b2, a3*b3 + a4*b4, a1*b1 + a2*b2, a3*b3 + a4*b4)
    # and after haddps: xmm0 = (..., a1*b1 + a2*b2 + a3*b3 + a4*b4)
    haddps xmm0, xmm0
    haddps xmm0, xmm0

    addss xmm2, xmm0
    add ecx, 8

    jmp begin_product

read_the_rest:
    sub ecx, 4
begin_reading:
    cmp ecx, edx
    jge end_product

    movss xmm0, [eax + 4 * ecx]
    movss xmm1, [ebx + 4 * ecx]
    mulss xmm0, xmm1
    addss xmm2, xmm0

    inc ecx
    jmp begin_reading


end_product:
    movss xmm0, xmm2

    sub     esp, 4
    movss   [esp], xmm0
    fld     dword ptr [esp]
    add     esp, 4

    pop ebx
    pop ebp
    ret
