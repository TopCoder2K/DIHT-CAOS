/*Реализуйте на языке ассемблера x86 или x86_64 программу,
которая читает со стандартного потока ввода текст,
после чего выводит все строки текста в обратном порядке.

Использование стандартной библиотеки Си запрещено.
Точка входа в программу - функция _start. */

.intel_syntax noprefix
.text
.global _start

_start:
    mov rbp, rsp
    push rbx

    # rsi = heap_pointer, rbx = counter (how many bytes is left).
    mov rax, 12                     # We have to know a pointer to the heap.
    mov rdi, 0
    syscall

    mov rsi, rax
    push rsi
    mov rbx, 0



read_loop:
    # If ecx == 0, allocate new 4 KB.
    test rbx, rbx
    jz allocate

    # Else continue to read.
    # Read from stdin and write to the heap as much as possible.
    mov rax, 0
    mov rdi, 0
    mov rdx, rbx
    syscall

    test rax, rax                   # If nothing to read start writing.
    jz write

    # Update the pointer and remaining length.
    # Now rsi is pointer to the end of the data.
    add rsi, rax
    sub rbx, rax

    jmp read_loop

allocate:
    # Allocate a new chunk of 4 KB.
    mov rbx, 4096
    add rsi, rbx

    mov rax, 12
    mov rdi, rsi
    syscall

    sub rsi, rbx
    jmp read_loop



write:
    # In order to understand where strings end, we'll use '\n'
    mov al, '\n'
    mov [rsi], al

write_loop:
    # Remember that heap_pointer is on the top of the stack,
    # so we'll stop writing when we'll reach it.
    cmp rsi, [rsp]
    je _end

    mov rdi, rsi

find_next_string:
    cmp rsi, [rsp]
    je write_last_string

    dec rsi
    mov al, '\n'
    cmp [rsi], al
    je write_next_string

    jmp find_next_string

write_next_string:
    # Now we can write in stdout new rdi - (rsi + 1) + 1 symbols.
    mov rdx, rdi
    sub rdx, rsi

    push rdi
    inc rsi
    mov rax, 1
    mov rdi, 1
    syscall
    dec rsi
    pop rdi

    jmp write_loop

write_last_string:
    # From begin to the last reached '\n': rdi - rsi + 1.
    mov rdx, rdi
    inc rdx
    sub rdx, rsi

    mov rax, 1
    mov rdi, 1
    syscall


_end:
    pop rsi
    pop rbx
    
    mov rax, 60
    mov rdi, 0
    syscall
