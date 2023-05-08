#include <chip8.h>

#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define to_interpret(x) ((context.memory[x] == 0x00EE) || (context.memory[x] >> 12 == 1) || (context.memory[x] >> 12 == 2) || (context.memory[x] >> 12) == 0xB) // RET, JP, CALL

#define INTERVAL_MS 16 // Approximately 60Hz (1000ms / 60 = 16.6667ms)

struct context_s context;

uint64_t recompiled_instr = 0;

void emulate()
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
		}
		else
		{
                	while(!to_interpret(context.pc+n))
                	{
                        	n++; // instructions to recompile
                	}
                	recomp = jit_recompile(&context.memory[context.pc], n);
		}
		
		recompiled_instr++;
		update_cache(recomp, n, context.pc);
		context.pc += n;
        }
}

