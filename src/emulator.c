#include <chip8.h>

#define memory context.memory
#define pc context.pc
#define to_interpret(x) ((memory[x] >> 12) <= 0x5 || (memory[x] >> 12) >= 0xC || memory[x]>>12==9) 

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

                jit_execute(recomp.code, recomp.size, recomp.new_pc);

                //give to jit_recompile a reference to memory[pc] with the number of instructions to recompile, being the number of instructions before a JMP, a CALL or an I/O instruction
                //execute it with jit_execute
        }
}

