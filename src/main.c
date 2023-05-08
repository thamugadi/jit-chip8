#include <chip8.h>
#include <GL/glut.h>

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

	glutInitWindowSize(WIDTH * PIXELSIZE, HEIGHT * PIXELSIZE);
	
	glutCreateWindow("CHIP-8 JIT compiler");

	initGL();

	//glutDisplayFunc(display);
	//glutKeyboardFunc(keyboard);

	//glutMainLoop();
}
