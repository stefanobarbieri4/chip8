#pragma once

#include <cstdint>
#include <string>

class Chip8
{
private:
    uint8_t memory[4096]; // 4KB RAM

    uint8_t V[16]; // variable register V0-VF

    uint16_t stack[16];
    uint8_t sp; // stack pointer

    // index register + program counter
    uint16_t I;
    uint16_t pc;

    bool waitingForKey;
    uint8_t waitingRegister;

public:
    Chip8();

    uint8_t display[64][32]; // 64 * 32 pixels
    uint8_t keypad[16];
    uint8_t delayTimer; // decremented at 60 Hz until 0
    uint8_t soundTimer; // also gives off beeping noise if not 0

    void cycle();
    void loadTestProgram();
    void loadDrawTest();
    void debuggingDisplay();
    void loadROM(const std::string &rom);
};