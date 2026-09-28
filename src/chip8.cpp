#include <algorithm>
#include <cstring>
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

    srand(time(NULL));

    const uint8_t font[0x50] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    constexpr uint16_t FONT_START = 0x00;

    // Load font

    for (int i = FONT_START; i < 80; i++)
    {
        memory[i] = font[i];
    }
}

void Chip8::cycle()
{
    // FETCH

    uint16_t opcode = (memory[pc] << 8) | memory[pc + 1];

    pc += 2;

    std::cout << "PC: 0x" << std::hex << pc << '\n';

    std::cout << "Opcode: 0x" << std::hex << opcode << '\n';

    // DECODE
    uint16_t first_nibble = opcode >> 12;

    uint16_t x = (opcode >> 8) & 0x0F;
    uint16_t y = (opcode >> 4) & 0x0F;
    uint16_t n = opcode & 0x0F;

    uint16_t nnn = opcode & 0x0FFF;
    uint16_t kk = opcode & 0xFF;

    uint16_t temp;

    switch (first_nibble)
    {

    case 0x0:

        switch (nnn)
        {
        case 0x0E0:
            memset(display, 0, sizeof(display));
            break;

        case 0x0EE:
            pc = stack[sp];
            sp--;

            std::cout << "PC: 0x" << std::hex << pc << '\n';
            std::cout << "SP: 0x" << std::hex << static_cast<int>(sp) << '\n';
            break;
        }

        break;

    case 0x1:
        pc = nnn;

        std::cout << "PC: 0x" << std::hex << pc << '\n';
        break;

    case 0x2:
        sp++;
        stack[sp] = pc;
        pc = nnn;

        std::cout << "PC: 0x" << std::hex << pc << '\n';
        std::cout << "SP: 0x" << std::hex << static_cast<int>(sp) << '\n';
        break;

    case 0x3:
        if (V[x] == kk)
        {
            pc += 2;
        }

        std::cout << "PC: 0x" << std::hex << pc << '\n';
        break;

    case 0x4:
        if (V[x] != kk)
        {
            pc += 2;
        }

        std::cout << "PC: 0x" << std::hex << pc << '\n';
        break;

    case 0x5:

        if (n != 0)
        {
            throw std::runtime_error("Not sure what to do with the following opcode: " + opcode);
        }

        if (V[x] == V[y])
        {
            pc += 2;
        }

        std::cout << "PC: 0x" << std::hex << pc << '\n';
        break;

    case 0x6:
        V[x] = kk;
        std::cout << "V" << std::hex << x << " = 0x" << std::hex << static_cast<int>(V[x]) << '\n';
        break;

    case 0x7:
        V[x] += kk;
        std::cout << "V" << std::hex << x << " = 0x" << std::hex << static_cast<int>(V[x]) << '\n';
        break;

    case 0x8:
        switch (n)
        {
        case 0x0:
            V[x] = V[y];

            std::cout << "V" << std::hex << x << " = 0x" << std::hex << static_cast<int>(V[x]) << '\n';
            break;

        case 0x1:
            V[x] = V[x] | V[y];

            std::cout << "V" << std::hex << x << " = 0x" << std::hex << static_cast<int>(V[x]) << '\n';
            break;

        case 0x2:
            V[x] = V[x] & V[y];

            std::cout << "V" << std::hex << x << " = 0x" << std::hex << static_cast<int>(V[x]) << '\n';
            break;

        case 0x3:
            V[x] = V[x] ^ V[y];

            std::cout << "V" << std::hex << x << " = 0x" << std::hex << static_cast<int>(V[x]) << '\n';
            break;

        case 0x4:
            temp = V[x] + V[y];

            if (temp > 0xFF)
            {
                V[0xF] = 1;
            }
            else
            {
                V[0xF] = 0;
            }

            V[x] = temp & 0xFF;

            std::cout << "V" << std::hex << x << " = 0x" << std::hex << static_cast<int>(V[x]) << '\n';
            break;

        case 0x5:
            if (V[x] > V[y])
            {
                V[0xF] = 1;
            }
            else
            {
                V[0xF] = 0;
            }

            V[x] -= V[y];

            std::cout << "V" << std::hex << x << " = 0x" << std::hex << static_cast<int>(V[x]) << '\n';
            break;

        case 0x6:
            if ((V[x] & 0x1) == 0x1)
            {
                V[0xF] = 1;
            }
            else
            {
                V[0xF] = 0;
            }

            V[x] >>= 1;

            std::cout << "V" << std::hex << x << " = 0x" << std::hex << static_cast<int>(V[x]) << '\n';
            break;

        case 0x7:
            if (V[y] > V[x])
            {
                V[0xF] = 1;
            }
            else
            {
                V[0xF] = 0;
            }

            V[x] = V[y] - V[x];

            std::cout << "V" << std::hex << x << " = 0x" << std::hex << static_cast<int>(V[x]) << '\n';
            break;

        case 0xE:

            if ((V[x] >> 7 & 0x1) == 0x1)
            {
                V[0xF] = 1;
            }
            else
            {
                V[0xF] = 0;
            }
            V[x] <<= 1;

            std::cout << "V" << std::hex << x << " = 0x" << std::hex << static_cast<int>(V[x]) << '\n';
            break;

        default:
            throw std::runtime_error("Not sure what to do with the following opcode: " + opcode);
        }

        break;

    case 0x9:

        if (n != 0x0)
        {
            throw std::runtime_error("Not sure what to do with the following opcode: " + opcode);
        }

        if (V[x] != V[y])
        {
            pc += 2;
            std::cout << "PC: 0x" << std::hex << pc << '\n';
        }
        break;

    case 0xA:
        I = nnn;
        break;

    case 0xB:
        pc = nnn + V[0];
        std::cout << "PC: 0x" << std::hex << pc << '\n';
        break;

    case 0xC:
        uint8_t random_number = rand() % 0x100;

        V[x] = random_number & kk;
        std::cout << "V" << std::hex << x << " = 0x" << std::hex << static_cast<int>(V[x]) << '\n';
        break;

    case 0xD:
        uint8_t x_coord = V[x] % 64;
        uint8_t y_coord = V[y] % 32;

        V[0xF] = 0;

        for (int row = 0; row < n; row++)
        {

            if (y_coord + row >= 32) // reached bottom of the screen?
            {
                break;
            }

            uint8_t pixel = memory[I + row];

            for (int col = 0; col < 0x8; col++)
            {
                if (x_coord + col >= 64) // going too far right
                {
                    break;
                }

                if (pixel & (0x80 >> col))
                {
                    display[x_coord + col][y_coord + row] ^= 1;
                }
            }
        }
        break;

    case 0xF:

        switch (kk)
        {
        case 0x1E:
            I += V[x];
            break;

        case 0x29:
            constexpr uint16_t FONT_START = 0x00;

            I = FONT_START + V[x] * 0x5; // location of font at V[x] - works because of how the font is loaded from 0x0 - 0x50
            break;

        case 0x33:
            uint16_t val = V[x];
            uint16_t hundreds = val / 100;
            uint16_t tens = (val / 10) % 10;
            uint16_t ones = (val % 10);

            memory[I] = hundreds;
            memory[I + 1] = tens;
            memory[I + 2] = ones;
            break;

        case 0x55:
            for (int i = 0; i <= x; i++)
            {
                memory[I + i] = V[i];
            }

            break;

        case 0x65:
            for (int i = 0; i <= x; i++)
            {
                V[i] = memory[I + i];
            }
        }
    }
}

void Chip8::loadTestProgram()
{
    memory[0x200] = 0x22;
    memory[0x201] = 0x06;

    memory[0x202] = 0x60;
    memory[0x203] = 0x0A;

    memory[0x204] = 0x12;
    memory[0x205] = 0x0A;

    memory[0x206] = 0x61;
    memory[0x207] = 0x05;

    memory[0x208] = 0x00;
    memory[0x209] = 0xEE;

    memory[0x20A] = 0x62;
    memory[0x20B] = 0xFF;
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