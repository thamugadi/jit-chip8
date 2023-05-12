#include <chip8.h>

#define to_interpret(x) ((x == 0x00EE) || (x >> 12 == 1) || (x >> 12 == 2) || (x >> 12 == 0xB) || (x >> 12 == 0xD) || (x >> 12 == 3) || (x >> 12 == 4) || (x >> 12 == 5) || (x >> 12 == 9) || ((x >> 12) == 0xF) && (x & 0xff) == 0x0A || ((x >> 12) == 0xE) && (x & 0xff) == 0x9E || ((x >> 12) == 0xE) && (x & 0xff) == 0xA1)
#define current(o) (uint16_t)((context.memory[context.pc+o] << 8) | context.memory[context.pc+1+o])

struct context_s context;

uint64_t recompiled_block = 0;

void emulate_basic_block()
{
	uint8_t* recomp;
	struct access_cache_s cache_a;

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
			recomp = cache_a.addr;

			printf("Cached block at: %x\n", context.pc);

			void (*f)() = recomp;
			f();
		}
		else
		{
                	while(!to_interpret(current(n*2)))
                	{
                        	n++; // instructions to recompile
                	}
			printf("Compiling block at: %x\n", context.pc);
                	recomp = jit_recompile(&context.memory[context.pc], n);
                        void (*f)() = recomp;
                        f();
			update_cache(recomp, n, context.pc);
		}

		recompiled_block++;
		context.pc += 2*n;
        }
}

