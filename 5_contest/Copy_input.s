/*Реализуйте на языке ассемблера x86 или x86_64 программу,
которая копирует содержимое со стандартного потока ввода на стандартный поток вывода.

Использование стандартной библиотеки Си запрещено.
Точка входа в программу - функция _start. */

.intel_syntax noprefix

.text
.global _start

getchar:
    push rbp
    mov rbp, rsp

    sub rsp, 2                  # Allocate place for the symbol.

    mov rax, 0
    mov rdi, 0                  # Read from stdin
    mov rsi, rsp                # one symbol and put it on the top of the stack.
    mov rdx, 1
    syscall

    test rax, rax               # Syscall read returned number of read elements.
    jz set_minus_one

    pop ax
    jmp standart_suffix

    set_minus_one:
        add rsp, 2              # Deallocate place.
        mov rax, -1

    
standart_suffix:
    pop rbp
    ret


putchar:
    push rbp
    mov rbp, rsp

    push ax

    mov rax, 1
    mov rdi, 1                  # Write in stdout
    mov rsi, rsp                # one symbol.
    mov rdx, 1
    syscall

    add rsp, 2

    pop rbp
    ret


_start:
    mov rbp, rsp

read_and_write:
    call getchar

    cmp rax, -1                 # If EOF was read, stop.
    jz end

    call putchar

    jmp read_and_write


end:
    mov rax, 60
    syscall
