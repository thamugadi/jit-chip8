#include <chip8.h>

#define CODE(x) code_domain[p] = x; p++;
#define SUB_RSP(n) CODE(0x48) CODE(0x81) CODE(0xEC) \
		   CODE((uint8_t)(n&0xff)) CODE((uint8_t)(n>>8)&0xff) \
		   CODE((uint8_t)(n>>16)&0xff) CODE(0x00);

struct compiled_s jit_recompile(uint16_t* instr, int n)
{
}

void jit_execute(uint8_t* compiled, int size, int newpc)
{
        int p = 0;
        uint8_t* code_domain = mmap(0, size, 7, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        CODE(0x55); //push rbp
        CODE(0x48); CODE(0x89); CODE(0xE5); //mov rsp, rbp
        
	SUB_RSP(0x100)

        for (int i = 0; i < size; i++)
        {
                CODE(compiled[i]);
        }

        CODE(0xC9); CODE(0xC3); // leave; ret

	asm("xor r9, r9");
	asm("xor r10, r10");

	asm("mov al, %0" : : "r" (context.sp) : "al");
        asm("mov cx, %0" : : "r" (context.I)  : "cx");

	for (int i = 0; i < 8; i++)
	{
		asm("or r9, %0" : : "r" ((uint64_t)context.V[i] << 8*i)  : "r9");
	}
        for (int i = 0; i < 8; i++)
	{
                asm("or r10, %0" : : "r" ((uint64_t)context.V[i+8] << 8*i)  : "r10");
	}

        void (*code)() = code_domain;
        code();
	
	asm("mov %0, al" : "=r" (context.sp) : :);
        asm("mov %0, cx" : "=r" (context.I)  : :);

	uint64_t tmp;
        for (int i = 0; i < 8; i++)
	{
		asm("mov %0, r9" : "=r" (context.V[i]) : :);
		context.V[i] = (context.V[i] >> 8*i) & 0xFF;
	}

        for (int i = 0; i < 8; i++)
        {
                asm("mov %0, r10" : "=r" (context.V[i]) : :);
                context.V[i] = (context.V[i] >> 8*i) & 0xFF;
        }

	context.pc = newpc;

	munmap(code_domain, size);
}

