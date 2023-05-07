#include <chip8.h>

#define memory context.memory
#define pc context.pc
#define to_interpret(x) ((memory[x] >> 12) <= 0x2 || (memory[x] >> 12) == 0xB || memory[x]>>12==0xD || to_interpret_f(x))

#define to_interpret_f(x) ((memory[x] >> 12) == 0xF && (!(memory[x] & 0x00F0)))

struct context_s context;

void emulate()
{
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

