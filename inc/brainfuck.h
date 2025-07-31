#include <stdlib.h>
#include <stdint.h>

#define false 0
#define true 1

#define MAX_MEMORY_SIZE (SIZE_MAX - 1)

int brainfuck(const char *code, size_t code_length);
int move_pointer(int64_t difference);    
int change_cell(int16_t difference);

size_t find_loop_start(const char *code, size_t code_length, size_t loop_start);
size_t find_loop_end(const char *code, size_t code_length, size_t loop_start);
