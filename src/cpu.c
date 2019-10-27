//
// Created by Emerson, Matthew on 10/16/19.
//

#include <cpu.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <display.h>

processor *init_cpu() {
    _cpu = malloc(sizeof(processor));
    if (_cpu == NULL) {
        return NULL;
    }

    _cpu->ram = init_memory();
    if (_cpu->ram == NULL) {
        return NULL;
    }

    memset(_cpu->registers, 0, sizeof(_cpu->registers) / sizeof(_cpu->registers[0]));
    memset(_cpu->keys, 0, sizeof(_cpu->keys) / sizeof(_cpu->keys[0]));

    _cpu->i = 0;
    _cpu->pc = PROG_OFFSET;
    _cpu->sp =0 ;
    _cpu->delay_timer = 0;
    _cpu->sound_timer = 0;

    _cpu->disp = init_display();

    if (_cpu->disp == NULL) {
        return NULL;
    }

    return _cpu;
}

void free_cpu() {
    free_memory();
    free_display();
    free(_cpu);
}

// See https://en.wikipedia.org/wiki/CHIP-8#Opcode_table
void (*opcode_lookup[16])(uint16_t opcode) = {
        cpu_op_clr_ret,
        cpu_op_jmp,
        cpu_op_call,
        cpu_op_skip_if,
        cpu_op_skip_not,
        cpu_op_skip_eq,
        cpu_op_mov,
        cpu_op_add_val,
        cpu_lookup_math,
        cpu_op_skip_ne,
        cpu_op_set_i,
        cpu_op_jump_offset,
        cpu_op_rand,
        cpu_op_draw,
        cpu_op_key_skip_eq,
        cpu_op_misc,
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
    uint16_t opcode = ((uint16_t)fetch(_cpu->pc) << 8) | fetch(_cpu->pc + 1);
    _cpu->pc += 2;

    opcode_lookup[(opcode & 0xF000) >> 12](opcode);

    if (_cpu->delay_timer > 0) {
        _cpu->delay_timer--;
    }

    if (_cpu->sound_timer > 0) {
        if (_cpu->sound_timer == 1) {
            printf("=========== BEEP ============\n");
        }
        _cpu->sound_timer--;
    }

    // update_window();
}

void test_opcode(uint16_t opcode) {
    opcode_lookup[(opcode & 0xF000) >> 12](opcode);
}

void cpu_lookup_math(uint16_t opcode) {
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t vy = (opcode & 0x00F0) >> 4;
    printf("math_lookup %01x %01x %01x\n", opcode & 0x000F, vx, vy);
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
            // cpu_no_op(opcode);
            break;
        case 0x00EE:
            _cpu->pc = stack_pop();
            printf("return: %2x\n", _cpu->pc);
            break;
        default:
            // cpu_no_op(opcode);
            break;
    }
}

// 0x1NNN jump to address NNN
void cpu_op_jmp(uint16_t opcode) {
    _cpu->pc = opcode & 0x0FFF;
}

// 0x2NNN call function NNN
void cpu_op_call(uint16_t opcode) {
    printf("call: %04x from: %04x\n", opcode & 0x0FFF, _cpu->pc);
    stack_push(_cpu->pc);
    _cpu->pc = opcode & 0x0FFF;
}

// 0x3XNN skip next instruction if VX == NN
void cpu_op_skip_if(uint16_t opcode) {
    if (_cpu->registers[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) {
        _cpu->pc += 2;
    }
}

// 0x4XNN skip next instruction if VX != NN
void cpu_op_skip_not(uint16_t opcode) {
    if (_cpu->registers[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)) {
        _cpu->pc += 2;
    }
}

// 0x5XY0 skip next instruction if VX == VY
void cpu_op_skip_eq(uint16_t opcode) {
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t vy = (opcode & 0x00F0) >> 4;
    if (_cpu->registers[vx] == _cpu->registers[vy]) {
        _cpu->pc += 2;
    }
}

// 0x6XNN set VX to NN
void cpu_op_mov(uint16_t opcode) {
    uint8_t reg = (opcode & 0x0F00) >> 8;
    uint8_t val = opcode & 0x00FF;
    _cpu->registers[reg] = val;
}

// 0x7XNN VX += NN
void cpu_op_add_val(uint16_t opcode) {
    _cpu->registers[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
}

// 0x8XY0 set vx to the value of vy
void cpu_op_math_set(uint8_t vx, uint8_t vy) {
    _cpu->registers[vx] = _cpu->registers[vy];
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
    _cpu->i = opcode & 0x0FFF;
}

// 0xBNNN jump to address NNN + V0
void cpu_op_jump_offset(uint16_t opcode) {
    _cpu->pc = _cpu->registers[0] + (opcode & 0x0FFF);
}

// 0xCXNN set VX to rand() & NN
void cpu_op_rand(uint16_t opcode) {
    uint8_t reg = (opcode & 0x0F00) >> 8;
    uint8_t val = opcode & 0x00FF;
    srand(time(0));
    _cpu->registers[reg] = (rand() % 0xFF) & val;
}

// 0xDXYN draw sprite at (VX, VY) that has a width of 8 bits and height N
// each row starts at I and sets VF if pixel is changed
void cpu_op_draw(uint16_t opcode) {
    uint8_t x = _cpu->registers[(opcode & 0x0F00) >> 8];
    uint8_t y = _cpu->registers[(opcode & 0x00F0) >> 4];
    uint8_t h = opcode & 0x000F;
    uint8_t pixels;

    _cpu->registers[0xF] = 0;
    for (uint16_t row = 0; row < h; row++) {
        pixels = fetch(_cpu->i + row);
        for (uint16_t col = 0; col < 8; col++) {
            // Start at first bit of pixels, if its a 1 write it to display
            if ((pixels & (0x80 >> col)) != 0) {
                if (get_pixel(x + col, y + row) != 0) {
                    _cpu->registers[0xF] = 1;
                }

                set_pixel(x + col, y + row);
            }
        }
    }

    update_window();
}

// 0xEX9E skip if key in VX is pressed
void cpu_op_key_skip_eq(uint16_t opcode) {
    // cpu_no_op(opcode);
}


// 0xFX07 set VX to the value of the delay timer
// 0xFX0A wait for keypress then store in VX
// 0xFX15 set delay timer to VX
// 0xFX18 set sound timer to VX
// 0xFX1E I += VX
// 0xFX29 Sets I to the location of the sprite for the character in VX; characters 0-F are represented by a 4x5 font.
// 0xFX33 stores the binary-coded decimal representation of VX
// 0xFX55 stores V0 to VX (including VX) in memory starting at address I without modifying I
// 0xFX65 fills V0 to VX (including VX) with values from memory starting at address I without modifying I
void cpu_op_misc(uint16_t opcode) {
    uint8_t reg = (opcode & 0x0F00) >> 8;
    switch (opcode & 0x00FF) {
        // 0xFX07 set VX to the value of the delay timer
        case 0x07:
            _cpu->registers[reg] = _cpu->delay_timer;
            break;
        // 0xFX0A wait for keypress then store in VX
        case 0x0A:
            cpu_no_op(opcode);
            break;
        // 0xFX15 set delay timer to VX
        case 0x15:
            _cpu->delay_timer = _cpu->registers[reg];
            break;
        // 0xFX18 set sound timer to VX
        case 0x18:
            _cpu->sound_timer = _cpu->registers[reg];
            break;
        // 0xFX1E I += VX
        case 0x1E:
            _cpu->i += _cpu->registers[reg];
            break;
        // 0xFX29 Sets I to the location of the sprite for the character in VX; characters 0-F are represented by a 4x5 font.
        case 0x29:
            _cpu->i = FONT_OFFSET + (_cpu->registers[reg] * 4);
            break;
        // 0xFX33 stores the binary-coded decimal representation of VX
        case 0x33:
            store(_cpu->i, _cpu->registers[reg] / 100);
            store(_cpu->i + 1, (_cpu->registers[reg] / 10) % 10);
            store(_cpu->i + 1, (_cpu->registers[reg] % 100) % 10);
            break;
        // 0xFX55 stores V0 to VX (including VX) in memory starting at address I without modifying I
        case 0x55:
            for (int i = 0; i < reg; i++) {
                store(_cpu->i + i, _cpu->registers[i]);
            }
            break;
        // 0xFX65 fills V0 to VX (including VX) with values from memory starting at address I without modifying I
        case 0x65:
            for (int i = 0; i < reg; i++) {
                _cpu->registers[i] = fetch(_cpu->i + i);
            }
            break;
    }
}