#include <chip8.h>

#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define memory context.memory
#define pc context.pc

#define to_interpret(x) ((memory[x] == 0x00EE) || (memory[x] >> 12 == 1) || (memory[x] >> 12 == 2) || (memory[x] >> 12) == 0xB) // RET, JP, CALL

#define INTERVAL_MS 16 // Approximately 60Hz (1000ms / 60 = 16.6667ms)

struct context_s context;

void* incr()
{
	struct timespec interval = {0, INTERVAL_MS * 1000000L}; // ms to ns
	while(1)
	{
		nanosleep(&interval, 0);
		context.dt++;
	}
	return 0;
}
void emulate()
{
	pthread_t incr_thread;
    	pthread_create(&incr_thread, NULL, incr, NULL);
        int n=0;

        if (to_interpret(pc))
        {
                interpret(memory[pc]);
                pc++;
        }
        else
        {
                while(!to_interpret(pc+n))
                {
                        n++; // instructions to recompile
                }
                struct compiled_s recomp = jit_recompile(&memory[pc], n);
		//add to cache (todo)

                jit_execute(recomp.code, recomp.size, recomp.new_pc);

                //give to jit_recompile a reference to memory[pc] with the number of instructions to recompile
                //execute it with jit_execute
        }
	// handle interrupts
}

