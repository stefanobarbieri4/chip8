#include <iostream>

#include "chip8.h"

int main()
{
    Chip8 chip8;

    // chip8.loadROM("")

    chip8.loadDrawTest();

    for (int i = 1; i <= 6; i++)
    {

        std::cout << "Cycle: " << i << '\n';
        chip8.cycle();
        std::cout << '\n';
    }

    std::cout << "Display:\n\n";
    chip8.debuggingDisplay();

    return 0;
}