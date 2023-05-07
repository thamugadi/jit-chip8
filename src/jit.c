#include <chip8.h>

#define CODE(a) code_domain[p++] = a;
#define __SUB_RSP(n) CODE(0x48) CODE(0x81) CODE(0xEC) \
		   CODE((uint8_t)(n&0xff)) CODE((uint8_t)(n>>8)&0xff) \
		   CODE((uint8_t)(n>>16)&0xff) CODE(0x00);

#define X64(a) code[dest_i++] = a;

#define MOV_AL_BYTE_PTR(a) X64(0xa0); EMIT_64LE(a); //9
#define CMP_AL_BYTE_PTR(a) X64(0x3a); X64(0xff); EMIT_32LE(a); //6
#define EMIT_32LE(a) *((uint32_t*)(&code[dest_i])) = a; dest_i+=4;
#define EMIT_64LE(a) *((uint64_t*)(&code[dest_i])) = a; dest_i+=8;

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

		if (instr[source_i] & 0xF000 = 0x3000) // SE Vx, byte
		{
			// mov al, byte ptr [&ins.kk]
			MOV_AL_BYTE_PTR(&ins.kk);
			// cmp al, byte ptr [&context.V[ins.x]]
			CMP_AL_BYTE_PTR(&context.V[ins.x]);
			// je 0 (placeholder)
			X64(0x74);
			X64(0); 

			emitted_instr = 17;
			fix_skip = 1;
		}
		else if (instr[source_i] & 0xF000 = 0x4000) // SNE Vx, byte
                {
                        // mov al, byte ptr [&ins.kk]
                        MOV_AL_BYTE_PTR(&ins.kk);
                        // cmp al, byte ptr [&context.V[ins.x]]
                        CMP_AL_BYTE_PTR(&context.V[ins.x]);
                        // jne 0 (placeholder)
                        X64(0x75);
                        X64(0);

                        emitted_instr = 17;
                        fix_skip = 1;
                }
		else if (instr[source_i] & 0xF000 = 0x5000) // SE Vx, Vy
		{
			// mov al, byte ptr [&context.V[ins.x]]
			MOV_AL_BYTE_PTR(&context.V[ins.x])
			// cmp al, byte ptr [&context.V[ins.y]]
			CMP_AL_BYTE_PTR(&context.V[ins.y]);
                        // je 0 (placeholder)
			X64(0x74);
			X64(0); 

			emitted_instr = 17;
			fix_skip = 1;

		}
		else if (instr[source_i] & 0xF000 == 0x6000) // LD Vx, byte
		{
			// mov al, byte ptr [&ins.kk]
                        MOV_AL_BYTE_PTR(&ins.kk);
			// mov byte ptr [&context.V[ins.n]], al
			X64(0x88); X64(0x05);
			EMIT_32LE(&context.V[ins.x]);

			emitted_instr = 15;
		}
		else if (instr[source_i] & 0xF000 == 0x7000) // ADD Vx, byte
		{
			// mov al, byte ptr [&ins.kk]
                        MOV_AL_BYTE_PTR(&ins.kk);
			// add byte ptr [&context.V[ins.x]], al
                        X64(0x00); X64(0x05);
                        EMIT_32LE(&context.V[ins.x]);

                        emitted_instr = 15;
		}


		if (fix_skip == 1) fix_skip++;
		else if (fix_skip == 2)
		{
			code[dest_i - emitted_instr - 1] = emitted_instr; // fixing jump
			fix_skip = 0;
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

        void (*f)() = code_domain;
        f();

//	todo: cache code_domain
//	munmap(code_domain, size);
}

