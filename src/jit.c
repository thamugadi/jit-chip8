#include <chip8.h>

#define CODE(x) code_domain[p++] = x;
#define __SUB_RSP(n) CODE(0x48) CODE(0x81) CODE(0xEC) \
		   CODE((uint8_t)(n&0xff)) CODE((uint8_t)(n>>8)&0xff) \
		   CODE((uint8_t)(n>>16)&0xff) CODE(0x00);

#define X64(x) code[dest_i++] = x;

struct compiled_s jit_recompile(uint16_t* instr, int n)
{
	uint8_t* code = malloc(n*MAX_INSTR_SIZE);
	int source_i;
	int dest_i=0;

	int emitted_instr = 0;
	int fix_skip = 0;

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
			// mov al, byte ptr [&ins.kk]
			X64(0xa0);
			*((uint64_t*)(&code[dest_i])) = &ins.kk;
			dest_i+=8;
			// mov byte ptr [&context.V[ins.n]], al
			X64(0x88); X64(0x05);
			*((uint32_t*)(&code[dest_i])) = &context.V[ins.n];
			dest_i+=4;

			emitted_instr = 15;
		}
		else if (instr[source_i] & 0xF000 == 0x7000) // ADD Vx, byte
		{
			// mov al, byte ptr [&ins.kk]
                        X64(0xa0);
                        *((uint64_t*)(&code[dest_i])) = &ins.kk;
                        dest_i+=8;
			// add byte ptr [&context.V[ins.n]], al
                        X64(0x00); X64(0x05);
                        *((uint32_t*)(&code[dest_i])) = &context.V[ins.n];
                        dest_i+=4;
                        emitted_instr = 15;
		}
		// fix skips

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

        void (*f)() = code_domain;
        f();

//	todo: cache code_domain
//	munmap(code_domain, size);
}

