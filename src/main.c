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

	// todo: init chip-8

	context.sp = 0;
	context.pc = 0;
	for (int i = 0; i < 16; i++) context.V[i] = 0;
	context.I = 0;
	context.dt = 0;
	context.st = 0;
	for (int i = 0; i < 64; i++) for (int j = 0; j < 32; j++) context.gfx[i][j] = 0;
	for (int i = 0; i < 16; i++) context.stack[i] = 0;

	// todo: load ROM

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

	glutInitWindowSize(WIDTH * PIXELSIZE, HEIGHT * PIXELSIZE);
	
	glutCreateWindow("CHIP-8 JIT compiler");

	initGL();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboardDown);
    	glutKeyboardUpFunc(keyboardUp);

	glutMainLoop();
}
