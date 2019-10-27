//
// Created by Matthew Emerson on 10/20/19.
//

#include <io.h>
#include <memory.h>

#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>


void load_rom(char *filename) {
    FILE *file;
    uint8_t buffer[RAM_SIZE - PROG_OFFSET];
    ssize_t filelen;

    file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Error opening file: %s\n", strerror(errno));
        exit(1);
    }
    fseek(file, 0, SEEK_END);
    filelen = ftell(file);
    if (filelen < 0) {
        fprintf(stderr, "Error determining ROM size\n");
        exit(1);
    }
    rewind(file);

    if (filelen > (RAM_SIZE - PROG_OFFSET)) {
        fprintf(stderr, "Error ROM too large to load");
        exit(1);
    }

    fread(&buffer, filelen, 1, file);
    load_prog(buffer, filelen);
    printf("Loaded %s into program memory\n", filename);
}

void wait_keypress() {

}
