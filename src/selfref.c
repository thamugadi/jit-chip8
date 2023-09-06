#include <chip8.h>

void mem_handler(uint8_t* addr)
{
	asm("push rax");
	asm("push rbx");
	asm("push rcx");
	asm("push rdx");
	asm("push r10");
	printf("a");
	asm("pop r10");
	asm("pop rdx");
	asm("pop rcx");
	asm("pop rbx");
	asm("pop rax");
}
