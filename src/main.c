#include <chip8.h>
#include <GL/glut.h>

#define DRAW_POS(px,py) \
        glVertex2i(px, py); \
        glVertex2i(px+PIXELSIZE, py); \
        glVertex2i(px+PIXELSIZE, py+PIXELSIZE); \
        glVertex2i(px, py+PIXELSIZE);

void display()
{
        glClear(GL_COLOR_BUFFER_BIT);

        glColor3f(1.0, 1.0, 1.0);
        glBegin(GL_QUADS);

	// emulation:
	emulate_basic_block();

        if (context.dt)
        {                                       
                context.dt--;
        }


        for (int i = 0; i < WIDTH; i++)
        {
                for (int j = 0; j < HEIGHT; j++)
                {
                        if (context.gfx[i][j])
                        {
                                DRAW_POS(i, j);
                        }
                }
        }
        glEnd();

        glutSwapBuffers();
}

void initGL()
{
        gluOrtho2D(0, WIDTH * PIXELSIZE, 0, HEIGHT * PIXELSIZE);
        glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		puts("format : jit-chip8 <chip-8 rom>");
		return 1;
	}

	FILE *file = fopen(argv[1], "rb");

	if (!file)
	{
        	fprintf(stderr, "Error: Failed to open file '%s'.\n", argv[1]);
        	return 2;
    	}

	size_t bytes_read = fread(&context.memory[0x200], 1, 0x800, file);

	if (bytes_read == 0x800 && !feof(file)) 
	{
        	fprintf(stderr, "Error: File size is larger than 0x800. \n");
        	fclose(file);
		return 3;
	}

	fclose(file);

	context.sp = 0;
	context.pc = 0x200;
	context.I = 0;
	context.dt = 0;
	context.st = 0;

	memset(context.V, 0, 16);
	memset(context.gfx, 0, WIDTH * HEIGHT);
	memset(context.stack, 0, 2 * 16);
	memset(context.keys, 0, 16);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

	glutInitWindowSize(WIDTH * PIXELSIZE, HEIGHT * PIXELSIZE);
	
	glutCreateWindow("CHIP-8 JIT compiler");

	initGL();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboardDown);
    	glutKeyboardUpFunc(keyboardUp);

	glutIdleFunc(display);

	glutMainLoop();
}
