#include <chip8.h>

#define CODE(x) code_domain[p] = x; p++;
struct context context;
/*
void mov_rsp_st(void)
{
        asm volatile ("mov rsp, qword ptr [%0]\n\t" : : "r" ((&context.stack)) : "rsp");
}
*/
struct compiled_s jit_recompile(uint16_t* instr, int n)
{
}

void jit_execute(uint8_t* compiled, int size, int newpc)
{
        int p = 0;
        uint8_t* code_domain = mmap(0, 0x1000, 7, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        CODE(0x55); //push rbp
        CODE(0x48); CODE(0x89); CODE(0xE5); //mov rsp, rbp
        //todo: decrement RSP

        for (int i = 0; i < size; i++)
        {
                CODE(compiled[size]);
        }

        CODE(0xC9); CODE(0xC3); // leave; ret

        //todo: copy all context (except memory) in registers
        void (*code)() = code_domain;
        code();
        //todo: copy all registers in context
        //todo: update PC
}
void interpret(uint16_t instr)
{

}
#define memory context.memory
#define pc context.pc
#define to_interpret(x) (memory[x] == 0x00E0 || (memory[x] >> 12) >= 0xC)
void handle()
{
	int n=0;

        if (to_interpret(pc))
        {
                interpret(memory[pc]);
        }
        else
        {
		while(!to_interpret(pc+n))
		{
			n++; // instructions to recompile
		}
		struct compiled_s recomp = jit_recompile(&memory[pc], n);

		jit_execute(&recomp.code, recomp.size, recomp.new_pc);

                //give to jit_recompile a reference to memory[pc] with the number of instructions to recompile, being the number of instructions before a JMP, a CALL or an I/O instruction
                //execute it with jit_execute
        }
}
int main()
{
}
