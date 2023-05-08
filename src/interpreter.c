#include <chip8.h>

void interpret(uint16_t instr)
{
	uint16_t addr = instr & 0x0FFF;
	if (instr == 0x00EE) // RET
	{
		context.pc = context.sp;
		context.sp--;
	}
	else if ((instr & 0xF000) == 0x1000) // JP addr
	{
		context.pc = addr;
	}
        else if ((instr & 0xF000) == 0x2000) // CALL addr
        {
		context.sp++;
		context.stack[context.sp] = context.pc;
		context.pc = addr;
        }
        
        else if ((instr & 0xF000) == 0xB000) // JP V0, addr
        {
		context.pc = addr + context.V[0];
        }
}

