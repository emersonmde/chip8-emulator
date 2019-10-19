//
// Created by Emerson, Matthew on 10/16/19.
//

#include <cpu.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

processor *init_cpu() {
    _cpu = malloc(sizeof(processor));
    if (_cpu == NULL) {
        return NULL;
    }

    memory *ram = init_memory();
    if (ram == NULL) {
        return NULL;
    }
    _cpu->ram = ram;

    memset(_cpu->registers, 0, sizeof(_cpu->registers) / sizeof(_cpu->registers[0]));
    memset(_cpu->keys, 0, sizeof(_cpu->keys) / sizeof(_cpu->keys[0]));

    _cpu->i = 0;
    _cpu->pc = PROG_OFFSET;
    _cpu->sp =0 ;
    _cpu->delay_timer = 0;
    _cpu->sound_timer = 0;

    return _cpu;
}

void free_cpu() {
    free_memory();
    free(_cpu);
}

void load_rom(char *filename) {
    FILE *file;
    uint8_t buffer[RAM_SIZE - PROG_OFFSET];
    ssize_t filelen;

    file = fopen(filename, "rb");
    fseek(file, 0, SEEK_END);
    filelen = ftell(file);
    if (filelen < 0) {
        fprintf(stderr, "Error determining ROM size\n");
        return;
    }
    rewind(file);

    if (filelen > (RAM_SIZE - PROG_OFFSET)) {
        fprintf(stderr, "Error ROM too large to load");
    }

    fread(&buffer, filelen, 1, file);
    load_prog(buffer, filelen);
    printf("Loaded %s into program memory\n", filename);
}

// See https://en.wikipedia.org/wiki/CHIP-8#Opcode_table
void (*opcode_lookup[16])(uint16_t opcode) = {
        cpu_op_clr_ret,
        cpu_no_op,
        cpu_op_call,
        cpu_no_op,
        cpu_no_op,
        cpu_no_op,
        cpu_op_mov,
        cpu_no_op,
        cpu_lookup_math,
        cpu_op_skip_ne,
        cpu_op_set_i,
        cpu_no_op,
        cpu_no_op,
        cpu_no_op,
        cpu_no_op,
        cpu_no_op,
};

void (*opcode_math_lookup[16])(uint8_t vx, uint8_t vy) = {
        cpu_op_math_set,
        cpu_op_math_or,
        cpu_op_math_and,
        cpu_op_math_xor,
        cpu_op_math_add,
        cpu_op_math_sub,
        cpu_op_math_rshift,
        cpu_op_math_isub,
        cpu_no_math_op,
        cpu_no_math_op,
        cpu_no_math_op,
        cpu_no_math_op,
        cpu_op_math_lshift,
        cpu_no_math_op,
        cpu_no_math_op,
        cpu_no_math_op,
};

void cpu_cycle() {
    // uint16_t (uint16_t)((uint16_t)_ram->heap[loc] << (uint16_t)8) | (uint16_t)_ram->heap[loc + 1];

    // All the casts to make clang happy
    uint16_t opcode = ((uint16_t)fetch(_cpu->pc) << 8) | fetch(_cpu->pc + 1);
    _cpu->pc += 2;

    opcode_lookup[(opcode & 0xF000) >> 12](opcode);
    // Decode Opcode
    // Execute Opcode

    // Update timers
}

void test_opcode(uint16_t opcode) {
    opcode_lookup[(opcode & 0xF000) >> 12](opcode);
}

void cpu_lookup_math(uint16_t opcode) {
    printf("math op: %04x\n", opcode);
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t vy = (opcode & 0x00F0) >> 4;
    opcode_math_lookup[opcode & 0x000F](vx, vy);
}

// Anything not implemented
void cpu_no_op(uint16_t opcode) {
    printf("no op: %04x\n", opcode);
}

// Any 0x8000 math operation not implemented
void cpu_no_math_op(uint8_t vx, uint8_t vy) {
    printf("no math op: vx %02x, vy %02x\n", vx, vy);
}

// 0x00E0 clear screen, 0x00EE return from function
void cpu_op_clr_ret(uint16_t opcode) {
    switch(opcode) {
        case 0x00E0:
            // clr
            cpu_no_op(opcode);
            break;
        case 0x00EE:
            _cpu->pc = stack_pop();
            break;
        default:
            cpu_no_op(opcode);
    }
    printf("clr_ret: %04x\n", opcode & 0x0FFF);
}

// 0x2NNN call function NNN
void cpu_op_call(uint16_t opcode) {
    printf("call: %04x\n", opcode & 0x0FFF);
    stack_push(_cpu->pc);
    _cpu->pc = opcode & 0x0FFF;
}

// 0x6XNN set VX to NN
void cpu_op_mov(uint16_t opcode) {
    uint8_t reg = (opcode & 0x0F00) >> 8;
    uint8_t val = opcode & 0x00FF;
    _cpu->registers[reg] = val;
    printf("mov %04x, reg: %04x, %04x\n", val, reg, opcode);
}

// 0x8XY0 set vx to the value of vy
void cpu_op_math_set(uint8_t vx, uint8_t vy) {
    _cpu->registers[vx] = _cpu->registers[vy];
    printf("set V%d to V%d\n", vx, vy);
}

// 0x8XY1 set VX to (VX | VY)
void cpu_op_math_or(uint8_t vx, uint8_t vy) {
    _cpu->registers[vx] |= _cpu->registers[vy];
}

// 0x8XY2 set VX to (VX & VY)
void cpu_op_math_and(uint8_t vx, uint8_t vy) {
    _cpu->registers[vx] &= _cpu->registers[vy];
}

// 0x8XY3 set VX to (VX ^ VY)
void cpu_op_math_xor(uint8_t vx, uint8_t vy) {
    _cpu->registers[vx] ^= _cpu->registers[vy];
}

// 0x8XY4 add VY to VX setting VF = 1 if carry
void cpu_op_math_add(uint8_t vx, uint8_t vy) {
    _cpu->registers[vx] += _cpu->registers[vy];
    _cpu->registers[0xF] = (_cpu->registers[vx] > 0xFF - _cpu->registers[vy]) ? 1 : 0;
}

// 0x8XY5 subtract VY to VX setting VF = 1 if borrow
void cpu_op_math_sub(uint8_t vx, uint8_t vy) {
    _cpu->registers[vx] -= _cpu->registers[vy];
    _cpu->registers[0xF] = (_cpu->registers[vx] < _cpu->registers[vy]) ? 1 : 0;
}

// 0x8XY6 store LSB in VF and right shfit VX by 1
void cpu_op_math_rshift(uint8_t vx, uint8_t vy) {
    _cpu->registers[0xF] = _cpu->registers[vx] & 0x0F;
    _cpu->registers[vx] >>= 1;
}

// 0x8XY7 sets VX to VY minus VX setting VF = 0 if borrow, VF = 1 otherwise
void cpu_op_math_isub(uint8_t vx, uint8_t vy) {
    _cpu->registers[vx] = _cpu->registers[vy] - _cpu->registers[vx];
    _cpu->registers[0xF] = (_cpu->registers[vy] < _cpu->registers[vx]) ? 0 : 1;
}

// 0x8XYE store MSB in VF and left shfit VX by 1
void cpu_op_math_lshift(uint8_t vx, uint8_t vy) {
    _cpu->registers[0xF] = (_cpu->registers[vx] & 0xF0) >> 4;
    _cpu->registers[vx] <<= 1;
}

// 0x9XY0 skip next instruction if VX != VY
void cpu_op_skip_ne(uint16_t opcode) {
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t vy = (opcode & 0x00F0) >> 4;
    if (_cpu->registers[vx] != _cpu->registers[vy]) {
        _cpu->pc += 2;
    }
}

// 0xANNN set I to NNN
void cpu_op_set_i(uint16_t opcode) {
    printf("set i: %04x\n", opcode & 0x0FFF);
    _cpu->i = opcode & 0x0FFF;
}

