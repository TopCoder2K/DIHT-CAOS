/*Реализуйте на языке ассемблера x86 или x86_64 программу, которая выводит "Hello, World!".

Использование стандартной библиотеки Си запрещено.

Точка входа в программу - функция _start.*/

# *asm/unistd_32.h
.intel_syntax noprefix

.text
.global _start

_start:
    push ebx

    mov eax, 4
    mov ebx, 1                  # write in stdout
    mov ecx, hello_world_ptr
    mov edx, 13
    int 0x80

    pop ebx
    mov eax, 1
    int 0x80

.data
hello_world_str:
    .string "Hello, World!"
hello_world_ptr:
    .long hello_world_str
