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

	// saved rip at rbp + 8
	void** saved_rip_ptr;
	asm("lea %0, [rbp+8]" : "=r" (saved_rip_ptr));
	void* saved_rip = __builtin_return_address(0);

	uint8_t* basic_block;
	uint16_t basic_block_pc;
	uint8_t* code;

	int in_cache = 0;
	int i;
	int diff;

	uint16_t corresponding_pc;
        for (i = 0; i < CACHE_SIZE; i++)
        {
		if (cache[i].addr == 0xffffffffffffffff) break;
		if ((int64_t)addr >= (int64_t)cache[i].addr && (uint64_t)(addr - cache[i].addr ) < cache[i].emitted_bytes)
		{
			asm("jmp $");
			printf("%x\n", addr);
			printf("%x\n", cache[i].addr);
			in_cache = 1;
			basic_block = cache[i].addr;
			basic_block_pc = cache[i].pc;
			break;
		}
        }
	if (in_cache)
	{
		printf("SMC: Invalidating cache entry n°%d\n", i);
		if (basic_block_pc == context.pc)
		{
			munmap(basic_block, cache[i].n * MAX_EMITTED);
			int64_t offset_rip = (int64_t)saved_rip - (int64_t)(cache[i].addr);
                        code =
                          mmap(0, cache[i].n*MAX_EMITTED, 
                            PROT_READ|PROT_WRITE|PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
                        if ((int64_t)code == -1)
                        {
                                printf("Failed to allocate memory\n");
                                exit(-1);
                        }
                        g_emitted_bytes = jit_recompile(code, &context.memory[context.pc], cache[i].n);

			for (diff = 0; diff < (int64_t)saved_rip-(int64_t)cache[i].addr; diff++)
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

			printf("%x\n", *saved_rip_ptr);
			printf("Offset rip: %d\n", offset_rip);

			*saved_rip_ptr = (void*)((int64_t)code + (int64_t)offset_rip);
			printf("%x\n", *saved_rip_ptr);
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
		"mov r10, %4"
		:
		: "m" (regs[0]), "m" (regs[1]), "m" (regs[2]), "m" (regs[3]), "m" (regs[4])
		: "rax", "rbx", "rcx", "rdx", "r10"
	);
}
