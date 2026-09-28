#include <algorithm>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "chip8.h"

Chip8::Chip8()
    : memory{},
      V{},
      stack{},
      sp{0},
      keypad{},
      display{},
      delayTimer{0},
      soundTimer{0},
      I{0},
      pc{0x200} // programs start at 0x200
{
}

void Chip8::cycle()
{
    // FETCH

    uint16_t opcode = (memory[pc] << 8) | memory[pc + 1];

    pc += 2;

    std::cout << "Opcode: 0x" << std::hex << opcode << '\n';

    // DECODE
}

void Chip8::loadTestProgram()
{
    memory[0x200] = 0x60;
    memory[0x201] = 0x0A;
}

void Chip8::loadROM(const std::string &rom)
{
    // Open file in binary, cursor at the end to determine size
    std::ifstream file(rom, std::ios::binary | std::ios::ate);

    std::streamsize size = file.tellg();

    constexpr std::size_t MAX_ROM_SIZE = 0xFFF - 0x200 + 1;

    if (size <= 0)
    {
        throw std::runtime_error("Could not open ROM file " + rom);
    }
    else if (static_cast<std::size_t>(size) > MAX_ROM_SIZE)
    {
        throw std::runtime_error("File is to big to be loaded into CHIP-8!");
    }

    // Move cursor back to beginning
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);

    // Load bytes into memory from 0x200 to 0xFFF
    if (file.read(buffer.data(), size))
    {
        std::copy(buffer.begin(), buffer.end(), &memory[0x200]);
    }
}