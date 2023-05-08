#include <chip8.h>
#include <GL/glut.h>

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

	glutMainLoop();

	while(1)
	{
		emulate_basic_block();
	}
}
