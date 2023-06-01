#include <chip8.h>

void interpret(uint16_t instr)
{
	uint16_t addr = instr & 0x0FFF;
	printf("Interpreting: %x at %x\n", instr, context.pc);
	if (instr == 0x00EE) // RET
	{
		context.pc = context.stack[--context.sp];
	}
	else if ((instr & 0xF000) == 0x1000) // JP addr
	{
		context.pc = addr;
/*                printf("V0: %x\n", context.V[0]);
                printf("V1: %x\n", context.V[1]);
                printf("I: %x\n", context.I);
                printf("PC: %x\n", context.pc);    
*/
	}
        else if ((instr & 0xF000) == 0x2000) // CALL addr
        {
		context.stack[context.sp++] = context.pc + 2;
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

}

