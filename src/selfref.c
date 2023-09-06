#include <chip8.h>

uint64_t regs[5];

void mem_handler(uint8_t* addr)
{
	asm volatile 
	(
		"mov %0, rax\n\t"
		"mov %1, rbx\n\t"
		"mov %2, rcx\n\t"
		"mov %3, rdx\n\t"
		"mov %4, r10"
		: "=m" (regs[0]), "=m" (regs[1]), "=m" (regs[2]), "=m" (regs[3]), "=m" (regs[4])
		:
		: "rax", "rbx", "rcx", "rdx", "r10"
	);

	uint8_t* basic_block;
	uint64_t basic_block_size;
	uint64_t basic_block_offset;
	uint64_t n;
	uint8_t* code;

	int in_cache = 0;
        for (int i = 0; i < CACHE_SIZE; i++)
        {
		if (addr >= cache[i].addr && (addr - cache[i].addr) <= cache[i].emitted_bytes)
		{
			in_cache = 1;
			basic_block = cache[i].addr;
			basic_block_size = cache[i].emitted_bytes;
			basic_block_offset = addr - cache[i].addr;
			n = cache[i].n;
		}
        }
	if (in_cache)
	{
	}
	else
	{
	}

	asm volatile
	(
		"mov rax, %0\n\t"
		"mov rbx, %1\n\t"
		"mov rcx, %2\n\t"
		"mov rdx, %3\n\t"
		"mov r10, %4"
		:
		: "m" (regs[0]), "m" (regs[1]), "m" (regs[2]), "m" (regs[3]), "m" (regs[4])
		: "rax", "rbx", "rcx", "rdx", "r10"
	);
}
