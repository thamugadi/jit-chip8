#include <chip8.h>

#define __SUB_RSP(n) X64(0x48) X64(0x81) X64(0xEC) \
		   X64((uint8_t)(n&0xff)) X64((uint8_t)(n>>8)&0xff) \
		   X64((uint8_t)(n>>16)&0xff) X64(0x00);

#define X64(a) code[dest_i++] = a; emitted_instr++;

#define EMIT_32LE(a) *((uint32_t*)(&code[dest_i])) = a; dest_i+=4; emitted_instr+=4;
#define EMIT_64LE(a) *((uint64_t*)(&code[dest_i])) = a; dest_i+=8; emitted_instr+=8;

#define MOV_AL_BYTE_PTR(a) X64(0x8a); X64(0x04); X64(0x25); EMIT_32LE(a); // 7
#define CMP_AL_BYTE_PTR(a) X64(0x3a); X64(0x04); X64(0x25); EMIT_32LE(a); // 7

uint8_t* jit_recompile(uint16_t* instr, int n)
{
	uint8_t* code = mmap(0, n*MAX_EMITTED, 7, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	int source_i;
	int dest_i = 0;

	int fix_skip = 0;

	struct instr_s ins;

	int emitted_instr = 0;

        X64(0x55); //push rbp
        X64(0x48); X64(0x89); X64(0xE5); //mov rsp, rbp

	for (source_i = 0; source_i < n; source_i++)
	{
		emitted_instr = 0;

		ins.nnn = instr[source_i] & 0xFFF;
		ins.n = instr[source_i] & 0xF;
                ins.x = (instr[source_i] & 0x0F00) >> 8;
		ins.y = (instr[source_i] & 0x00F0) >> 4;
                ins.kk = instr[source_i] & 0x00FF;

		if (instr[source_i] == 0x00E0) // CLS
		{
			// xor rax, rax
			X64(0x48); X64(0x31); X64(0xc0);
			// xor rbx, rbx
                        X64(0x48); X64(0x31); X64(0xdb);
			// mov qword [rax+&context.gfx], rbx
			X64(0x48); X64(0x89); X64(0x98);
			EMIT_32LE(&context.gfx);
			// add rax, 8
			X64(0x48); X64(0x83); X64(0xc0);
			X64(0x08);
			// cmp rax, 0x800
			X64(0x48); X64(0x3d); X64(0x00); 
			X64(0x08); X64(0x00); X64(0x00);
			// jne -17 (previous 3 instr)
			X64(0x75); X64(17);
		}
		else if (instr[source_i] & 0xF000 == 0x3000) // SE Vx, byte
		{
			// mov al, byte ptr [&ins.kk]
			MOV_AL_BYTE_PTR(&ins.kk);
			// cmp al, byte ptr [&context.V[ins.x]]
			CMP_AL_BYTE_PTR(&context.V[ins.x]);
			// je 0 (placeholder)
			X64(0x74);
			X64(0); 

			fix_skip = 1;
		}
		else if (instr[source_i] & 0xF000 == 0x4000) // SNE Vx, byte
                {
                        // mov al, byte ptr [&ins.kk]
                        MOV_AL_BYTE_PTR(&ins.kk);
                        // cmp al, byte ptr [&context.V[ins.x]]
                        CMP_AL_BYTE_PTR(&context.V[ins.x]);
                        // jne 0 (placeholder)
                        X64(0x75);
                        X64(0);

                        fix_skip = 1;
                }
		else if (instr[source_i] & 0xF000 == 0x5000) // SE Vx, Vy
		{
			// mov al, byte ptr [&context.V[ins.x]]
			MOV_AL_BYTE_PTR(&context.V[ins.x])
			// cmp al, byte ptr [&context.V[ins.y]]
			CMP_AL_BYTE_PTR(&context.V[ins.y]);
                        // je 0 (placeholder)
			X64(0x74);
			X64(0); 

			fix_skip = 1;
		}
		else if (instr[source_i] & 0xF000 == 0x6000) // LD Vx, byte
		{
			// mov al, byte ptr [&ins.kk]
                        MOV_AL_BYTE_PTR(&ins.kk);
			// mov byte ptr [&context.V[ins.x]], al
			X64(0x88); X64(0x04); X64(0x25);
			EMIT_32LE(&context.V[ins.x]);
		}
		else if (instr[source_i] & 0xF000 == 0x7000) // ADD Vx, byte
		{
			// mov al, byte ptr [&ins.kk]
                        MOV_AL_BYTE_PTR(&ins.kk);
			// add byte ptr [&context.V[ins.x]], al
                        X64(0x00); X64(0x04); X64(0x25);
                        EMIT_32LE(&context.V[ins.x]);
		}

		else if (instr[source_i] & 0xF000 == 0x8000 && instr[source_i] & 0xF == 0)
		{ // LD Vx, Vy
			// mov al, byte ptr [&context.V[ins.y]]
			MOV_AL_BYTE_PTR(&context.V[ins.y]);
			// mov byte ptr [&context.V[ins.x]], al
                        X64(0x88); X64(0x04); X64(0x25);
                        EMIT_32LE(&context.V[ins.x]);
		}
		else if (instr[source_i] & 0xF000 == 0x8000 && instr[source_i] & 0xF == 1)
		{ // OR Vx, Vy
			// mov al, byte ptr [&context.V[ins.y]]
                        MOV_AL_BYTE_PTR(&context.V[ins.y]);
			// or byte ptr [&context.V[ins.x]], al
                        X64(0x08); X64(0x04); X64(0x25);
                        EMIT_32LE(&context.V[ins.x]);
		}
		else if (instr[source_i] & 0xF000 == 0x8000 && instr[source_i] & 0xF == 2)
		{ // AND Vx, Vy
                        // mov al, byte ptr [&context.V[ins.y]]
                        MOV_AL_BYTE_PTR(&context.V[ins.y]);
			// and byte ptr [&context.V[ins.x]], al
                        X64(0x20); X64(0x04); X64(0x25);
                        EMIT_32LE(&context.V[ins.x]);
		}
		else if (instr[source_i] & 0xF000 == 0x8000 && instr[source_i] & 0xF == 3)
		{ // XOR Vx, Vy
			// mov al, byte ptr [&context.V[ins.y]]
                        MOV_AL_BYTE_PTR(&context.V[ins.y]);
			// xor byte ptr [&context.V[ins.x]], al
                        X64(0x30); X64(0x04); X64(0x25);
                        EMIT_32LE(&context.V[ins.x]);
                }
                else if (instr[source_i] & 0xF000 == 0x8000 && instr[source_i] & 0xF == 4)
                { // ADD Vx, Vy
			// mov bl, 0
			X64(0xb3); X64(0x00);
			// mov byte ptr [&context.V[15]], bl
			X64(0x88); X64(0x1c); X64(0x25);
			EMIT_32LE(&context.V[15]);
			// mov al, byte ptr [&context.V[ins.y]]
			MOV_AL_BYTE_PTR(&context.V[ins.y]);
			// add byte ptr [&context.V[ins.x]], al
			X64(0x00); X64(0x04); X64(0x25);
			EMIT_32LE(&context.V[ins.x]);
			// jnc 9 (size of next)
			X64(0x73); X64(9);
			// inc bl
			X64(0xfe); X64(0xc3);
			// add byte ptr [&context.V[15]], bl
			X64(0x00); X64(0x1c); X64(0x25);
			EMIT_32LE(&context.V[15]);
                }
                else if (instr[source_i] & 0xF000 == 0x8000 && instr[source_i] & 0xF == 5)
                { // SUB Vx, Vy
		  	// mov bl, 0
			X64(0xb3); X64(0x00);
			// mov byte ptr [&context.V[15]], bl 
                        X64(0x88); X64(0x1c); X64(0x25);
                        EMIT_32LE(&context.V[15]);
			// mov al, byte ptr [&context.V[ins.y]]
                        MOV_AL_BYTE_PTR(&context.V[ins.y]);
			// cmp byte ptr [&context.V[ins.x]], al
			X64(0x38); X64(0x04); X64(0x25);
			EMIT_32LE(&context.V[ins.x]);
			// jng 9 (size of next)
			X64(0x7e); X64(9);
			// inc bl
			X64(0xfe); X64(0xc3);
			// add byte ptr [&context.V[15]], bl
			X64(0x00); X64(0x1c); X64(0x25);
			EMIT_32LE(&context.V[15]);
                }
                else if (instr[source_i] & 0xF000 == 0x8000 && instr[source_i] & 0xF == 6)
                { // SHR Vx {, Vy}
                }
                else if (instr[source_i] & 0xF000 == 0x8000 && instr[source_i] & 0xF == 7)
                { // SUBN Vx, Vy
                }
                else if (instr[source_i] & 0xF000 == 0x8000 && instr[source_i] & 0xF == 0xE)
                { // SHL Vx {, Vy}
                }

		else if (instr[source_i] & 0xF000 == 0x9000) // SNE Vx, Vy
		{
                        // mov al, byte ptr [&context.V[ins.x]]
                        MOV_AL_BYTE_PTR(&context.V[ins.x])
                        // cmp al, byte ptr [&context.V[ins.y]]
                        CMP_AL_BYTE_PTR(&context.V[ins.y]);
                        // jne 0 (placeholder)
                        X64(0x75);
                        X64(0); 

                        fix_skip = 1;
		}

                else if (instr[source_i] & 0xF000 == 0xA000) // LD I, addr
		{
			// mov al, byte ptr [&ins.nnn]
			MOV_AL_BYTE_PTR(&ins.nnn);

                        // mov byte ptr [&context.I], al
                        X64(0x88); X64(0x05);
                        EMIT_32LE(&context.I);
		}
                else if (instr[source_i] & 0xF000 == 0xC000) // RND Vx, byte
		{
			// mov rax, 0x13e
			X64(0x48); X64(0xc7); X64(0xc0);
			X64(0x3e); X64(0x01); X64(0x00);
			X64(0x00);
			// mov rdi, &context.V[ins.x]
			X64(0x48); X64(0xc7); X64(0xc7);
			EMIT_32LE(&context.V[ins.x]);
			// mov rsi, 1
			X64(0x48); X64(0xc7); X64(0xc6);
			X64(0x01); X64(0x00); X64(0x00);
			X64(0x00);
			// syscall
			X64(0x0f); X64(0x05);
			// mov al, byte ptr [&ins.kk]
                        MOV_AL_BYTE_PTR(&ins.kk);
			// and byte ptr [&context.V[ins.x]], al
                        X64(0x20); X64(0x04); X64(0x25);
                        EMIT_32LE(&context.V[ins.x]);
		}
		else if ((instr[source_i] & 0xF000) == 0xD000)
		{ // DRW Vx, Vy, n
		}
		else if ((instr[source_i] & 0xF000) == 0xE000 && instr[source_i] & 0xFF == 0x9E)
		{ // SKP Vx
			// mov al, byte ptr [&context.keys[ins.x]]
			MOV_AL_BYTE_PTR(&context.keys[ins.x]);
			// cmp al, 1
			X64(0x3c); X64(0x01);
			// je 0 (placeholder)
			X64(0x74);
			X64(0);

                        fix_skip = 1;
		}
		else if ((instr[source_i] & 0xF000) == 0xE000 && instr[source_i] & 0xFF == 0xA1)
		{ // SKNP Vx
                        // mov al, byte ptr [&context.keys[ins.x]]
                        MOV_AL_BYTE_PTR(&context.keys[ins.x]);
                        // cmp al, 0              
                        X64(0x3c); X64(0x00);
                        // je 0 (placeholder)
                        X64(0x74);
                        X64(0);
                	
                        fix_skip = 1;
		}
		else if ((instr[source_i] & 0xF000) == 0xF000 && instr[source_i] & 0xFF == 0x07)
		{ // LD Vx, DT
		}
		else if ((instr[source_i] & 0xF000) == 0xF000 && instr[source_i] & 0xFF == 0x0A)
		{ // LD Vx, K
		}
		else if ((instr[source_i] & 0xF000) == 0xF000 && instr[source_i] & 0xFF == 0x15)
		{ // LD DT, Vx
		}
		else if ((instr[source_i] & 0xF000) == 0xF000 && instr[source_i] & 0xFF == 0x18)
		{ // LD ST, Vx
			// no sound yet
		}
                else if ((instr[source_i] & 0xF000) == 0xF000 && instr[source_i] & 0xFF == 0x1E)
                { // ADD I, Vx 
                }                                  
                else if ((instr[source_i] & 0xF000) == 0xF000 && instr[source_i] & 0xFF == 0x29)
                { // LD F, Vx
                }                                  
                else if ((instr[source_i] & 0xF000) == 0xF000 && instr[source_i] & 0xFF == 0x33)
                { // LD B, Vx
                }                                  
                else if ((instr[source_i] & 0xF000) == 0xF000 && instr[source_i] & 0xFF == 0x55)
                { // LD [I], Vx
                }                                  
                else if ((instr[source_i] & 0xF000) == 0xF000 && instr[source_i] & 0xFF == 0x65)
                { // LD Vx, [I]
                }                                  

		if (fix_skip == 1) fix_skip++;
		else if (fix_skip == 2)
		{
			code[dest_i - emitted_instr - 1] = emitted_instr; // fixing jump
			fix_skip = 0;
		}
	}

        X64(0xC9); X64(0xC3); // leave; ret

	return code;
}
