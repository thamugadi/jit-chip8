#include <chip8.h>

#define CODE(x) code_domain[p++] = x;
#define __SUB_RSP(n) CODE(0x48) CODE(0x81) CODE(0xEC) \
		   CODE((uint8_t)(n&0xff)) CODE((uint8_t)(n>>8)&0xff) \
		   CODE((uint8_t)(n>>16)&0xff) CODE(0x00);

#define X64(x) code[dest_i++] = x;

#define MOV_REG_BYTE(reg, byte) \
	if (reg < 8) \
	{ \
		X64(0xB0 | reg2mov(reg)); \
		X64(byte); \
	} \
	else\
	{ \
		X64(0x49); X64(0xC7); \
		X64(0xC0 | reg); \
		X64(byte); \
		X64(0x00); X64(0x00); X64(0x00); \
	}

#define ADD_REG_BYTE(reg, byte) \
	if (!reg) \
	{ \
		X64(0x04); X64(byte); \
	} \
	else if (reg < 8) \
	{ \
		X64(0x80); X64(0xC0 | reg2mov(reg)); \
		X64(byte); \
	} \
	else if (byte < 0x80) \
	{ \
		X64(0x48); X64(0x83); \
		X64(0xC0 | reg - 8); \
		X64(byte); \
	} \
	else \
	{ \
		X64(0x48); X64(0x81); \
		X64(0xC0 | reg - 8); \
		X64(byte); \
		X64(0x00); X64(0x00); X64(0x00); \
	}

uint8_t reg2mov(uint8_t x)
{
	switch(x)
	{
		case 0: return 0; break;
                case 1: return 4; break;
                case 2: return 3; break;
                case 3: return 7; break;
                case 4: return 1; break;
                case 5: return 5; break;
                case 6: return 2; break;
                case 7: return 6; break;
	}
}

struct compiled_s jit_recompile(uint16_t* instr, int n)
{
	uint8_t* code = malloc(n*32);
	int source_i;
	int dest_i=0;

	struct instr_s ins;

	for (source_i = 0; source_i < n; source_i++)
	{
		ins.nnn = instr[source_i] & 0xFFF;
		ins.n = instr[source_i] & 0xF;
                ins.x = (instr[source_i] & 0x0F00) >> 8;
		ins.y = (instr[source_i] & 0x00F0) >> 4;
                ins.kk = instr[source_i] & 0x00FF;

		if (instr[source_i] & 0xF000 == 0x6000) // LD Vx, byte
		{
			MOV_REG_BYTE(ins.x, ins.kk);
		}
		else if (instr[source_i] & 0xF000 == 0x7000) // ADD Vx, byte
		{
			ADD_REG_BYTE(ins.x, ins.kk);
		}
	}
}

void jit_execute(uint8_t* compiled, int size, int newpc)
{
        int p = 0;
        uint8_t* code_domain = mmap(0, size, 7, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        CODE(0x55); //push rbp
        CODE(0x48); CODE(0x89); CODE(0xE5); //mov rsp, rbp

	//__SUB_RSP(0x100)

        for (int i = 0; i < size; i++)
        {
                CODE(compiled[i]);
        }

        CODE(0xC9); CODE(0xC3); // leave; ret

	#define COPY_REG1(instr, dest, src) asm(instr : : "r" ((uint8_t)context.V[src]) : dest);
        #define COPY_REG1_(instr, dest, src) asm(instr : : "r" ((uint64_t)context.V[src]) : dest); 

	COPY_REG1("mov al, %0", "al", 0);
        COPY_REG1("mov ah, %0", "ah", 1);
        COPY_REG1("mov bl, %0", "bl", 2);
        COPY_REG1("mov bh, %0", "bh", 3);
        COPY_REG1("mov cl, %0", "cl", 4);
        COPY_REG1("mov ch, %0", "ch", 5);
        COPY_REG1("mov dl, %0", "dl", 6);
        COPY_REG1("mov dh, %0", "dh", 7);
        COPY_REG1_("mov r8, %0", "r8", 8);
        COPY_REG1_("mov r9, %0", "r9", 9);
        COPY_REG1_("mov r10, %0", "r10", 10);
        COPY_REG1_("mov r11, %0", "r11", 11);
        COPY_REG1_("mov r12, %0", "r12", 12);
        COPY_REG1_("mov r13, %0", "r13", 13);
        COPY_REG1_("mov r14, %0", "r14", 14);
        COPY_REG1_("mov r15, %0", "r15", 15);

	asm("mov rsi, %0" : : "r" ((uint64_t)context.I) : "rsi");

	printf("AAAAAAAAAAAAAAAAAA");
        void (*f)() = code_domain;
        f();

	#define COPY_REG2(instr, dest) asm(instr : "=r" (context.V[dest]) : :);

	COPY_REG2("mov %0, al", 0);
        COPY_REG2("mov %0, ah", 1);
        COPY_REG2("mov %0, bl", 2);
        COPY_REG2("mov %0, bh", 3);
        COPY_REG2("mov %0, cl", 4);
        COPY_REG2("mov %0, ch", 5);
        COPY_REG2("mov %0, dl", 6);
        COPY_REG2("mov %0, dh", 7);

	uint64_t tmp_V[8];
	uint64_t tmp_I;

	asm("mov %0, r8" : "=r" (tmp_V[0]) : :);
        asm("mov %0, r9" : "=r" (tmp_V[1]) : :);
        asm("mov %0, r10" : "=r" (tmp_V[2]) : :);
        asm("mov %0, r11" : "=r" (tmp_V[3]) : :);
        asm("mov %0, r12" : "=r" (tmp_V[4]) : :);
        asm("mov %0, r13" : "=r" (tmp_V[5]) : :);
        asm("mov %0, r14" : "=r" (tmp_V[6]) : :);
        asm("mov %0, r15" : "=r" (tmp_V[7]) : :);

	for (int i = 0; i < 8; i++)
	{
		context.V[i+8] = (uint8_t)tmp_V[i];
	}

        asm("mov %0, rsi" : "=r" (tmp_I) : :);
	context.I = (uint8_t)tmp_I;

	context.pc = newpc;

	munmap(code_domain, size);
}

