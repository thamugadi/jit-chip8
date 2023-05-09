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

	else if ((instr & 0xF000) == 0xB000) // DRW Vx, Vy, nibble (temporary)
	{
		uint8_t x, y, n;
		x = (instr & 0x0F00) >> 8;
		y = (instr & 0x00F0) >> 4;
		n = instr & 0xF;

		context.V[15] = 0;
		for (int i = 0; i < n; i++)
		{
			uint8_t src = context.memory[context.I + i];
			for (int j = 0; j < 8; j++)
			{
				uint8_t bit = (src >> j) & 1;

				if (bit && (context.gfx[(x+7-j)%WIDTH][(y+i)%HEIGHT]))
				{
					context.V[15] = 1;
				}

				context.gfx[(x+7-j)%WIDTH][(y+i)%HEIGHT] ^= bit;
			}
		}
	}
}

