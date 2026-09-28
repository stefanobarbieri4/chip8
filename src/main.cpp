#include "chip8.h"

int main()
{
    Chip8 chip8;

    // chip8.loadROM("")

    chip8.loadTestProgram();

    chip8.cycle();

    return 0;
}