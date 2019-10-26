//
// Created by Emerson, Matthew on 10/16/19.
//

#ifndef CHIP8_EMU_MEMORY_H
#define CHIP8_EMU_MEMORY_H

#include <stdlib.h>

#define RAM_SIZE 4096
#define STACK_SIZE 16
#define PROG_OFFSET 0x200
#define FONT_OFFSET 0x50

typedef struct {
    uint8_t heap[RAM_SIZE];
    uint16_t stack[STACK_SIZE];

    uint16_t sp;
} memory;
memory *_ram;

memory *init_memory();
void free_memory();
uint8_t fetch(uint16_t loc);
void store(uint16_t loc, uint8_t value);


void stack_push(uint16_t pc);
uint16_t stack_pop();


void load_prog(const uint8_t *buffer, size_t len);
void dump_heap();
void dump_stack();


#endif //CHIP8_EMU_MEMORY_H
