#include <chip8.h>

void interpret(uint16_t instr)
{
	uint16_t addr = instr & 0x0FFF;
	printf("Interpreting: %x at %x\n", instr, context.pc);
	if (instr == 0x00EE) // RET
	{
		context.pc = context.stack[context.sp];
		context.sp--;
	}
	else if ((instr & 0xF000) == 0x1000) // JP addr
	{
		context.pc = addr;
	}
        else if ((instr & 0xF000) == 0x2000) // CALL addr
        {
		context.sp++;
		context.stack[context.sp] = context.pc + 2;
		context.pc = addr;
        }

        else if ((instr & 0xF000) == 0x3000) // SE Vx, byte
        {
                if (context.V[(instr & 0x0F00) >> 8] == (instr & 0xff))
		{
			context.pc += 4;
		}
		else
		{
			context.pc += 2;
		}
        }
        else if ((instr & 0xF000) == 0x4000) // SNE Vx, byte
        {
                if (context.V[(instr & 0x0F00) >> 8] != (instr & 0xff))
                {
                        context.pc += 4;
                }
                else
                {       
                        context.pc += 2;
                }
        }
        else if ((instr & 0xF000) == 0x5000) // SE Vx, Vy
        {
                if (context.V[(instr & 0x0F00) >> 8] == (context.V[(instr & 0x00F0) >> 4]))
                {
                        context.pc += 4;
                }
                else
                {       
                        context.pc += 2;
                }
        }
        else if ((instr & 0xF000) == 0x9000) // SNE Vx, Vy
        {
                if (context.V[(instr & 0x0F00) >> 8] != (context.V[(instr & 0x00F0) >> 4]))
                {
                        context.pc += 4;
                }
                else
                {       
                        context.pc += 2;
                }
        }

	else if ((instr & 0xF000) == 0xE000 && (instr & 0xFF) == 0x9E) //SKP Vx
	{
		if (context.keys[context.V[(instr & 0x0F00) >> 8]])
		{
			context.pc += 4;
		}
		else
		{
			context.pc += 2;
		}
	}
        else if ((instr & 0xF000) == 0xE000 && (instr & 0xFF) == 0xA1) //SKNP Vx
        {
                if (!(context.keys[context.V[(instr & 0x0F00) >> 8]]))
                {
                        context.pc += 4;
                }
                else
                {       
                        context.pc += 2;
                }
        }


        
        else if ((instr & 0xF000) == 0xB000) // JP V0, addr
        {
		context.pc = addr + context.V[0];
        }
	else if ((instr & 0xF000) == 0xD000) // DRW Vx, Vy, nibble (temporary)
	{
		uint64_t xi, yi, x, y, n;
		xi = (instr & 0x0F00) >> 8;
		yi = (instr & 0x00F0) >> 4;

		x = context.V[xi];
		y = context.V[yi];

		n = instr & 0xF;

		context.V[15] = 0;
		for (int i = 0; i < n; i++)
		{
			uint8_t src = context.memory[context.I + i];
			for (int j = 0; j < 8; j++)
			{
				uint8_t bit = (src << j) & 0b10000000;
				if (bit && (context.gfx[(x+j)%WIDTH][32 - (y+i)%HEIGHT]))
				{
					context.V[15] = 1;
				}

				context.gfx[(x+j)%WIDTH][31 - (y+i)%HEIGHT] ^= bit;
			}
		}
		context.pc += 2;

	}
        else if ((instr & 0xF000) == 0xF000) // LD Vx, K 
	{
		for (int i = 0; i < 16; i++)
		{
			if (context.keys[i] == 1)
			{
					context.V[(instr & 0x0F00) >> 8] = i;
					context.pc += 2;
					break;
			}
		}
        }
}

