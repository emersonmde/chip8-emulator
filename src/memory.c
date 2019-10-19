//
// Created by Emerson, Matthew on 10/16/19.
//

#include <memory.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

memory *init_memory() {
    _ram = malloc(sizeof(memory));
    if (_ram == NULL) {
        fprintf(stderr, "Error allocating memory");
        return NULL;
    }
    memset(_ram->heap, 0, sizeof(_ram->heap) / sizeof(_ram->heap[0]));
    memset(_ram->stack, 0, sizeof(_ram->stack) / sizeof(_ram->stack[0]));

    _ram->sp = 0;

    return _ram;
}

void free_memory() {
    free(_ram);
}

uint8_t fetch(uint16_t loc) {
    return _ram->heap[loc];
}

void stack_push(uint16_t pc) {
    _ram->stack[_ram->sp++] = pc;
}

uint16_t stack_pop() {
    return _ram->stack[_ram->sp--];
}


void load_prog(const uint8_t *buffer, const size_t len) {
    for (size_t i = 0; i < len && i < (RAM_SIZE - PROG_OFFSET); i++) {
        _ram->heap[i + PROG_OFFSET] = buffer[i];
    }
}

void dump_heap() {
    printf("BEGIN HEAP\n");
    for (int i = 0; i < RAM_SIZE; i++) {
        if (i % 32 == 0) {
            printf("%5d: ", i);
        }
        printf("%02x ", _ram->heap[i]);
        if ((i + 1) % 32 == 0) {
            printf("\n");
        }
    }
    printf("\nEND HEAP\n");
}