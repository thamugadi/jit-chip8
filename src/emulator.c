#include <chip8.h>

#define to_interpret(x) ((x == 0x00EE) || (x >> 12 == 1) || (x >> 12 == 2) || (x >> 12 == 0xB) || (x >> 12 == 0xD) || (x >> 12 == 3) || (x >> 12 == 4) || (x >> 12 == 5) || (x >> 12 == 9))

#define current(o) (uint16_t)((context.memory[context.pc+o] << 8) | context.memory[context.pc+1+o])

struct context_s context;

uint64_t recompiled_block = 0;

void emulate_basic_block()
{
	struct access_cache_s cache_a;

	uint8_t* recomp;

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
                	recomp = jit_recompile(&context.memory[context.pc], n);
                        void (*f)() = recomp;
                        f();
		}
		
		recompiled_block++;
		update_cache(recomp, n, context.pc);
		context.pc += 2*n;
        }
}

