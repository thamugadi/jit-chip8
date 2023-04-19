#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>

#define USE_NNN 1
#define USE_N   2
#define USE_X   4
#define USE_Y   8
#define USE_KK  16

struct compiled_s
{
        int size;
        uint16_t new_pc;
        uint8_t code[0x100];
};

struct context
{
        uint8_t memory[0x1000];
        uint8_t stack[0x200];

        uint8_t sp;
        uint16_t pc;
        uint8_t V[16];
        uint16_t I;
};


struct instr
{
        uint16_t opcode;
        char use;

        char name[4];

        uint16_t nnn;
        uint8_t n;
        uint8_t x;
        uint8_t y;
        uint8_t kk;
};
