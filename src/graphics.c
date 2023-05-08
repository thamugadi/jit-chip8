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
	// draw
	glEnd();

	glutSwapBuffers();
}

void initGL()
{
	gluOrtho2D(0, WIDTH * PIXELSIZE, 0, HEIGHT * PIXELSIZE);
	glMatrixMode(GL_MODELVIEW);
}
