#include <chip8.h>

#define to_interpret(x) ((x == 0x00EE) || (x >> 12 == 1) || (x >> 12 == 2) || (x >> 12 == 0xB) || (x >> 12 == 3) || (x >> 12 == 4) || (x >> 12 == 5) || (x >> 12 == 9) || ((x >> 12) == 0xE) && (x & 0xff) == 0x9E || ((x >> 12) == 0xE) && (x & 0xff) == 0xA1 || (x >> 12 == 0xD))
#define current(o) (uint16_t)(((int)context.memory[context.pc+o] << 8) | (int)context.memory[context.pc+1+o])

struct context_s context;

uint64_t recompiled_block = 0;

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
		cache_a = access_cache(context.pc);
		if (cache_a.present)
		{
			n = cache_a.n;
			code = cache_a.addr;

			printf("Cached block at: %x\n", context.pc);

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
			code = mmap(0, n*MAX_EMITTED, 7, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
                	jit_recompile(code, &context.memory[context.pc], n);

//TODO
//			mprotect(context.memory, 0x1000, 0);
                        void (*f)() = code; 
			exec_jit = 1;
                        f();
			exec_jit = 0;
			update_cache(code, n, context.pc);
		}

		recompiled_block++;
		context.pc += 2*n;
        }
}

