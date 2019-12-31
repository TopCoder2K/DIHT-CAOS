/*Реализуйте функцию на языке ассемблера x86 (IA-32) или x86-64 с сигнатурой:

extern void summ(int N, const int *A, const int *B, int *R);
      
Функция вычисляет значения Ri=Ai+Bi для всех i от 0 до N.
Память для хранения массива-результата уже выделена.*/

.intel_syntax noprefix
.global summ

.text
summ:
    push ebp
    push ebx
    mov ebp, esp
    
    # ecx = N - 1
    mov ecx, [ebp + 12]
    dec ecx

start_loop:
    # Loop condition.
    cmp ecx, -1
    jz end_loop

    # eax = A, ebx = B, edx = R
    mov eax, [ebp + 16]
    mov ebx, [ebp + 20]
    mov edx, [ebp + 24]

    # eax = *(A + i) + *(B + i)
    mov eax, [eax + 4 * ecx]
    mov ebx, [ebx + 4 * ecx]
    add eax, ebx

    mov [edx + 4 * ecx], eax
    dec ecx

    jmp start_loop

end_loop:
    pop ebx
    pop ebp
    ret
