/*Implement a function on asm-x86 (IA-32) or x86-64 with this signature:

extern void mergesort(int from, int to, const int *in, int *out);
      
This function should do merge sort of integers.
Parameters from and to are indices of left and right boundaries of sorting areas;
in is a source array, out - a pointer to the allocated memory for the result.
Use only stack memory, usage of the heap is forbidden.*/

.intel_syntax noprefix
//.extern printf
// gdb asm x86
.global mergesort

out_param:
    .string "%d %d\n"
out_param2:
    .string "%d %d %d %d\n"

.text
mergesort:
    push ebp
    mov ebp, esp
    push ebx

    # We have to copy all array from in to out.
    # [ebp + 8] = from, [ebp + 12] = to, [ebp + 16] = in, [ebp + 20] = out
    dec word ptr [ebp + 12]
    push [ebp + 8]
    push [ebp + 12]
    push [ebp + 16]
    push [ebp + 20]
    call copy_from_in_to_out
    add esp, 16

    # MergeSort(arr, from ,to);
    # [ebp + 8] = from, [ebp + 12] = to, [ebp + 20] = out
    # But we have to shift indices.
    mov eax, [ebp + 12]
    mov ebx, [ebp + 8]
    sub eax, ebx
    mov ebx, 0

    push eax
    push ebx

    /*push [ebp + 12]
    push [ebp + 8]*/
    push [ebp + 20]
    call MergeSort
    add esp, 12

    pop ebx
    pop ebp
    ret



copy_from_in_to_out:
    push ebp
    mov ebp, esp
    push ebx

    # ebp + 8 = out, ebp + 12 = in, ebp + 16 = to, ebp + 20 = from 
    # ecx = from, esi = in, edi = out
    mov ecx, [ebp + 20]
    mov ebx, 0
    mov esi, [ebp + 12]
    mov edi, [ebp + 8]

start_copying:
    cmp ecx, [ebp + 16]
    jg stop_copying

    mov eax, [esi + 4 * ecx]
    mov [edi + 4 * ebx], eax
    inc ecx
    inc ebx

    jmp start_copying

stop_copying:
    pop ebx
    pop ebp
    ret




MergeSort:
    push ebp
    mov ebp, esp
    push ebx

    /*# For DEBUGGING!
    mov eax, [ebp + 12]
    mov ebx, [ebp + 16]

    push eax    # to
    push ebx    # from
    push offset out_param
    call printf
    add esp, 12*/

    # Check if recursive stops.
    # eax = from, ebx = to
    mov eax, [ebp + 12]
    mov ebx, [ebp + 16]
    
    cmp ebx, eax
    jle StopDividingThisFragment

    # Calculate middle.
    mov ecx, [ebp + 12]
    add ecx, [ebp + 16]
    shr ecx, 1
    push ecx

    # Start new branches of recursion: from - middle, middle + 1 - to.
    # MergeSort(arr, from ,to)
    push ecx
    push [ebp + 12]
    push [ebp + 8]
    call MergeSort
    add esp, 12

    # Save middle.
    pop ecx
    inc ecx
    push ecx

    push [ebp + 16]
    push ecx
    push [ebp + 8]
    call MergeSort
    add esp, 12

    # Merge two parts of the array. At this moment ecx = middle + 1.
    # Merge(arr, from, middle, middle + 1, to)
    pop ecx

    push [ebp + 16]
    push ecx
    dec ecx
    push ecx
    push [ebp + 12]
    push [ebp + 8]
    call Merge
    add esp, 20

StopDividingThisFragment:
    pop ebx
    pop ebp
    ret



Merge:
    push ebp
    mov ebp, esp
    push ebx

    # edi = arr, eax = from, ebx = middle, ecx = middle + 1, edx = to
    mov edi, [ebp + 8]
    mov eax, [ebp + 12]
    mov ebx, [ebp + 16]
    mov ecx, [ebp + 20]
    mov edx, [ebp + 24]

    /*push eax
    push ebx
    push ecx
    push edx
    push offset out_param2
    call printf
    add esp, 20

    mov edi, [ebp + 8]
    mov eax, [ebp + 12]
    mov ebx, [ebp + 16]
    mov ecx, [ebp + 20]
    mov edx, [ebp + 24]*/

    # Pushing on the stack in ascending order while one of the part doesn't reach the end.
    begin_while:
        # Loop condition.
        cmp eax, [ebp + 16]
        jg end_while
        cmp ecx, [ebp + 24]
        jg end_while

        # Choose what value should be pushed on the stack for later copy.
        mov ebx, [edi + 4 * eax]
        mov edx, [edi + 4 * ecx]
        cmp edx, ebx
        jge push_the_first
        jmp push_the_second

        push_the_first:
            push ebx
            inc eax
            jmp begin_while

        push_the_second:
            push edx
            inc ecx
            jmp begin_while

    end_while:
        # Copy the rest.
        cmp eax, [ebp + 16]
        jg push_the_second_rest
        jmp push_the_first_rest

        push_the_second_rest:
            cmp ecx, [ebp + 24]
            jg before_popping

            mov edx, [edi + 4 * ecx]

            push edx
            inc ecx
            jmp push_the_second_rest

        push_the_first_rest:
            cmp eax, [ebp + 16]
            jg before_popping

            mov ebx, [edi + 4 * eax]
            push ebx
            inc eax
            jmp push_the_first_rest

    # Now we have to pop all values from the stack and put them in out array.
    before_popping:
    mov eax, [ebp + 12]
    mov edx, [ebp + 24]
    begin_popping:
        # Loop condition: left_bound = right_bound => end popping.
        cmp eax, edx
        jg StopDividingThisFragment

        pop ecx
        mov [edi + 4 * edx], ecx
        dec edx
        jmp begin_popping

StopMergingThisFragments:
    pop ebx
    pop ebp
    ret
