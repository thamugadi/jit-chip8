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

	// rbp + 8
	void** saved_rip_ptr;
	asm("lea %0, [rbp+8]" : "=r" (saved_rip_ptr));
	void* saved_rip = __builtin_return_address(0);

	uint8_t* basic_block;
	uint64_t basic_block_size;
	uint64_t basic_block_offset;
	uint64_t n;
	uint16_t basic_block_pc;
	uint8_t* code;

	int in_cache = 0;
	int i;
	int diff;
        for (i = 0; i < CACHE_SIZE; i++)
        {
		if (addr-context.memory >= cache[i].pc && (addr-context.memory - cache[i].pc ) <= cache[i].n)
		{
			in_cache = 1;
			basic_block = cache[i].addr;
			basic_block_size = cache[i].emitted_bytes;
			basic_block_offset = cache[i].addr + cache[i].emitted_bytes;
			basic_block_pc = cache[i].pc;
			n = cache[i].n;
			break;
		}
        }
	if (in_cache)
	{
		if (basic_block_pc == context.pc)
		{
			void* offset_rip = saved_rip - (void*)(cache[i].addr);
                        code =
                          mmap(0, n*MAX_EMITTED+0x100, 
                            PROT_READ|PROT_WRITE|PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
                        g_emitted_bytes = jit_recompile(code, &context.memory[context.pc], n);
			
			for (diff = 0; diff < (uint8_t*)saved_rip-cache[i].addr; diff++)
			{
				if (*(code+diff) != *cache[i].addr+diff)
				{
					break;
				}
			}

			if ((code + diff) < (uint8_t*)saved_rip)
			{
				int delta = g_emitted_bytes - cache[i].emitted_bytes;
				offset_rip += delta;
			}

                        cache[i].addr = code;
			cache[i].emitted_bytes = g_emitted_bytes;
                        munmap(basic_block, n * MAX_EMITTED);

			*saved_rip_ptr = (void*)((uint64_t)code + (uint64_t)offset_rip);
			// TODO: patch RIP
		}
		else
		{
                        code =
                          mmap(0, n*MAX_EMITTED+0x100, 
                            PROT_READ|PROT_WRITE|PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
                        g_emitted_bytes = jit_recompile(code, &context.memory[context.pc], n);
			cache[i].addr = code;
			cache[i].emitted_bytes = g_emitted_bytes;
			munmap(basic_block, n * MAX_EMITTED);
		}
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
