/*Implement function

extern void normalize_path(char * path);

which handles filepathes (ending with any symbol except /) and directory pathes (ending with /).
Function must convert text to the canonic form:

remove duplication of consecutive /
handle fragments ./ and ../

It's forbidden to use string library function.
Use pointer arithmetic.*/

#include <stdint.h>
#include <linux/limits.h>
#include <string.h>

void normalize_path(char *path) {
    int32_t stack_ptr = 0;
    char *iterator = path;
    char *stack[PATH_MAX + 2];              // + 2 because we check *(iterator + 2).
    int32_t substrings_len[PATH_MAX + 2];

    memset(stack, 0, PATH_MAX + 2);
    memset(substrings_len, 0, sizeof(int32_t) * (PATH_MAX + 2));
    
    // If path starts with '/', we should add it to answer.
    if (*path == '/') {
        stack[0] = iterator;
        substrings_len[0] = 0;
        ++stack_ptr;
        ++iterator;
    }

    while (*iterator != 0) {
        // We also should take into account that pathes can contain '.'.
        if (*iterator == '.'
            && (*(iterator + 1) == '/' || (*(iterator + 1) == '.' && *(iterator + 2) == '/'))
           ) {

            if (*(iterator + 1) == '/') {
                iterator += 2;
            } else {
                --stack_ptr;
                iterator += 3;
            }
        } else {
            if (*iterator == '/') {
                ++iterator;
            } else {                    // Letter case.
                char *next_file_name = strchr(iterator, '/');
                if (next_file_name == NULL) {
                    next_file_name = strchr(iterator, 0);
                }

                substrings_len[stack_ptr] = next_file_name - iterator;

                stack[stack_ptr] = iterator;
                ++stack_ptr;

                iterator = next_file_name;
            }
        }
    }

    char new_path[PATH_MAX];
    int32_t current_path_len = 0;
    // Copying fragments.
    for (int32_t i = 0; i < stack_ptr; ++i) {
        memcpy(new_path + current_path_len, stack[i], substrings_len[i]);
        current_path_len += substrings_len[i];
 
        new_path[current_path_len] = '/';
        ++current_path_len;
    }

    // We always add '/' to the end of the loop above.
    // So, we erase it if path doesn't start with '/'.
    if (iterator != path && *(iterator - 1) != '/') {
        new_path[current_path_len - 1] = 0;
    } else {
        new_path[current_path_len] = 0;
        ++current_path_len;
    }

    memcpy(path, new_path, current_path_len);
}
