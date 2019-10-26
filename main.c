#include <cpu.h>
#include <io.h>

#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>

int main() {
    processor *cpu = init_cpu();
    if (cpu == NULL) {
        printf("Error initializing CPU\n");
        return 1;
    }

    load_rom("assets/pong.rom");
    // dump_heap();

    // set_pixel(1, 1);

    // printf("pixel %x\n", get_pixel(1, 1));
    // for (int i = 0; i < DISP_WIDTH; i++) {
    //     for (int j = 0; j < DISP_HEIGHT; j++) {
    //         set_pixel(i, j);
    //     }
    // }

    bool quit = false;
    SDL_Event e;
    while (!quit) {
        cpu_cycle();
        while( SDL_PollEvent( &e ) != 0 ) {
            if( e.type == SDL_QUIT ) {
                quit = true;
            }
        }
        // SDL_Delay(1);
    }
    // TODO: Switched to ;;
    // for (int i = 0; i < (RAM_SIZE - PROG_OFFSET); i++) {
    //     cpu_cycle();
    // }
    // test_opcode(0x6299);
    // test_opcode(0x8120);

    free_cpu();
    return 0;
}