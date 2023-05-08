#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>

#define MAX_EMITTED 40 

#define CACHE_SIZE 8 

#define WIDTH 64
#define HEIGHT 32
#define PIXELSIZE 1

extern void initGL();
extern void display();

extern void keyboardUp(uint8_t key, int x, int y);
extern void keyboardDown(uint8_t key, int x, int y);

extern struct context_s context;
extern uint64_t recompiled_instr;

void emulate_basic_block();

void interpret(uint16_t instr);

uint8_t* jit_recompile(uint16_t* instr, int n);

struct compiled_s
{
        int size;
        uint16_t new_pc;
        uint8_t* code;
};

struct context_s
{
        uint16_t memory[0x1000];
        uint16_t stack[0x10];

        uint8_t sp; 
        uint16_t pc; 

        uint8_t V[16]; 
        uint16_t I; 

	uint8_t gfx[64][32];
	
	uint8_t dt;
	uint8_t st;

	uint8_t keys[16];
};

struct instr_s
{
        uint16_t opcode;
        char use;

        char name[4];

        uint16_t nnn; //12
        uint8_t n; //4
        uint8_t x; //4
        uint8_t y; //4
        uint8_t kk; //8
};

struct cache_entry
{
	uint8_t* addr;
	uint16_t pc;
	uint64_t freq;
	uint64_t n;
};

struct access_cache_s
{
	int present;
	int n;
	uint8_t* addr;
};

struct access_cache_s access_cache(uint16_t pc);

void update_cache(uint8_t* addr, int n, uint16_t pc);
