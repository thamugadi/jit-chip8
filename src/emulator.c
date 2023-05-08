#include <chip8.h>

#define to_interpret(x) ((context.memory[x] == 0x00EE) || (context.memory[x] >> 12 == 1) || (context.memory[x] >> 12 == 2) || (context.memory[x] >> 12) == 0xB) // RET, JP, CALL

struct context_s context;

uint64_t recompiled_block = 0;

void emulate_basic_block()
{
	struct access_cache_s cache_a;

	uint8_t* recomp;

        int n=0;

        if (to_interpret(context.pc))
        {
                interpret(context.memory[context.pc]);
                context.pc++;
        }

        else
        {
		cache_a = access_cache(context.pc);
		if (cache_a.present)
		{
			n = cache_a.n;
			recomp = cache_a.addr;

			void (*f)() = recomp;
			f();
		}
		else
		{
                	while(!to_interpret(context.pc+n))
                	{
                        	n++; // instructions to recompile
                	}
                	recomp = jit_recompile(&context.memory[context.pc], n);
                        void (*f)() = recomp;
                        f();
		}
		
		recompiled_block++;
		update_cache(recomp, n, context.pc);
		context.pc += n;
        }
}

