#include <chip8.h>

#define to_interpret(x) ((x == 0x00EE) || (x >> 12 == 1) || (x >> 12 == 2) || (x >> 12 == 0xB) || (x >> 12 == 3) || (x >> 12 == 4) || (x >> 12 == 5) || (x >> 12 == 9) || ((x >> 12) == 0xE) && (x & 0xff) == 0x9E || ((x >> 12) == 0xE) && (x & 0xff) == 0xA1 || (x >> 12 == 0xD) || ((x >> 12) == 0xF && ((x & 0xFF) == 0x0A)))
#define current(o) (uint16_t)(((int)context.memory[context.pc+o] << 8) | (int)context.memory[context.pc+1+o])

struct context_s context;
void* current_basic_block;
uint64_t recompiled_block = 0;
uint64_t g_emitted_bytes; 
void emulate_basic_block()
{
	struct access_cache_s cache_a;

	uint8_t* code;

        int n=0;

        if (to_interpret(current(0)))
        {
                interpret(current(0));
        }
     	else
	{
		cache_a = access_cache(context.pc, 1);
		if (cache_a.present)
		{
			n = cache_a.n;
			code = cache_a.addr;

			printf("Cached block at: %x\n", context.pc);
			g_emitted_bytes = cache_a.emitted_bytes;
			void (*f)() = code;
			f();
		}
		else
		{
                	while(!to_interpret(current(n*2)))
                	{
                        	n++; // instructions to recompile
                	}
			printf("Compiling block at: %x\n", context.pc);
			current_basic_block = context.pc;

			code =
			  mmap(0, n*MAX_EMITTED, 
			    PROT_READ|PROT_WRITE|PROT_EXEC, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
			if ((int64_t)code == -1)
			{
				printf("Failed to allocate memory");
			}

                	g_emitted_bytes = jit_recompile(code, &context.memory[context.pc], n);

			update_cache(code, n, context.pc, g_emitted_bytes, &context.memory[context.pc]);
                        void (*f)() = code;
                        f();
		}

		recompiled_block++;
		context.pc += 2*n;
        }
}

