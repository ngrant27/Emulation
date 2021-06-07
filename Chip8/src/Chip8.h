#ifndef CHIP8_H
#define CHIP8_H

#include "pch.h"

typedef std::uint8_t u8;
typedef std::uint16_t u16;

class Chip8
{
private:
  u16 pc;
  u16 opcode;
  u16 I;
  u16 sp;

  u8  V[0xF];
  u8  mem[0xFFF];
  u16 stack[0xF];

  u8 delayTimer;
  u8 soundTimer;

  void initialize();
public:
  u8 display[64][32];
  u8 keys[0xF];

  bool drawFlag;

  void executeCycle();
  bool loadROM(const char* filePath);
};

#endif 
