#include <chip8.h>

#define CODE(x) code_domain[p] = x; p++;

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

