#include <chip8.h>

uint64_t regs[5];

void mem_handler(uint8_t* addr)
{
        volatile void** saved_rip;
	asm volatile 
	(
		"mov %0, rsp\n\t"
		: "=r" (saved_rip)
	);
	saved_rip += 11;

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
	uint16_t basic_block_pc;
	uint8_t* code;

	int in_cache = 0;
	int i;
	int diff;

	uint16_t corresponding_pc;
        for (i = 0; i < CACHE_SIZE; i++)
        {
		if (addr >= cache[i].mem_address && (addr - cache[i].mem_address) < cache[i].n)
		{
			in_cache = 1;
			basic_block = cache[i].addr;
			basic_block_pc = cache[i].pc;
			break;
		}
        }
	if (in_cache)
	{
		printf("SMC: Invalidating cache entry n°%d\n", i);
		if (cache[i].mem_address == &context.memory[context.pc])
		{
			printf("%llx\n", cache[i].addr);
			printf("%llx\n", saved_rip);
			// should be on the same chunk.
			uint64_t offset_rip = (uint64_t)saved_rip - (uint64_t)(cache[i].addr);
                        code =
                          mmap(0, cache[i].n*MAX_EMITTED, 
                            PROT_READ|PROT_WRITE|PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
                        if ((int64_t)code == -1)
                        {
                                printf("Failed to allocate memory\n");
                                exit(-1);
                        }
                        g_emitted_bytes = jit_recompile(code, &context.memory[context.pc], cache[i].n);

			for (diff = 0; diff < offset_rip; diff++)
			{
				if (*(code+diff) != *(cache[i].addr+diff))
				{
					break;
				}
			}

			if (diff < (uint8_t*)saved_rip - cache[i].addr)
			{
				offset_rip += g_emitted_bytes - cache[i].emitted_bytes;
			}

                        cache[i].addr = code;
			cache[i].emitted_bytes = g_emitted_bytes;
			cache[i].mem_address = addr;

			printf("%llx\n", (uint64_t)code);
			printf("%llx\n", code+offset_rip);

			*saved_rip = (void*)((uint64_t)code + (uint64_t)offset_rip);
			munmap(basic_block, cache[i].n * MAX_EMITTED);
		}
		else
		{
			munmap(basic_block, cache[i].n * MAX_EMITTED);
			printf("%x", cache[i].n);
                        code =
                          mmap(0, cache[i].n*MAX_EMITTED, 
                            PROT_READ|PROT_WRITE|PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
                        if ((int64_t)code == -1)
                        {
                                printf("Failed to allocate memory\n");
                                exit(-1);        
                        }
                        g_emitted_bytes = jit_recompile(code, &context.memory[context.pc], cache[i].n);
			cache[i].addr = code;
			cache[i].emitted_bytes = g_emitted_bytes;
		}
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
		"mov r10, %4\n\t"
		:
		: "m" (regs[0]), "m" (regs[1]), "m" (regs[2]), "m" (regs[3]), "m" (regs[4])
		: "rax", "rbx", "rcx", "rdx", "r10"
	);

}
