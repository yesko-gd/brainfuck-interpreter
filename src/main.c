#include <brainfuck.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Missing argument for file path\n");
        return 1;
    }

    char script_path[1024] = { 0 };
    strncpy(script_path, argv[1], 1024);

    FILE *script = fopen(script_path, "r");

    if (script == NULL) {
        fprintf(stderr, "Failed to open file '%s'\n", script_path);
        return 1;
    }

    fseek(script, 0L, SEEK_END);
    size_t size = ftell(script);
    fseek(script, 0L, SEEK_SET);

    char *code = calloc(size + 1, sizeof(char));
    fread(code, sizeof(char), size, script);

    fclose(script);
    
    int err = brainfuck(code, size);

    free(code);

    return err;
}
