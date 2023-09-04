#include <stdio.h>
#define __USE_GNU
#include <stdint.h>
#include <signal.h>
#include <execinfo.h>
#include <ucontext.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>

#define MAX_EMITTED 4096 

#define CACHE_SIZE 12 

#define WIDTH 64
#define HEIGHT 32
#define PIXELSIZE 1

#define MEMSIZE 0x1000

extern int exec_jit;

extern void initGL();
extern void display();

extern int wait_keyboard;
extern int wait_register;

extern void keyboardUp(uint8_t key, int x, int y);
extern void keyboardDown(uint8_t key, int x, int y);

extern struct context_s context;
extern uint64_t recompiled_block;
extern int c;

void emulate_basic_block();

void interpret(uint16_t instr);

void jit_recompile(uint8_t* code, uint8_t* instr, int n);

void segfault_handler(int sig_n, siginfo_t *info, void *ctx);

struct compiled_s
{
        int size;
        uint16_t new_pc;
        uint8_t* code;
};

struct context_s
{
        uint8_t memory[MEMSIZE];
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

extern uint8_t font[0x50];
