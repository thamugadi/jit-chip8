#include <chip8.h>

#define CODE(x) code_domain[p++] = x;
#define __SUB_RSP(n) CODE(0x48) CODE(0x81) CODE(0xEC) \
		   CODE((uint8_t)(n&0xff)) CODE((uint8_t)(n>>8)&0xff) \
		   CODE((uint8_t)(n>>16)&0xff) CODE(0x00);

#define X64(x) code[dest_i++] = x;
#define MOV_RBX_BYTE(x) X64(0x48) X64(0xC7) X64(0xC3) \
			X64((uint8_t)x) X64(0x00) X64(0x00) X64(0x00);

#define MOV_BL_BYTE(x) X64(0xB3) X64(0xFF) X64((uint8_t)x);

#define SHL_RBX(x) X64(0x48) X64(0xC1) X64(0xE3) X64(x);
#define OR_R9_RBX X64(0x49) X64(0x09) X64(0xD9);
#define OR_R10_RBX X64(0x49) X64(0x09) X64(0xDA);

#define MOV_RBX_R9 X64(0x4C) X64(0x89) X64(0xCB);
#define MOV_RBX_R10 X64(0x4C) X64(0x89) X64(0xD3);
#define SHR_RBX(x) X64(0x48) X64(0xC1) X64(0xEB) X64(x);
#define ADD_BL_BYTE(x) X64(0x80) X64(0xC3) X64((uint8_t)x);
#define AND_RBX_FF X64(0x48) X64(0x81) X64(0xE3) X64(0xFF) \
	X64(0x00) X64(0x00) X64(0x00);

struct compiled_s jit_recompile(uint16_t* instr, int n)
{
	uint8_t* code = malloc(n*16);
	int source_i;
	int dest_i=0;

	struct instr_s instr_s;

	for (source_i = 0; source_i < n; source_i++)
	{
		if (instr[source_i] & 0xF000 == 0x6000) // LD Vx, byte
		{
			instr_s.x = (instr[source_i] & 0x0F00) >> 8;
			instr_s.kk = instr[source_i] & 0x00FF;

			if (instr_s.x < 8)
			{
				MOV_BL_BYTE(instr_s.kk);
				SHL_RBX((instr_s.x%8)*8);
				OR_R9_RBX;
			}
			else
			{
                                MOV_BL_BYTE(instr_s.kk);
                                SHL_RBX((instr_s.x%8)*8);
                                OR_R10_RBX;
			}
		}
		else if (instr[source_i] & 0xF000 == 0x7000) // ADD Vx, byte
		{
                        instr_s.x = (instr[source_i] & 0x0F00) >> 8;
                        instr_s.kk = instr[source_i] & 0x00FF;

                        if (instr_s.x < 8)
                        {
                                MOV_RBX_R9;
				SHR_RBX((instr_s.x%8)*8);
				ADD_BL_BYTE(instr_s.kk);
				AND_RBX_FF;
                                SHL_RBX((instr_s.x%8)*8);
				OR_R9_RBX;
                        }
                        else
                        {
                                MOV_RBX_R10;
                                SHR_RBX((instr_s.x%8)*8);
                                ADD_BL_BYTE(instr_s.kk);
                                AND_RBX_FF;
                                SHL_RBX((instr_s.x%8)*8);
                                OR_R10_RBX; 
                        }

		}
	}

}

void jit_execute(uint8_t* compiled, int size, int newpc)
{
        int p = 0;
        uint8_t* code_domain = mmap(0, size, 7, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        CODE(0x55); //push rbp
        CODE(0x48); CODE(0x89); CODE(0xE5); //mov rsp, rbp

	__SUB_RSP(0x100)

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
		asm("or r9, %0" : : "r" (context.V[i] << 8*i)  : "r9");
	}
        for (int i = 0; i < 8; i++)
	{
                asm("or r10, %0" : : "r" (context.V[i+8] << 8*i)  : "r10");
	}

        void (*f)() = code_domain;
        f();
	
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

