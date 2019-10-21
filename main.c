#include <cpu.h>

#include <stdio.h>

int main() {
    processor *cpu = init_cpu();
    if (cpu == NULL) {
        printf("Error initializing CPU\n");
        return 1;
    }

    load_rom("assets/pong.rom");
    dump_heap();

    // TODO: Switched to ;;
    for (int i = 0; i < (RAM_SIZE - PROG_OFFSET); i++) {
        cpu_cycle();
    }
    // test_opcode(0x6299);
    // test_opcode(0x8120);

    free_cpu();
    return 0;
}