#include <chip8.h>

#define __SUB_RSP(n) X64(0x48) X64(0x81) X64(0xEC) \
		   X64((uint8_t)(n&0xff)) X64((uint8_t)(n>>8)&0xff) \
		   X64((uint8_t)(n>>16)&0xff) X64(0x00);

#define X64(a) code[dest_i++] = a; emitted_instr++;

#define EMIT_32LE(a) *((uint32_t*)(&code[dest_i])) = a; dest_i+=4; emitted_instr+=4;

#define MOV_AL_BYTE_PTR(a) X64(0x8a); X64(0x04); X64(0x25); EMIT_32LE(a); // 7
#define CMP_AL_BYTE_PTR(a) X64(0x3a); X64(0x04); X64(0x25); EMIT_32LE(a); // 7

/*debug:
48c7 c001 0000 0048 c7c7 0100 0000 e800
0000 005e 48c7 c280 0000 000f 05eb fe

mov rax, 1
mov rdi, 1
call next
next:
pop rsi
mov rdx, 128
syscall
jmp $
*/

#define JIT_DEBUGGER \
	X64(0x48); X64(0xc7); X64(0xc0); X64(0x01); X64(0x00); X64(0x00); X64(0x00); X64(0x48); \
	X64(0xc7); \
	X64(0xc7); \
	X64(0x01); X64(0x00); X64(0x00); X64(0x00); X64(0xe8); X64(0x00); X64(0x00); X64(0x00); \
	X64(0x00); X64(0x5e); X64(0x48); X64(0xc7); X64(0xc2); X64(0xff); X64(0xff); X64(0x00); \
	X64(0x00); X64(0x0f); X64(0x05); X64(0xeb); X64(0xfe);

#define STOP X64(0xeb); X64(0xfe);
struct instr_s ins;

uint8_t* jit_recompile(uint8_t* instr, int n)
{

	uint8_t* code = mmap(0, n*MAX_EMITTED, 7, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	int source_i;
	int dest_i = 0;

	int fix_skip = 0;

	int emitted_instr = 0;

        X64(0x55); //push rbp
        X64(0x48); X64(0x89); X64(0xE5); //mov rsp, rbp
	__SUB_RSP(0x40);

	// push rax
	X64(0x50);
	// push rbx
	X64(0x53);
	// push rcx
	X64(0x51);
	// push rdx
	X64(0x52);

//	JIT_DEBUGGER;

	for (source_i = 0; source_i < n*2; source_i+=2)
	{
		uint16_t current_instr = 
			(uint16_t)(instr[source_i] << 8) | (uint16_t)(instr[source_i+1]);
		printf("Compiled instruction: %x\n", current_instr);

		emitted_instr = 0;
		
		ins.nnn = current_instr & 0xFFF;
		ins.n = current_instr & 0xF;
                ins.x = (current_instr & 0x0F00) >> 8;
		ins.y = (current_instr & 0x00F0) >> 4;
                ins.kk = current_instr & 0x00FF;

		if (current_instr== 0x00E0) // CLS
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
			X64(0x75); X64(0xed);
		}
		else if ((current_instr & 0xF000) == 0x3000) // SE Vx, byte
		{
/*			// mov al, ins.kk
			X64(0xb0); X64(ins.kk);
			// cmp al, byte ptr [&context.V[ins.x]]
			CMP_AL_BYTE_PTR(&context.V[ins.x]);
			// je 0 (placeholder)
			X64(0x74);
			X64(0); 

			fix_skip = 1;
			*/
		}
		else if ((current_instr & 0xF000) == 0x4000) // SNE Vx, byte
                {
/*                        // mov al, ins.kk
                        X64(0xb0); X64(ins.kk);
                        // cmp al, byte ptr [&context.V[ins.x]]
                        CMP_AL_BYTE_PTR(&context.V[ins.x]);
                        // jne 0 (placeholder)
                        X64(0x75);
                        X64(0);

                        fix_skip = 1;
			*/
                }
		else if ((current_instr & 0xF000) == 0x5000) // SE Vx, Vy
		{
/*			// mov al, byte ptr [&context.V[ins.x]]
			MOV_AL_BYTE_PTR(&context.V[ins.x])
			// cmp al, byte ptr [&context.V[ins.y]]
			CMP_AL_BYTE_PTR(&context.V[ins.y]);
                        // je 0 (placeholder)
			X64(0x74);
			X64(0); 

			fix_skip = 1;
			*/
		}
		else if ((current_instr & 0xF000) == 0x6000) // LD Vx, byte
		{
			// mov al, ins.kk
                        X64(0xb0); X64(ins.kk);
			// mov byte ptr [&context.V[ins.x]], al
			X64(0x88); X64(0x04); X64(0x25);
			EMIT_32LE(&context.V[ins.x]);
		}
		else if ((current_instr & 0xF000) == 0x7000) // ADD Vx, byte
		{
			// mov al, ins.kk
                        X64(0xb0); X64(ins.kk);
			// add byte ptr [&context.V[ins.x]], al
                        X64(0x00); X64(0x04); X64(0x25);
                        EMIT_32LE(&context.V[ins.x]);
		}

		else if (((current_instr & 0xF000) == 0x8000) && (current_instr & 0xF) == 0)
		{ // LD Vx, Vy
			// mov al, byte ptr [&context.V[ins.y]]
			MOV_AL_BYTE_PTR(&context.V[ins.y]);
			// mov byte ptr [&context.V[ins.x]], al
                        X64(0x88); X64(0x04); X64(0x25);
                        EMIT_32LE(&context.V[ins.x]);
		}
		else if (((current_instr & 0xF000) == 0x8000) && (current_instr & 0xF) == 1)
		{ // OR Vx, Vy
			// mov al, byte ptr [&context.V[ins.y]]
                        MOV_AL_BYTE_PTR(&context.V[ins.y]);
			// or byte ptr [&context.V[ins.x]], al
                        X64(0x08); X64(0x04); X64(0x25);
                        EMIT_32LE(&context.V[ins.x]);
		}
		else if ((current_instr & 0xF000) == 0x8000 && (current_instr & 0xF) == 2)
		{ // AND Vx, Vy
                        // mov al, byte ptr [&context.V[ins.y]]
                        MOV_AL_BYTE_PTR(&context.V[ins.y]);
			// and byte ptr [&context.V[ins.x]], al
                        X64(0x20); X64(0x04); X64(0x25);
                        EMIT_32LE(&context.V[ins.x]);
		}
		else if ((current_instr & 0xF000) == 0x8000 && (current_instr & 0xF) == 3)
		{ // XOR Vx, Vy
			// mov al, byte ptr [&context.V[ins.y]]
                        MOV_AL_BYTE_PTR(&context.V[ins.y]);
			// xor byte ptr [&context.V[ins.x]], al
                        X64(0x30); X64(0x04); X64(0x25);
                        EMIT_32LE(&context.V[ins.x]);
                }
                else if ((current_instr & 0xF000) == 0x8000 && (current_instr & 0xF) == 4)
                { // ADD Vx, Vy
			// mov al, byte ptr [&context.V[ins.y]]
			MOV_AL_BYTE_PTR(&context.V[ins.y]);
			// add byte ptr [&context.V[ins.x]], al
			X64(0x00); X64(0x04); X64(0x25);
			EMIT_32LE(&context.V[ins.x]);
			// setc byte ptr [&context.V[15]]
			X64(0x0f); X64(0x92); X64(0x04);
			X64(0x25);
			EMIT_32LE(&context.V[15]);
			X64(0x90);
                }
                else if ((current_instr & 0xF000) == 0x8000 && (current_instr & 0xF) == 5)
                { // SUB Vx, Vy
			// mov al, byte ptr [&context.V[ins.y]]
                        MOV_AL_BYTE_PTR(&context.V[ins.y]);
			// sub byte ptr [&context.V[ins.x]], al
			X64(0x28); X64(0x04); X64(0x25);
			EMIT_32LE(&context.V[ins.x]);
			// setnc byte ptr [&context.V[15]]
			X64(0x0f); X64(0x93); X64(0x04);
			X64(0x25);
			EMIT_32LE(&context.V[15]);
                }
                else if ((current_instr & 0xF000) == 0x8000 && (current_instr & 0xF) == 6)
                { // SHR Vx {, Vy}
			// mov al, byte ptr [&context.V[ins.x]]
			MOV_AL_BYTE_PTR(&context.V[ins.x]);
			// shr al, 1
			X64(0xd0); X64(0xe8);
                        // setc byte ptr [&context.V[15]]
                        X64(0x0f); X64(0x92); X64(0x04);
			X64(0x25);
                        EMIT_32LE(&context.V[15]);
			// mov byte ptr [&context.V[ins.x]], al
                        X64(0x88); X64(0x04); X64(0x25);
                        EMIT_32LE(&context.V[ins.x]);
		}
                else if ((current_instr & 0xF000) == 0x8000 && (current_instr & 0xF) == 7)
                { // SUBN Vx, Vy
			// mov al, byte ptr [&context.V[ins.y]]
			MOV_AL_BYTE_PTR(&context.V[ins.y]);
			// mov cl, byte ptr [&context.V[ins.x]]
			X64(0x8a); X64(0x0c); X64(0x25);
			EMIT_32LE(&context.V[ins.x]);
			// sub al, cl
			X64(0x28); X64(0xc8);
			// setnc byte ptr [&context.V[15]]
                        X64(0x0f); X64(0x93); X64(0x04);
			X64(0x25);
                        EMIT_32LE(&context.V[15]);
			// mov byte ptr [&context.V[ins.x]], al
                        X64(0x88); X64(0x04); X64(0x25);
                        EMIT_32LE(&context.V[ins.x]);
                }
                else if ((current_instr& 0xF000) == 0x8000 && (current_instr & 0xF) == 0xE)
                { // SHL Vx {, Vy}
                        // mov al, byte ptr [&context.V[ins.x]]
                        MOV_AL_BYTE_PTR(&context.V[ins.x]);
                        // shl al, 1
                        X64(0xd0); X64(0xe0);
                        // setc byte ptr [&context.V[15]]
                        X64(0x0f); X64(0x92); X64(0x04);
			X64(0x25);
                        EMIT_32LE(&context.V[15]);
                        // mov byte ptr [&context.V[ins.x]], al 
                        X64(0x88); X64(0x04); X64(0x25);
                        EMIT_32LE(&context.V[ins.x]);
                }

		else if ((current_instr & 0xF000) == 0x9000) // SNE Vx, Vy
		{
/*                        // mov al, byte ptr [&context.V[ins.x]]
                        MOV_AL_BYTE_PTR(&context.V[ins.x])
                        // cmp al, byte ptr [&context.V[ins.y]]
                        CMP_AL_BYTE_PTR(&context.V[ins.y]);
                        // jne 0 (placeholder)
                        X64(0x75);
                        X64(0); 

                        fix_skip = 1;
			*/
		}

                else if ((current_instr & 0xF000) == 0xA000) // LD I, addr
		{
			uint8_t* ptr = &context.I;

			// mov ax, ins.nnn
			X64(0x66); X64(0xB8);
			X64(ins.nnn & 0xff);
			X64(ins.nnn >> 8);
			// mov byte ptr [&context.I], ah
			X64(0x88); X64(0x24); X64(0x25);
			EMIT_32LE(ptr+1);
			// mov byte ptr [&context.I+1], al
			X64(0x88); X64(0x04); X64(0x25);
			EMIT_32LE(ptr);

		}
                else if ((current_instr & 0xF000) == 0xC000) // RND Vx, byte
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
			// mov al, ins.kk
                        X64(0xb0); X64(ins.kk);
			// and byte ptr [&context.V[ins.x]], al
                        X64(0x20); X64(0x04); X64(0x25);
                        EMIT_32LE(&context.V[ins.x]);
		}
		else if ((current_instr & 0xF000) == 0xD000)
		{ // DRW Vx, Vy, n
			// Currently interpreted, waiting to be implemented here.
		}
		else if ((current_instr & 0xF000) == 0xE000 && (current_instr & 0xFF) == 0x9E)
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
		else if ((current_instr & 0xF000) == 0xE000 && (current_instr & 0xFF) == 0xA1)
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
		else if ((current_instr & 0xF000) == 0xF000 && (current_instr & 0xFF) == 0x07)
		{ // LD Vx, DT
			// mov al, byte ptr [&context.dt]
			MOV_AL_BYTE_PTR(&context.dt);
                        // mov byte ptr [&context.V[ins.x]], al 
                        X64(0x88); X64(0x04); X64(0x25);
                        EMIT_32LE(&context.V[ins.x]);
		}
		else if ((current_instr & 0xF000) == 0xF000 && (current_instr & 0xFF) == 0x0A)
		{ // LD Vx, K
		  /*	// begin:
			// xor rax, rax
			X64(0x48); X64(0x31); X64(0xc0);
			// mov bl, 1
			X64(0xb3); X64(0x01);
			// loop:
			// cmp al, 16
			X64(0x3c); X64(0x10);
			// je begin
			X64(0x74); X64(0xf7);
			// cmp byte ptr [rax + &context.keys], bl
			X64(0x38); X64(0x98);
			EMIT_32LE(&context.keys);
			// je copy
			X64(0x74); X64(0x04);
			// inc al
			X64(0xfe); X64(0xc0);
			// jmp loop
			X64(0xeb); X64(0xf0);
			// copy:
			// mov byte ptr [&context.V[ins.x]], al
			X64(0x88); X64(0x04); X64(0x25);
			EMIT_32LE(&context.V[ins.x]);
			*/
		}
		else if ((current_instr & 0xF000) == 0xF000 && (current_instr & 0xFF) == 0x15)
		{ // LD DT, Vx
                        // mov al, byte ptr [&context.V[ins.x]]
                        MOV_AL_BYTE_PTR(&context.V[ins.x]);                                      
                        // mov byte ptr [&context.dt], al 
                        X64(0x88); X64(0x04); X64(0x25);
                        EMIT_32LE(&context.dt);
		}
		else if ((current_instr & 0xF000) == 0xF000 && current_instr & 0xFF == 0x18)
		{ // LD ST, Vx
                        // mov al, byte ptr [&context.V[ins.x]]
                        MOV_AL_BYTE_PTR(&context.V[ins.x]);                                      
                        // mov byte ptr [&context.st], al 
                        X64(0x88); X64(0x04); X64(0x25);
                        EMIT_32LE(&context.st);
		}
                else if ((current_instr & 0xF000) == 0xF000 && (current_instr & 0xFF) == 0x1E)
                { // ADD I, Vx 
		  	uint8_t* ptr = &context.I;
		  	// xor ax, ax
			X64(0x66); X64(0x31); X64(0xc0);
			// mov al, byte ptr [&context.V[ins.x]]
			X64(0x8a); X64(0x04); X64(0x25);
			EMIT_32LE(&context.V[ins.x]);
			// xor ah, ah
			X64(0x30); X64(0xe4);
			// xor bx, bx
			X64(0x66); X64(0x31); X64(0xdb);
			// mov bh, byte ptr [&context.I+1]
			X64(0x8a); X64(0x3c); X64(0x25);
			EMIT_32LE(ptr+1);
			// mov bl, byte ptr [&context.I]
			X64(0x8a); X64(0x1c); X64(0x25);
			EMIT_32LE(ptr);
			// add ax, bx
			X64(0x66); X64(0x01); X64(0xd8);
			// mov byte ptr [&context.I+1], ah
                        X64(0x88); X64(0x24); X64(0x25);
                        EMIT_32LE(ptr+1);
			// mov byte ptr [&context.I], al
                        X64(0x88); X64(0x04); X64(0x25);
                        EMIT_32LE(ptr);
			X64(0x90);
                        X64(0x90);
                        X64(0x90);
                        X64(0x90);


                }                                  
                else if ((current_instr & 0xF000) == 0xF000 && (current_instr & 0xFF) == 0x29)
                { // LD F, Vx
		  	uint8_t* ptr = &context.I;
		  	// xor rax, rax
			X64(0x48); X64(0x31); X64(0xc0);
			// mov al, byte ptr [&context.V[ins.x]]
			X64(0x8a); X64(0x04); X64(0x25);
			EMIT_32LE(&context.V[ins.x]);
			// mov bl, 5
			X64(0xb3); X64(0x05);
			// mul bl
			X64(0xf6); X64(0xe3);
                        // mov byte ptr [&context.I+1], ah
                        X64(0x88); X64(0x24); X64(0x25);
                        EMIT_32LE(ptr+1);
                        // mov byte ptr [&context.I], al
                        X64(0x88); X64(0x04); X64(0x25);
                        EMIT_32LE(ptr);

                }                                  
                else if ((current_instr & 0xF000) == 0xF000 && (current_instr & 0xFF) == 0x33)
                { // LD B, Vx
		  // use F7 div
		  	STOP;
			// xor rax, rax
			X64(0x48); X64(0x31); X64(0xc0);
                        // xor rcx, rcx
                        // mov cl, byte ptr [&context.I]
                        // mov ch, byte ptr [&context.I+1]
			// add rcx, &context.memory
			X64(0x48); X64(0x81); X64(0xc1);
			// mov al, byte ptr [&context.V[ins.x]]
			X64(0x8a); X64(0x04); X64(0x25);
			EMIT_32LE(&context.V[ins.x]);
			// mov bx, 1000
			X64(0x66); X64(0xbb); X64(0xe8);
			X64(0x03);
			// div bx 
			X64(0x66); X64(0xf7); X64(0xf3);
			// mov ax, dx
			X64(0x66); X64(0x89); X64(0xd0);
			// mov bx, 100
			X64(0x66); X64(0xbb);
			X64(0x64); X64(0x00);
			// div bx
			X64(0x66); X64(0xf7); X64(0xf3);
			// mov byte ptr [rcx], al
			X64(0x88); X64(0x01);

                        // mov al, byte ptr [&context.V[ins.x]]
			X64(0x8a); X64(0x04); X64(0x25);
			EMIT_32LE(&context.V[ins.x]);
                        // xor ah, ah
			X64(0x30); X64(0xe4);
                        // mov bx, 100
                        X64(0x66); X64(0xbb);
                        X64(0x64); X64(0x00);
                        // div bx 
			X64(0x66); X64(0xf7); X64(0xf3);
                        // mov ax, dx
			X64(0x66); X64(0x89); X64(0xd0);
                        // mov bx, 10
			X64(0x66); X64(0xbb); X64(0x0a); X64(0x00);
                        // div bx
			X64(0x66); X64(0xf7); X64(0xf3);
			// inc rcx
			X64(0x48); X64(0xff); X64(0xc1);
                        // mov byte ptr [rcx], al
			X64(0x88); X64(0x01);

	                // mov al, byte ptr [&context.V[ins.x]]
			X64(0x8a); X64(0x04); X64(0x25);
			EMIT_32LE(&context.V[ins.x]);
                        // xor ah, ah
			X64(0x30); X64(0xe4);
                        // mov bx, 10
			X64(0x66); X64(0xbb); X64(0x0a); X64(0x00);
                        // div bx 
			X64(0x66); X64(0xf7); X64(0xf3);
			// inc rcx
			X64(0x48); X64(0xff); X64(0xc1);
                        // mov byte ptr [rcx], al
			X64(0x88); X64(0x01);
                }                                  
                else if ((current_instr & 0xF000) == 0xF000 && (current_instr & 0xFF) == 0x55)
                { // LD [I], Vx
		  	// xor rcx, rcx
			// mov cl, byte ptr [&context.I]
			// mov ch, byte ptr [&context.I+1]
			// add rcx, &context.memory
                        X64(0x48); X64(0x81); X64(0xc1);
                        EMIT_32LE(&context.memory);
		  	// xor rax, rax
			X64(0x48); X64(0x31); X64(0xc0);
			// loop:
			// cmp rax, ins.x
			X64(0x48); X64(0x3d);
			EMIT_32LE(ins.x);
			// jg next
			X64(0x7f); X64(0x0e);
			// mov bl, byte ptr [rax + &context.V[0]]
			X64(0x8a); X64(0x98);
			EMIT_32LE(&context.V[0]);
			// mov byte ptr [rcx + rax], bl
			X64(0x88); X64(0x1c); X64(0x01);
			// inc rax
			X64(0x48); X64(0xff); X64(0xc0);
			// jmp loop
			X64(0xeb); X64(0xea);
			// next:
                }                                  
                else if ((current_instr & 0xF000) == 0xF000 && (current_instr & 0xFF) == 0x65)
                { // LD Vx, [I]
                        // mov rcx, context.I
                        X64(0x48); X64(0xc7); X64(0xc1);
                        EMIT_32LE(context.I);
                        // add rcx, &context.memory
                        X64(0x48); X64(0x81); X64(0xc1);
                        EMIT_32LE(&context.memory);
                        // xor rax, rax
                        X64(0x48); X64(0x31); X64(0xc0);
                        // loop:
                        // cmp rax, ins.x
                        X64(0x48); X64(0x3d);
                        EMIT_32LE(ins.x);
                        // jg next
                        X64(0x7f); X64(0x0e);
                        // mov bl, byte ptr [rax + rcx]
			X64(0x8a); X64(0x1c); X64(0x08);
                        // mov byte ptr [rax + &context.V[0]], bl
			X64(0x88); X64(0x98);
			EMIT_32LE(&context.V[0]);
                        // inc rax
			X64(0x48); X64(0xff); X64(0xc0);
                        // jmp loop
			X64(0xeb); X64(0xea);
                        // next:
                }                                  
		else
		{
			printf("error: unsupported opcode: %x\n", current_instr);
			exit(1);
		}

		if (fix_skip == 1) fix_skip++;
		else if (fix_skip == 2)
		{
			code[dest_i - emitted_instr - 1] = emitted_instr; // fixing jump
			fix_skip = 0;
		}
	}

	//pop rdx
	X64(0x5a);
	//pop rcx
	X64(0x59);
	//pop rbx
	X64(0x5b);
	//pop rax
	X64(0x58);

        X64(0xC9); // leave
	X64(0xC3); // ret

	return code;
}
