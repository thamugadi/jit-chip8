#include <GL/glut.h>

int width = 64;
int height = 32;
int pixelSize = 1;

#define DRAW_POS(px,py) \
        glVertex2i(px, py); \
        glVertex2i(px+pixelSize, py); \
        glVertex2i(px+pixelSize, py+pixelSize); \
        glVertex2i(px, py+pixelSize);


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
	gluOrtho2D(0, width * pixelSize, 0, height * pixelSize);
	glMatrixMode(GL_MODELVIEW);
}
