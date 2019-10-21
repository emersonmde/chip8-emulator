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

// 0x1NNN jump to address NNN
void cpu_op_jmp(uint16_t opcode);

// 0x2NNN call function NNN
void cpu_op_call(uint16_t opcode);

// 0x3XNN skip next instruction if VX == NN
void cpu_op_skip_if(uint16_t opcode);

// 0x4XNN skip next instruction if VX != NN
void cpu_op_skip_not(uint16_t opcode);

// 0x5XY0 skip next instruction if VX == VY
void cpu_op_skip_eq(uint16_t opcode);

// 0x6XNN set VX to NN
void cpu_op_mov(uint16_t opcode);

// 0x7XNN VX += NN
void cpu_op_add_val(uint16_t opcode);

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

// 0xBNNN jump to address NNN + V0
void cpu_op_jump_offset(uint16_t opcode);

// 0xCXNN set VX to rand() & NN
void cpu_op_rand(uint16_t opcode);

// 0xDXYN draw sprite at (VX, VY) that has a width of 8 bits
// each row starts at I and sets VF if pixel is changed
void cpu_op_draw(uint16_t opcode);

// 0xEX9E skip if key in VX is pressed
void cpu_op_key_skip_eq(uint16_t opcode);

// 0xFX07 set VX to the value of the delay timer
// 0xFX0A wait for keypress then store in VX
// 0xFX15 set delay timer to VX
// 0xFX18 set sound timer to VX
// 0xFX1E I += VX
// 0xFX29 Sets I to the location of the sprite for the character in VX; characters 0-F are represented by a 4x5 font.
// 0xFX33 stores the binary-coded decimal representation of VX
// 0xFX55 stores V0 to VX (including VX) in memory starting at address I without modifying I
// 0xFX65 fills V0 to VX (including VX) with values from memory starting at address I without modifying I
void cpu_op_misc(int16_t opcode);

#endif //CHIP8_EMU_CPU_H
