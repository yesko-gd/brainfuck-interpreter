#include <brainfuck.h>
#include <stdio.h>
#include <limits.h>
#include <memory.h>

#ifdef __linux__
#include <termios.h>
#include <unistd.h>
#endif // __linux
#ifdef _WIN32
#include <conio.h>
#endif // _WIN32

uint8_t *memory;
size_t memory_length;
size_t pointer;

int brainfuck(const char *code, size_t code_length) {
#ifdef __linux__
    // modify input settings: don't wait for the user to press enter when getting input (ICANON); don't display user input (ECHO)
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
#endif // __linux__

    bool has_output = false;

    memory_length = 1 << 16; 
    pointer = memory_length / 2;

    memory = calloc(memory_length, sizeof(uint8_t));

    if (memory == NULL) {
        fprintf(stderr, "Failed to allocate memory for program ('memory == NULL')\n");
    }

    int ret = 0;

    for (size_t i = 0; i < code_length; i++) {
        char character = code[i];

        switch (character) {
        case '>':
            ret = move_pointer(1);
            break;
        case '<':
            ret = move_pointer(-1);
            break;
        case '+':
            ret = change_cell(1);
            break;
        case '-':
            ret = change_cell(-1);
            break;
        case ',':
#ifdef _WIN32
            memory[pointer] = _getch();
#else // _WIN32
            memory[pointer] = getchar();
#endif
            break;
        case '.':
            has_output = true;
            putchar(memory[pointer]);
            break;
        case '[':
            if (memory[pointer] == 0) i = find_loop_end(code, code_length, i);
            break;
        case ']':
            if (memory[pointer] != 0) i = find_loop_start(code, code_length, i) - 1;
            break;
        default:
            break;
        }

        if (ret != 0)
            break;

        if (i == SIZE_MAX)
            break;
    }

    free(memory);

    if (has_output)
        printf("\n");

#ifdef __linux__
    // restore input settings
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif // __linux__

    return ret;
}

int move_pointer(int64_t difference) {
    long long int position = pointer + difference;

    if (position < 0) {
        fprintf(stderr, "Moving the pointer to the left of the array ('position < 0') is not supported\n");
        return 1;
    }
    else if (position >= memory_length) {
        if (position >= SIZE_MAX) {
            fprintf(stderr, "Failed to move pointer ('position >= SIZE_MAX')\n");            
            return 1;
        }
        size_t new_memory_length = position + 1;

        if (new_memory_length > MAX_MEMORY_SIZE) {
            fprintf(stderr, "Requested memory exceeds limit (requested: %zu; available: %zu)\n", new_memory_length, MAX_MEMORY_SIZE);
            return 1;
        }

        uint8_t *new_memory = realloc(memory, new_memory_length);

        if (new_memory == NULL) {
            printf("Failed to move pointer ('new_memory == NULL')\n");
            return 1;
        }

        if (new_memory_length > memory_length)
            memset(new_memory + memory_length, 1, new_memory_length - memory_length);

        memory = new_memory;
        memory_length = new_memory_length;
    }

    pointer = position;

    return 0;
}

int change_cell(int16_t difference) {
    memory[pointer] += difference;

    return 0;
}

size_t find_loop_start(const char *code, size_t code_length, size_t loop_end) {
    size_t i = loop_end - 1;
    uint32_t nesting = 1;
    while (true) {
        char character = code[i];

        switch (character) {
        case '[':
            nesting--;
            break;
        case ']':
            nesting++;
            break;
        default:
            break;
        }

        if (nesting == 0)
            return i;

        if (i == 0)
            break;

        i--;
    }

    fprintf(stderr, "No matching loop start found for loop end at character %zu\n", loop_end);
    return SIZE_MAX;
}

size_t find_loop_end(const char *code, size_t code_length, size_t loop_start) {
    size_t i = loop_start + 1;
    uint32_t nesting = 1;
    while (i < code_length) {
        char character = code[i];

        switch (character) {
        case '[':
            nesting++;
            break;
        case ']':
            nesting--;
            break;
        default:
            break;
        }

        if (nesting == 0)
            return i;

        i++;
    }

    fprintf(stderr, "No matching loop end found for loop start at character %zu\n", loop_start);
    return SIZE_MAX;
}
