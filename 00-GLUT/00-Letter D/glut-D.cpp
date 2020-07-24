#include <GL\freeglut.h>

bool bFullscreen = false;

int main(int argc, char** argv)
{
	// code
	// function declarations
	void initialize(void);
	void resize(int, int);
	void display(void);
	void keyboard(unsigned char, int, int);
	void mouse(int, int, int, int);
	void uninitialize(void);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("GLUT: Letter D");

	initialize();

	glutDisplayFunc(display);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutCloseFunc(uninitialize);

	glutMainLoop();

	return(0);  // this line is not necessary
}

void initialize(void)
{
	// code
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void resize(int width, int height)
{
	// code
	if (height <= 0)
		height = 1;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
}

void display(void)
{
	// function declarations
	void LetterD(float pos[], float scale);

	// code
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	float pos[] = { 1.0f, 1.0f, 0.0f };
	LetterD(pos, 0.75f);

	
	glFlush();
}

void keyboard(unsigned char key, int x, int y)
{
	// code
	switch (key)
	{
	case 27:
		glutLeaveMainLoop();
		break;

	case 'F':
	case 'f':
		if (bFullscreen == false)
		{
			glutFullScreen();
			bFullscreen = true;
		}
		else
		{
			glutLeaveFullScreen();
			bFullscreen = false;
		}
		break;

	default:
		break;
	}
}

void mouse(int button, int mode, int x, int y)
{
	// code
	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		break;

	case GLUT_RIGHT_BUTTON:
		glutLeaveMainLoop();
		break;

	default:
		break;
	}
}

void uninitialize(void)
{
	// code
}

void LetterD(float pos[], float scale)
{
	// variables 
	float dx = -0.02f, dy = -0.02f;

	// code
	glMatrixMode(GL_MODELVIEW);

	glScalef(scale, scale, scale);
	glTranslatef(pos[0], pos[1], pos[2]);

	glBegin(GL_QUADS);
	{
		// shadow for letter
		glColor3f(0.0f, 0.5f, 0.0f);

		glVertex2f(dx+ -0.2f + -0.04f, -0.8f + dy);
		glVertex2f(dx+ -0.2f + -0.04f, 0.8f + dy);
		glVertex2f(dx+ -0.2f + 0.04f, 0.8f + dy);
		glVertex2f(dx+ -0.2f + 0.04f, -0.8f + dy);

		glVertex2f(dx+ 0.2f + -0.04f, -0.8f + dy);
		glVertex2f(dx+ 0.2f + -0.04f, 0.8f + dy);
		glVertex2f(dx+ 0.2f + 0.04f, 0.8f + dy);
		glVertex2f(dx+ 0.2f + 0.04f, -0.8f + dy);

		glVertex2f(dx+ -0.3f + -0.03f, 0.85f + dy);
		glVertex2f(dx+ 0.25f + -0.03f, 0.85f + dy);
		glVertex2f(dx+ 0.25f + -0.03f, 0.75f + dy);
		glVertex2f(dx+ -0.3f + -0.03f, 0.75f + dy);

		glVertex2f(dx+ -0.3f + -0.03f, -0.75f + dy);
		glVertex2f(dx+ 0.25f + -0.03f, -0.75f + dy);
		glVertex2f(dx+ 0.25f + -0.03f, -0.85f + dy);
		glVertex2f(dx+ -0.3f + -0.03f, -0.85f + dy);

		// shadow for letter
		dx = -0.02f;
		dy = -0.00f;
		glColor3f(0.0f, 0.5f, 0.0f);

		glVertex2f(dx+ -0.2f + -0.04f, -0.8f + dy);
		glVertex2f(dx+ -0.2f + -0.04f, 0.8f + dy);
		glVertex2f(dx+ -0.2f + 0.04f, 0.8f + dy);
		glVertex2f(dx+ -0.2f + 0.04f, -0.8f + dy);

		glVertex2f(dx+ 0.2f + -0.04f, -0.8f + dy);
		glVertex2f(dx+ 0.2f + -0.04f, 0.8f + dy);
		glVertex2f(dx+ 0.2f + 0.04f, 0.8f + dy);
		glVertex2f(dx+ 0.2f + 0.04f, -0.8f + dy);

		glVertex2f(dx+ -0.3f + -0.03f, 0.85f + dy);
		glVertex2f(dx+ 0.25f + -0.03f, 0.85f + dy);
		glVertex2f(dx+ 0.25f + -0.03f, 0.75f + dy);
		glVertex2f(dx+ -0.3f + -0.03f, 0.75f + dy);

		glVertex2f(dx+ -0.3f + -0.03f, -0.75f + dy);
		glVertex2f(dx+ 0.25f + -0.03f, -0.75f + dy);
		glVertex2f(dx+ 0.25f + -0.03f, -0.85f + dy);
		glVertex2f(dx+ -0.3f + -0.03f, -0.85f + dy);

		// glow for letter
		dx = -dx;
		dy = -dy;
		
		glColor3f(0.0f, 1.0f, 0.0f);
		// glColor3f(0.0f, 0.0f, 0.65f);

		glVertex2f(dx+ -0.2f + -0.04f, -0.8f + dy);
		glVertex2f(dx+ -0.2f + -0.04f, 0.8f + dy);
		glVertex2f(dx+ -0.2f + 0.04f, 0.8f + dy);
		glVertex2f(dx+ -0.2f + 0.04f, -0.8f + dy);

		glVertex2f(dx+ 0.2f + -0.04f, -0.8f + dy);
		glVertex2f(dx+ 0.2f + -0.04f, 0.8f + dy);
		glVertex2f(dx+ 0.2f + 0.04f, 0.8f + dy);
		glVertex2f(dx+ 0.2f + 0.04f, -0.8f + dy);

		glVertex2f(dx+ -0.3f + -0.03f, 0.85f + dy);
		glVertex2f(dx+ 0.25f + -0.03f, 0.85f + dy);
		glVertex2f(dx+ 0.25f + -0.03f, 0.75f + dy);
		glVertex2f(dx+ -0.3f + -0.03f, 0.75f + dy);

		glVertex2f(dx+ -0.3f + -0.03f, -0.75f + dy);
		glVertex2f(dx+ 0.25f + -0.03f, -0.75f + dy);
		glVertex2f(dx+ 0.25f + -0.03f, -0.85f + dy);
		glVertex2f(dx+ -0.3f + -0.03f, -0.85f + dy);

		// glow for letter
		dx = dx;
		dy = dy + 0.02f;
		
		glColor3f(0.0f, 1.0f, 0.0f);
		// glColor3f(0.0f, 0.0f, 0.65f);

		glVertex2f(dx+ -0.2f + -0.04f, -0.8f + dy);
		glVertex2f(dx+ -0.2f + -0.04f, 0.8f + dy);
		glVertex2f(dx+ -0.2f + 0.04f, 0.8f + dy);
		glVertex2f(dx+ -0.2f + 0.04f, -0.8f + dy);

		glVertex2f(dx+ 0.2f + -0.04f, -0.8f + dy);
		glVertex2f(dx+ 0.2f + -0.04f, 0.8f + dy);
		glVertex2f(dx+ 0.2f + 0.04f, 0.8f + dy);
		glVertex2f(dx+ 0.2f + 0.04f, -0.8f + dy);

		glVertex2f(dx+ -0.3f + -0.03f, 0.85f + dy);
		glVertex2f(dx+ 0.25f + -0.03f, 0.85f + dy);
		glVertex2f(dx+ 0.25f + -0.03f, 0.75f + dy);
		glVertex2f(dx+ -0.3f + -0.03f, 0.75f + dy);

		glVertex2f(dx+ -0.3f + -0.03f, -0.75f + dy);
		glVertex2f(dx+ 0.25f + -0.03f, -0.75f + dy);
		glVertex2f(dx+ 0.25f + -0.03f, -0.85f + dy);
		glVertex2f(dx+ -0.3f + -0.03f, -0.85f + dy);

		// glow for letter
		dx = dx - 0.02f;
		dy = dy;
		
		glColor3f(0.0f, 1.0f, 0.0f);
		// glColor3f(0.0f, 0.0f, 0.65f);

		glVertex2f(dx+ -0.2f + -0.04f, -0.8f + dy);
		glVertex2f(dx+ -0.2f + -0.04f, 0.8f + dy);
		glVertex2f(dx+ -0.2f + 0.04f, 0.8f + dy);
		glVertex2f(dx+ -0.2f + 0.04f, -0.8f + dy);

		glVertex2f(dx+ 0.2f + -0.04f, -0.8f + dy);
		glVertex2f(dx+ 0.2f + -0.04f, 0.8f + dy);
		glVertex2f(dx+ 0.2f + 0.04f, 0.8f + dy);
		glVertex2f(dx+ 0.2f + 0.04f, -0.8f + dy);

		glVertex2f(dx+ -0.3f + -0.03f, 0.85f + dy);
		glVertex2f(dx+ 0.25f + -0.03f, 0.85f + dy);
		glVertex2f(dx+ 0.25f + -0.03f, 0.75f + dy);
		glVertex2f(dx+ -0.3f + -0.03f, 0.75f + dy);

		glVertex2f(dx+ -0.3f + -0.03f, -0.75f + dy);
		glVertex2f(dx+ 0.25f + -0.03f, -0.75f + dy);
		glVertex2f(dx+ 0.25f + -0.03f, -0.85f + dy);
		glVertex2f(dx+ -0.3f + -0.03f, -0.85f + dy);


		// letter D
		glColor3f(0.0f, 0.8f, 0.0f);
		glVertex2f(-0.2f + -0.04f, -0.8f);

		glColor3f(0.0f, 0.7f, 0.0f);
		glVertex2f(-0.2f + -0.04f, 0.8f);

		glColor3f(0.0f, 0.7f, 0.0f);
		glVertex2f(-0.2f + 0.04f, 0.8f);
		
		glColor3f(0.0f, 0.8f, 0.0f);
		glVertex2f(-0.2f + 0.04f, -0.8f);


		glColor3f(0.0f, 0.8f, 0.0f);
		glVertex2f(0.2f + -0.04f, -0.8f);

		glColor3f(0.0f, 0.7f, 0.0f);
		glVertex2f(0.2f + -0.04f, 0.8f);

		glColor3f(0.0f, 0.7f, 0.0f);
		glVertex2f(0.2f + 0.04f, 0.8f);

		glColor3f(0.0f, 0.8f, 0.0f);
		glVertex2f(0.2f + 0.04f, -0.8f);


		glColor3f(0.0f, 0.7f, 0.0f);
		glVertex2f(-0.3f + -0.03f, 0.85f);
		glVertex2f(0.25f + -0.03f, 0.85f);
		glVertex2f(0.25f + -0.03f, 0.75f);
		glVertex2f(-0.3f + -0.03f, 0.75f);


		glColor3f(0.0f, 0.8f, 0.0f);
		glVertex2f(-0.3f + -0.03f, -0.75f);
		glVertex2f(0.25f + -0.03f, -0.75f);
		glVertex2f(0.25f + -0.03f, -0.85f);
		glVertex2f(-0.3f + -0.03f, -0.85f);

	}
	glEnd();
}
