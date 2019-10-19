//
// Created by Emerson, Matthew on 10/16/19.
//

#ifndef CHIP8_EMU_CPU_H
#define CHIP8_EMU_CPU_H

#include <memory.h>

#include <stdint.h>

typedef struct {
    // Registers V0-VF
    // VF is used as a carry flag
    uint8_t registers[16];
    uint16_t keys[16];

    uint16_t i;
    uint16_t pc;
    uint16_t sp;

    uint8_t delay_timer;
    uint8_t sound_timer;

    memory *ram;
} processor;
processor *_cpu;

processor *init_cpu();
void free_cpu();
void load_rom(char *filename);

void cpu_cycle();
void test_opcode(uint16_t opcode);

void cpu_lookup_math(uint16_t opcode);

void cpu_no_op(uint16_t opcode);
void cpu_no_math_op(uint8_t vx, uint8_t vy);

// 0x00E0 clear screen, 0x00EE return from function
void cpu_op_clr_ret(uint16_t opcode);

// 0x2NNN call function NNN
void cpu_op_call(uint16_t opcode);

// 0x6XNN set VX to NN
void cpu_op_mov(uint16_t opcode);

// 0x8XY0 set vx to the value of vy
void cpu_op_math_set(uint8_t vx, uint8_t vy);

// 0x8XY1 set VX to (VX | VY)
void cpu_op_math_or(uint8_t vx, uint8_t vy);

// 0x8XY2 set VX to (VX & VY)
void cpu_op_math_and(uint8_t vx, uint8_t vy);

// 0x8XY3 set VX to (VX ^ VY)
void cpu_op_math_xor(uint8_t vx, uint8_t vy);

// 0x8XY4 add VY to VX setting VF = 1 if carry
void cpu_op_math_add(uint8_t vx, uint8_t vy);

// 0x8XY5 add VY to VX setting VF = 1 if carry
void cpu_op_math_sub(uint8_t vx, uint8_t vy);

// 0x8XY6 store LSB in VF and right shfit VX by 1
void cpu_op_math_rshift(uint8_t vx, uint8_t vy);

// 0x8XY7 sets VX to VY minus VX setting VF = 0 if borrow, VF = 1 otherwise
void cpu_op_math_isub(uint8_t vx, uint8_t vy);

// 0x8XYE store MSB in VF and left shfit VX by 1
void cpu_op_math_lshift(uint8_t vx, uint8_t vy);

// 0x9XY0 skip next instruction if VX != VY
void cpu_op_skip_ne(uint16_t opcode);

// 0xANNN set I to NNN
void cpu_op_set_i(uint16_t opcode);


#endif //CHIP8_EMU_CPU_H
