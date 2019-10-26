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

    uint8_t fontset[80] = {
            0xF0, 0x90, 0x90, 0x90, 0xF0,
            0x20, 0x60, 0x20, 0x20, 0x70,
            0xF0, 0x10, 0xF0, 0x80, 0xF0,
            0xF0, 0x10, 0xF0, 0x10, 0xF0,
            0x90, 0x90, 0xF0, 0x10, 0x10,
            0xF0, 0x80, 0xF0, 0x10, 0xF0,
            0xF0, 0x80, 0xF0, 0x90, 0xF0,
            0xF0, 0x10, 0x20, 0x40, 0x40,
            0xF0, 0x90, 0xF0, 0x90, 0xF0,
            0xF0, 0x90, 0xF0, 0x10, 0xF0,
            0xF0, 0x90, 0xF0, 0x90, 0x90,
            0xE0, 0x90, 0xE0, 0x90, 0xE0,
            0xF0, 0x80, 0x80, 0x80, 0xF0,
            0xE0, 0x90, 0x90, 0x90, 0xE0,
            0xF0, 0x80, 0xF0, 0x80, 0xF0,
            0xF0, 0x80, 0xF0, 0x80, 0x80
    };

    for (int i = 0; i < 80; i++) {
        _ram->heap[i + FONT_OFFSET] = fontset[i];
    }

    return _ram;
}

void free_memory() {
    free(_ram);
}

uint8_t fetch(uint16_t loc) {
    return _ram->heap[loc];
}

void store(uint16_t loc, uint8_t value) {
    _ram->heap[loc] = value;
}

void stack_push(uint16_t pc) {
    _ram->stack[_ram->sp++] = pc;
}

uint16_t stack_pop() {
    return _ram->stack[--_ram->sp];
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
void dump_stack() {
    printf("BEGIN STACK\n");
    for (int i = 0; i < STACK_SIZE; i++) {
        // if (i % 32 == 0) {
        //     printf("%5d: ", i);
        // }
        printf("%04x\n", _ram->stack[i]);
        // if ((i + 1) % 32 == 0) {
        //     printf("\n");
        // }
    }
    printf("\nEND STACK\n");
}
