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
            // TODO: clear display
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
        srand(time(NULL));
        uint8_t random_number = rand() % 0x100;

        V[x] = random_number & kk;
        std::cout << "V" << std::hex << x << " = 0x" << std::hex << static_cast<int>(V[x]) << '\n';
        break;
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