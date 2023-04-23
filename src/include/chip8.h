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

extern struct context_s context;

void emulate();

void interpret(uint16_t instr);

struct compiled_s jit_recompile(uint16_t* instr, int n);
void jit_execute(uint8_t* compiled, int size, int newpc);

struct compiled_s
{
        int size;
        uint16_t new_pc;
        uint8_t* code;
};

struct context_s
{
        uint16_t memory[0x1000];
        uint8_t stack[0x200];

        uint8_t sp; // in ``rax``
        uint16_t pc; 
        uint8_t V[16]; // in ``r9`` and ``r10``
        uint16_t I; // in ``rcx``
};

struct instr_s
{
        uint16_t opcode;
        char use;

        char name[4];

        uint64_t nnn; //12
        uint64_t n; //4
        uint64_t x; //4
        uint64_t y; //4
        uint64_t kk; //8
};
