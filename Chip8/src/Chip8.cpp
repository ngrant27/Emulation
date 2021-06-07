#include "Chip8.h"

#define NNN (opcode & 0x0FFF)
#define KK  (opcode & 0x00FF)
#define X   ((opcode & 0x0F00) >> 8)
#define Y   ((opcode & 0x00F0) >> 4)
#define N   (opcode & 0x000F)

const u8 fontSet[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, //0
    0x20, 0x60, 0x20, 0x20, 0x70, //1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
    0x90, 0x90, 0xF0, 0x10, 0x10, //4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
    0xF0, 0x10, 0x20, 0x40, 0x40, //7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
    0xF0, 0x90, 0xF0, 0x90, 0x90, //A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
    0xF0, 0x80, 0x80, 0x80, 0xF0, //C
    0xE0, 0x90, 0x90, 0x90, 0xE0, //D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
    0xF0, 0x80, 0xF0, 0x80, 0x80  //F
};

void Chip8::initialize()
{
  pc     = 0x200;  
  opcode = 0;
  I      = 0;
  sp     = 0;

  for (int i = 0; i < 0xFFF; i++)
    mem[i] = 0;

  for (int i = 0; i < 64; i++)
    for (int j = 0; j < 32; j++)
      display[i][j] = 0;

  for (int i = 0; i < 0xF; i++)
  {
    V[i]   = 0;
    keys[i]  = 0;
    stack[i] = 0;
  }

  for (int i = 0; i < 80; i++)
    mem[i] = fontSet[i];

  delayTimer = 0;
  soundTimer = 0;

  drawFlag = true;

  srand(time(NULL));
}

bool Chip8::loadROM(const char* filePath)
{ 
  FILE* rom; 
  long lSize;
  char* buffer;
  size_t result;
  
  // reset system
  initialize();

  // open rom
  rom = fopen(filePath, "rb");
  if (rom == NULL)
  {
    std::cout << "Failed to open file\n";
    return false;
  }

  // get file size
  fseek(rom, 0, SEEK_END);
  lSize = ftell(rom);
  rewind(rom);

  // allocate buffer to hold rom
  buffer = (char*)malloc(sizeof(char)*lSize);
  if (buffer == NULL)
  {
    std::cout << "Failed to allocate memory buffer\n";
    fclose(rom);
    return false;
  }

  // copy rom into buffer
  result = fread(buffer, 1, lSize, rom);
  if (result != lSize)
  {
    std::cout << "Failed to read ROM\n";
    fclose(rom);
    free(buffer);
    return false;
  }

  // copy buffer into chip8 memory
  if ((0xFFF - 0x200) > lSize)
  {
    for (int i = 0; i < lSize; ++i)
      mem[0x200 + i] = buffer[i];
  }
  else
  {
    std::cout << "ROM is too large for system memory\n";
    return false;
  }

  // cleanup
  fclose(rom);
  free(buffer);

  return true;
}

void Chip8::executeCycle()
{
  opcode = mem[pc] << 8 | mem[pc + 1];

  switch(opcode & 0xF000)
  {
    case 0x0000:
      switch(opcode & 0x000F)
      {
        case 0x0000:
          for (int i = 0; i < 64; i++)
          {
            for (int j = 0; j < 32; j++)
            {
              display[i][j] = 0;
            }
          }
          pc += 2;
        break;

        case 0x000E:
          sp--;
          pc = stack[sp];
          pc += 2;
        break;
      }
    break;

    case 0x1000:
      pc = NNN;      
    break;

    case 0x2000:
      stack[sp] = pc;
      sp++;
      pc = NNN;
    break;

    case 0x3000:
      if (V[X] == KK)
        pc += 4;
      else 
        pc += 2;
    break;

    case 0x4000:
      if (V[X] != KK)
        pc += 4;
      else
        pc += 2;
    break;

    case 0x5000:
      if (V[X] == V[Y])
        pc += 4;
      else
        pc += 2;
    break;

    case 0x6000:
      V[X] = KK;
      pc += 2;
    break;

    case 0x7000:
      V[X] = V[X] + KK;
      pc += 2;
    break;

    case 0x8000:
      switch(opcode & 0x000F)
      {
        case 0x0000:
          V[X] = V[Y];
          pc += 2;
        break;

        case 0x0001:
          V[X] = V[X] | V[Y];
          pc += 2;
        break;

        case 0x0002:
          V[X] = V[X] & V[Y];
          pc += 2;
        break;

        case 0x0003:
          V[X] = V[X] ^ V[Y];
          pc += 2;
        break;

        case 0x0004:
          if (V[Y] > (0xFF - V[X]))
            V[0xF] = 1;
          else
            V[0xF] = 0;
          V[X] = V[X] + V[Y];
          pc += 2;
        break;

        case 0x0005:
          if (V[X] > V[Y])
            V[0xF] = 1;
          else
            V[0xF] = 0;
          V[X] = V[X] - V[Y];
          pc += 2;
        break;

        case 0x0006:
          V[0xF] = V[X] & 0x1;
          V[X] >>= 1;
          pc += 2;
        break;

        case 0x0007:
          if (V[Y] - V[X])
            V[0xF] = 1;
          else
            V[0xF] = 0;
          V[X] = V[Y] - V[X];
          pc += 2;
        break;

        case 0x000E:
          V[0xF] = V[X] >> 7;
          V[X] <<= 1;
          pc += 2;
        break;
      }
    break;

    case 0x9000:
      if (V[X] != V[Y])
        pc += 4;
      else
        pc += 2;
    break;

    case 0xA000:
      I = NNN;
      pc += 2;
    break;

    case 0xB000:
      pc = NNN + V[0];
    break;

    case 0xC000:
      V[X] = (rand() % 0xFF) & KK;
      pc += 2;
    break;

    case 0xD000:
    {
      u8 x = V[X];
      u8 y = V[Y];
      u8 height = N;
      u8 pixel;

      for (int yLine = 0; yLine < height; yLine++)
      {
        u8 yPos = (y + yLine) % 32;
        pixel = mem[I + yLine];
        for (int xLine = 0; xLine < 8; xLine++)
        {
          u8 xPos = (x + xLine) % 64;
          if ((pixel & (0x80 >> xLine)) != 0)
          {
            if (display[xPos][yPos] != 0)
            {
              V[0xF] = 1;
            }
            display[xPos][yPos] ^= 1;
          }
        }
      }

      drawFlag = true;
      pc += 2;
    }
    break;

    case 0xE000:
      switch(opcode & 0x00FF)
      {
        case 0x009E:
          if (keys[V[X]] == 1)
            pc += 4;
          else
            pc += 2;
        break;

        case 0x00A1:
          if (keys[V[X]] == 0)
            pc += 4;
          else
            pc += 2;
        break;
      }
    break;

    case 0xF000:
      switch(opcode & 0x00FF)
      {
        case 0x0007:
          V[X] = delayTimer;
          pc += 2;
        break;

        case 0x000A:
          {
            bool keyPress = false;

            for (int i = 0; i < 16; i++)
            {
              if (keys[i] != 0)
              {
                V[X] = i;
                keyPress = true;
              }
            }

            if (!keyPress)
              return;

            pc += 2;
          }
        break;

        case 0x0015:
          delayTimer = V[X];
          pc += 2;
        break;

        case 0x0018:
          soundTimer = V[X];
          pc += 2;
        break;

        case 0x001E:
          I = I + V[X];
          pc += 2;
        break;

        case 0x0029:
          I = 0x5 * V[X];
          pc += 2;
        break;

        case 0x0033:
          mem[I]     = V[X] / 100;
          mem[I + 1] = (V[X] / 10) % 10;
          mem[I + 2] = (V[X] % 100) % 10;
          pc += 2;
        break;

        case 0x0055:
          for (int i = 0; i <= X; i++)
            mem[I + i] = V[i];
          pc += 2;
        break;

        case 0x0065:
          for (int i = 0; i <= X; i++)
            V[i] = mem[I + i];
          pc += 2;
        break;
      }
    break;

    default:
    break;
  }

  if (delayTimer > 0)
    delayTimer--;
  
  if (soundTimer > 0)
    soundTimer--;
}

