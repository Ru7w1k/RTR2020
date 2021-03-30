// headers
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <X11/keysym.h>

#include <GL/gl.h>   // for OpenGL APIs
#include <GL/glu.h>  // for OpenGL utility functions
#include <GL/glx.h>  // for GLX APIs (bridging API)

#include <AL/al.h>   // OpenAL
#include <AL/alc.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include "wavhelper.h"

// namespace
using namespace std;

// global variable declarations
Display *gpDisplay = NULL;
Window gWindow;
XVisualInfo *gpXVisualInfo = NULL;
Colormap gColormap;

GLXContext gGLXContext;

bool bFullscreen = false;
int giWindowWidth = 1920;
int giWindowHeight = 1080;

#define STEPS 20

const GLfloat orange[] = { 1.0f, 153.0f/256.0f, 51.0f/256.0f };
const GLfloat  green[] = { 19.0f/256.0f, 136.0f/256.0f, 8.0f/256.0f };
const GLfloat  white[] = { 1.0f, 1.0f, 1.0f };
const GLfloat  black[] = { 0.0f, 0.0f, 0.0f };

GLfloat i1 = STEPS;
GLfloat n = STEPS;
GLfloat d = STEPS;
GLfloat i2 = STEPS;
GLfloat a = STEPS;

GLfloat planeX = -STEPS;
GLfloat planeY = 15.0f;
GLfloat planeAngle = 80.0f;

bool bADash = false;

//// OPENAL //////

ALCdevice *device = NULL;
ALCcontext *context = NULL;
ALuint buffer;
ALuint source;

//////////////////


// entry-point function
int main(void)
{
	// function prototypes
	void CreateWindow(void);
	void ToggleFullscreen(void);

	void initialize(void);
	void display(void);
	void update(void);
	void resize(int, int);
	void uninitialize(void);

	// variable declarations
	int winWidth = giWindowWidth;
	int winHeight = giWindowHeight;

	bool bDone = false;

	// code
	CreateWindow();
	initialize();

	// game loop
	XEvent event;
	KeySym keysym;

	while (bDone == false)
	{
		while (XPending(gpDisplay))
		{
			XNextEvent(gpDisplay, &event);
			switch (event.type)
			{
				case MapNotify:
					break;

				case KeyPress:
					keysym = XkbKeycodeToKeysym(gpDisplay, event.xkey.keycode, 0, 0);
					switch (keysym)
					{
						case XK_Escape:
							bDone = true;
							break;

						case XK_F:
						case XK_f:
							if (bFullscreen == false)
							{
								ToggleFullscreen();
								bFullscreen = true;
							}
							else
							{
								ToggleFullscreen();
								bFullscreen = false;
							}
							break;
					
						default:
							break;
					}
					break;

				case ButtonPress:
					switch (event.xbutton.button)
					{
						case 1:
							break;

						case 2:
							break;

						case 3:
							break;
						
						default:
							break;
					}
					break;

				case MotionNotify:
					break;

				case ConfigureNotify:
					winWidth = event.xconfigure.width;
					winHeight = event.xconfigure.height;
					resize(winWidth, winHeight);
					break;

				case Expose:
					break;

				case DestroyNotify:
					break;

				case 33:
					bDone = true;
					break;
				
				default:
					break;
			}
		}
		update();
		display();
	}

	uninitialize();
	return(0);
}

void CreateWindow(void)
{
	// function prototypes
	void uninitialize(void);

	// variable declarations
	XSetWindowAttributes winAttribs;
	int defaultScreen;
	int styleMask;

	// ~ pixel format descriptor
	static int frameBufferAttributes[] = {
		GLX_DOUBLEBUFFER, True,
		GLX_RGBA,
		GLX_RED_SIZE, 8,
		GLX_GREEN_SIZE, 8,
		GLX_BLUE_SIZE, 8,
		GLX_ALPHA_SIZE, 8,
		None // here '0' is also possible
	};

	// code
	gpDisplay = XOpenDisplay(NULL);
	if (gpDisplay == NULL)
	{
		printf("ERROR: Unable to open XDisplay.\nExiting..\n");
		uninitialize();
		exit(1);
	}

	defaultScreen = XDefaultScreen(gpDisplay);

	gpXVisualInfo = glXChooseVisual(gpDisplay, defaultScreen, frameBufferAttributes);
	if (gpXVisualInfo == NULL)
	{
		printf("ERROR: Unable to get a Visual.\nExiting..\n");
		uninitialize();
		exit(1);
	}

	winAttribs.border_pixel = 0;
	winAttribs.background_pixmap = 0;
	winAttribs.colormap = XCreateColormap(gpDisplay,
		RootWindow(gpDisplay, gpXVisualInfo->screen),
		gpXVisualInfo->visual,
		AllocNone);
	gColormap = winAttribs.colormap;
	winAttribs.background_pixel = BlackPixel(gpDisplay, gpXVisualInfo->depth);

	winAttribs.event_mask = ExposureMask | VisibilityChangeMask | ButtonPressMask | KeyPressMask | PointerMotionMask | StructureNotifyMask;
	styleMask = CWBorderPixel | CWBackPixel | CWEventMask | CWColormap;
	
	gWindow = XCreateWindow(gpDisplay,
		RootWindow(gpDisplay, gpXVisualInfo->screen),
		0,
		0,
		giWindowWidth,
		giWindowHeight,
		0,
		gpXVisualInfo->depth,
		InputOutput,
		gpXVisualInfo->visual,
		styleMask,
		&winAttribs);
	if (!gWindow)
	{
		printf("ERROR: Failed to create Main Window.\nExiting..\n");
		uninitialize();
		exit(1);
	}
	
	XStoreName(gpDisplay, gWindow, "OpenGL | Dynamic India");

	Atom windowManagerDelete = XInternAtom(gpDisplay, "WM_DELETE_WINDOW", True);
	XSetWMProtocols(gpDisplay, gWindow, &windowManagerDelete, 1);

	XMapWindow(gpDisplay, gWindow);
}

void ToggleFullscreen(void)
{
	// variable declarations
	Atom wm_state;
	Atom fullscreen;
	XEvent xev = {0};

	// code
	wm_state = XInternAtom(gpDisplay, "_NET_WM_STATE", False);
	memset(&xev, 0, sizeof(XEvent));

	xev.type = ClientMessage;
	xev.xclient.window = gWindow;
	xev.xclient.message_type = wm_state;
	xev.xclient.format = 32;
	xev.xclient.data.l[0] = bFullscreen ? 0:1;

	fullscreen = XInternAtom(gpDisplay, "_NET_WM_STATE_FULLSCREEN", False);
	xev.xclient.data.l[1] = fullscreen;

	XSendEvent(gpDisplay,
		RootWindow(gpDisplay, gpXVisualInfo->screen),
		False,
		StructureNotifyMask,
		&xev);
}

void initialize(void)
{
	// function declarations
	void resize(int, int);
	void uninitialize(void);
	void initOpenAL(void);

	// code
	gGLXContext = glXCreateContext(gpDisplay, gpXVisualInfo, NULL, GL_TRUE);
	glXMakeCurrent(gpDisplay, gWindow, gGLXContext);

	// opengl
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// OpenAL
	initOpenAL();


	// warmup resize
	resize(giWindowWidth, giWindowHeight);
}

void resize(int width, int height)
{
	// code
	if (height == 0)
		height = 1;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f, (GLfloat)width/(GLfloat)height, 0.1f, 100.0f);
}

void display(void)
{
	// function declarations
	void I1(void);
	void N(void);
	void D(void);
	void I2(void);
	void A(void);
	void plane(void);

	// code
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// letters
	glLoadIdentity();
	glTranslatef(-i1, 0.0f, -12.0f);
	I1();


	glLoadIdentity();
	glTranslatef(0.0f, n, -12.0f);
	N();


	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -12.0f);
	D();


	glLoadIdentity();
	glTranslatef(0.0f, -i2, -12.0f);
	I2();


	glLoadIdentity();
	glTranslatef(a, 0.0f, -12.0f);
	A();

	glLoadIdentity();
	glTranslatef(planeX, planeY, -12.0f);
	glRotatef(-planeAngle, 0.0f, 0.0, 1.0f);
	plane();

	glLoadIdentity();
	glTranslatef(planeX, 0.0f, -12.0f);
	plane();

	glLoadIdentity();
	glTranslatef(planeX, -planeY, -12.0f);
	glRotatef(planeAngle, 0.0f, 0.0, 1.0f);
	plane();

	glXSwapBuffers(gpDisplay, gWindow);
}

void update(void)
{
	static int state = 0;
	switch (state)
	{
	case 0:
		i1 -= 0.05f;
		if (i1 <= 0.0f)
		{
			i1 = 0.0f;
			state++;
		}
		break;

	case 1:
		a -= 0.05f;
		if (a <= 0.0f)
		{
			a = 0.0f;
			state++;
		}
		break;

	case 2:
		n -= 0.05f;
		if (n <= 0.0f)
		{
			n = 0.0f;
			state++;
		}
		break;

	case 3:
		i2 -= 0.05f;
		if (i2 <= 0.0f)
		{
			i2 = 0.0f;
			state++;
		}
		break;

	case 4:
		d -= 0.1f;
		if (d <= 0.0f)
		{
			d = 0.0f;
			state++;
		}
		break;

	case 5:
		planeX += 0.05f;
		planeY -= 0.05f;
		if (planeY <= 0.0f)
			planeY = 0.0f;

		planeAngle -= 0.25f;
		if (planeAngle <= 0.0f)
			planeAngle = 0.0f;

		if (planeX >= -2.0f)
			state++;
		break;

	case 6:
		planeX += 0.04f;
		if (planeX >= 6.0f)
		{
			bADash = true;
			state++;
		}
		break;

	case 7:
		planeX += 0.04f;

		planeY += 0.05f;
		planeAngle -= 0.25f;

		if (planeX >= 15.0f)
			state++;
		break;

	default:
		break;
	}
}

void uninitialize(void)
{
	void uninitOpenAL();

	// OpenAL
	uninitOpenAL();

	GLXContext currentGLXContext = glXGetCurrentContext();
	if (currentGLXContext != NULL && currentGLXContext == gGLXContext)
	{
		glXMakeCurrent(gpDisplay, 0, 0);
	}

	if (gGLXContext)
	{
		glXDestroyContext(gpDisplay, gGLXContext);
	}

	if (gWindow)
	{
		XDestroyWindow(gpDisplay, gWindow);
	}

	if (gColormap)
	{
		XFreeColormap(gpDisplay, gColormap);
	}

	if (gpXVisualInfo)
	{
		free(gpXVisualInfo);
		gpXVisualInfo = NULL;
	}

	if (gpDisplay)
	{
		XCloseDisplay(gpDisplay);
		gpDisplay = NULL;
	}
}

void I1(void)
{
	glBegin(GL_QUADS);
	{
		glColor3fv(orange);
		glVertex3f(-4.0f, 3.25f, 0.0f);
		glVertex3f(-4.5f, 3.0f, 0.0f);
		
		glColor3fv(green);
		glVertex3f(-4.5f, -3.25f, 0.0f);
		glVertex3f(-4.0f, -3.0f, 0.0f);

	}
	glEnd();
}

void N(void)
{
	glBegin(GL_QUADS);
	{
		/* 1st vertical */
		glColor3fv(orange);
		glVertex3f(-2.5f, 3.25f, 0.0f);
		glVertex3f(-3.0f, 3.0f, 0.0f);
		
		glColor3fv(green);
		glVertex3f(-3.0f, -3.25f, 0.0f);
		glVertex3f(-2.5f, -3.0f, 0.0f);

		/* 2nd vertical */
		glColor3fv(orange);
		glVertex3f(-1.0f, 3.25f, 0.0f);
		glVertex3f(-1.5f, 3.0f, 0.0f);
		
		glColor3fv(green);
		glVertex3f(-1.5f, -3.25f, 0.0f);
		glVertex3f(-1.0f, -3.0f, 0.0f);

		/* slant line */
		glColor3fv(orange);
		glVertex3f(-2.5f, 3.25f, 0.0f);
		glVertex3f(-3.0f, 3.0f, 0.0f);
		
		glColor3fv(green);
		glVertex3f(-1.5f, -3.25f, 0.0f);
		glVertex3f(-1.0f, -3.0f, 0.0f);
	}
	glEnd();
}

void D(void)
{
	GLfloat alpha = 1.0f - (d/STEPS);
	glBegin(GL_QUADS);
	{
		/* 1st vertical */
		glColor3f(orange[0]*alpha, orange[1]*alpha, orange[2]*alpha);
		glVertex3f(0.5f, 3.25f, 0.0f);
		glVertex3f(0.0f, 3.0f, 0.0f);
		
		glColor3f(green[0]*alpha, green[1]*alpha, green[2]*alpha);
		glVertex3f(0.0f, -3.25f, 0.0f);
		glVertex3f(0.5f, -3.0f, 0.0f);

		/* 2nd vertical */
		glColor3f(orange[0]*alpha, orange[1]*alpha, orange[2]*alpha);
		glVertex3f(1.5f, 3.25f, 0.0f);
		glVertex3f(1.0f, 3.0f, 0.0f);
		
		glColor3f(green[0]*alpha, green[1]*alpha, green[2]*alpha);
		glVertex3f(1.0f, -3.25f, 0.0f);
		glVertex3f(1.5f, -3.0f, 0.0f);

		/* 1st horizontal */
		glColor3f(orange[0]*alpha, orange[1]*alpha, orange[2]*alpha);
		glVertex3f(0.0f, 3.25f, 0.0f);
		glVertex3f(-0.5f, 3.0f, 0.0f);

		glVertex3f(1.0f, 3.0f, 0.0f);
		glVertex3f(1.5f, 3.25f, 0.0f);

		/* 2nd horizontal */
		glColor3f(green[0]*alpha, green[1]*alpha, green[2]*alpha);
		glVertex3f(-0.5f, -3.25f, 0.0f);
		glVertex3f(0.0f, -3.0f, 0.0f);

		glVertex3f(1.5f, -3.0f, 0.0f);
		glVertex3f(1.0f, -3.25f, 0.0f);
	}
	glEnd();
}

void I2(void)
{
	glBegin(GL_QUADS);
	{
		glColor3fv(orange);
		glVertex3f(3.0f, 3.25f, 0.0f);
		glVertex3f(2.5f, 3.0f, 0.0f);
		
		glColor3fv(green);
		glVertex3f(2.5f, -3.25f, 0.0f);
		glVertex3f(3.0f, -3.0f, 0.0f);

	}
	glEnd();
}

void A(void)
{
	glBegin(GL_QUADS);
	{
		/* 1st vertical */
		glColor3fv(orange);
		glVertex3f(4.5f, 3.25f, 0.0f);
		glVertex3f(4.0f, 3.0f, 0.0f);
		
		glColor3fv(green);
		glVertex3f(4.0f, -3.25f, 0.0f);
		glVertex3f(4.5f, -3.0f, 0.0f);

		if (bADash)
		{
			/* 2nd horizontal */
			glColor3fv(orange);
			glVertex3f(4.0f, 0.1f, 0.0f);
			glVertex3f(3.8f, -0.0f, 0.0f);
			glVertex3f(5.5f, -0.0f, 0.0f);
			glVertex3f(5.5f, 0.1f, 0.0f);

			glColor3fv(white);
			glVertex3f(3.8f, -0.0f, 0.0f);
			glVertex3f(3.6f, -0.1f, 0.0f);
			glVertex3f(5.5f, -0.1f, 0.0f);
			glVertex3f(5.5f, -0.0f, 0.0f);

			glColor3fv(green);
			glVertex3f(3.6f, -0.1f, 0.0f);
			glVertex3f(3.4f, -0.2f, 0.0f);
			glVertex3f(5.5f, -0.2f, 0.0f);
			glVertex3f(5.5f, -0.1f, 0.0f);
		}

		/* 2nd vertical */
		glColor3fv(orange);
		glVertex3f(5.5f, 3.25f, 0.0f);
		glVertex3f(5.0f, 3.0f, 0.0f);
		
		glColor3fv(green);
		glVertex3f(5.0f, -3.25f, 0.0f);
		glVertex3f(5.5f, -3.0f, 0.0f);

		/* 1st horizontal */
		glColor3fv(orange);
		glVertex3f(4.0f, 3.25f, 0.0f);
		glVertex3f(3.5f, 3.0f, 0.0f);

		glColor3fv(orange);
		glVertex3f(5.0f, 3.0f, 0.0f);
		glVertex3f(5.5f, 3.25f, 0.0f);


	}
	glEnd();
}

void plane(void)
{
	glScalef(0.5f, 0.5f, 1.0f);

	/* Plane Body */
	glBegin(GL_POLYGON);
		glColor3f(186.0f / 256.0f, 226.0f / 256.0f, 238.0f / 256.0f);

		glVertex3f(0.0f, 0.6f,0.0f);
		glVertex3f(-3.0f, 0.6f,0.0f);
		glVertex3f(-3.0f, -0.6f,0.0f);
		glVertex3f(0.0f, -0.6f,0.0f);
		glVertex3f(2.0f, 0.0f,0.0f);
	glEnd();

	glBegin(GL_POLYGON);
		glColor3f(186.0f / 256.0f, 226.0f / 256.0f, 238.0f / 256.0f);

		glVertex3f(-3.0f, 0.5f,0.0f);
		glVertex3f(-3.2f, 0.5f,0.0f);
		glVertex3f(-3.5f, 0.35f,0.0f);
		glVertex3f(-3.5f, -0.35f,0.0f);
		glVertex3f(-3.2f, -0.5f,0.0f);
		glVertex3f(-3.0f, -0.5f,0.0f);

	glEnd();

	glBegin(GL_QUADS);
		
		glColor3fv(orange);
		glVertex3f(-3.5f, 0.35f,0.0f);
		glColor3fv(black);
		glVertex3f(-7.5f, 0.35f,0.0f);
		glVertex3f(-7.5f, 0.15f,0.0f);
		glColor3fv(orange);
		glVertex3f(-3.5f, 0.15f,0.0f);

		glColor3fv(white);
		glVertex3f(-3.5f, 0.15f,0.0f);
		glColor3fv(black);
		glVertex3f(-7.5f, 0.15f,0.0f);
		glVertex3f(-7.5f, -0.15f,0.0f);
		glColor3fv(white);
		glVertex3f(-3.5f, -0.15f,0.0f);

		glColor3fv(green);
		glVertex3f(-3.5f, -0.35f,0.0f);
		glColor3fv(black);
		glVertex3f(-7.5f, -0.35f,0.0f);
		glVertex3f(-7.5f, -0.15f,0.0f);
		glColor3fv(green);
		glVertex3f(-3.5f, -0.15f,0.0f);
	glEnd();

	glBegin(GL_TRIANGLES);
		glColor3f(186.0f / 256.0f, 226.0f / 256.0f, 238.0f / 256.0f);

		glVertex3f(-2.2f, 0.6f,0.0f);
		glVertex3f(-3.5f, 0.6f,0.0f);
		glVertex3f(-3.5f, 1.4f,0.0f);

		glVertex3f(-2.2f, -0.6f,0.0f);
		glVertex3f(-3.5f, -0.6f,0.0f);
		glVertex3f(-3.5f, -1.4f,0.0f);
	glEnd();

	glBegin(GL_TRIANGLES);
		glColor3f(186.0f / 256.0f, 226.0f / 256.0f, 238.0f / 256.0f);

		glVertex3f(-0.1f, 0.6f,0.0f);
		glVertex3f(-2.0f, 0.6f,0.0f);
		glVertex3f(-2.0f, 2.7f,0.0f);

		glVertex3f(-0.1f, -0.6f,0.0f);
		glVertex3f(-2.0f, -0.6f,0.0f);
		glVertex3f(-2.0f, -2.7f,0.0f);
	glEnd();

	glBegin(GL_QUADS);
		glColor3f(186.0f / 256.0f, 226.0f / 256.0f, 238.0f / 256.0f);

		glVertex3f(-1.0f, 2.5f,0.0f);
		glVertex3f(-2.2f, 2.5f,0.0f);
		glVertex3f(-2.2f, 2.7f,0.0f);
		glVertex3f(-1.2f, 2.7f,0.0f);

		glVertex3f(-1.0f, -2.5f,0.0f);
		glVertex3f(-2.2f, -2.5f,0.0f);
		glVertex3f(-2.2f, -2.7f,0.0f);
		glVertex3f(-1.2f, -2.7f,0.0f);

	glEnd();

	glBegin(GL_POLYGON);
		glColor3f(186.0f / 256.0f, 226.0f / 256.0f, 238.0f / 256.0f);

		glVertex3f(0.1f, 0.8f,0.0f);
		glVertex3f(-1.0f, 0.8f,0.0f);
		glVertex3f(-1.0f, 1.2f,0.0f);
		glVertex3f(0.1f, 1.2f,0.0f);
		glVertex3f(0.5f, 1.0f,0.0f);

	glEnd();

	glBegin(GL_POLYGON);
		glColor3f(186.0f / 256.0f, 226.0f / 256.0f, 238.0f / 256.0f);

		glVertex3f(0.1f, -0.8f,0.0f);
		glVertex3f(-1.0f, -0.8f,0.0f);
		glVertex3f(-1.0f, -1.2f,0.0f);
		glVertex3f(0.1f, -1.2f,0.0f);
		glVertex3f(0.5f, -1.0f,0.0f);

	glEnd();


	glLineWidth(4.0f);
	glBegin(GL_LINES);
	glColor3f(45.0f / 256.0f, 88.0f / 256.0f, 102.0f / 256.0f);

	/* I */
	glVertex3f(-1.7f, 0.5f, 0.0f);
	glVertex3f(-1.7f, -0.5f, 0.0f);

	/* A */
	glVertex3f(-1.3f, 0.5f, 0.0f);
	glVertex3f(-1.5f, -0.5f, 0.0f);

	glVertex3f(-1.3f, 0.5f, 0.0f);
	glVertex3f(-1.1f, -0.5f, 0.0f);

	glVertex3f(-1.2f, -0.2f, 0.0f);
	glVertex3f(-1.4f, -0.2f, 0.0f);	

	/* F */
	glVertex3f(-0.9f, 0.5f, 0.0f);
	glVertex3f(-0.9f, -0.5f, 0.0f);

	glVertex3f(-0.9f, 0.5f, 0.0f);
	glVertex3f(-0.7f, 0.5f, 0.0f);

	glVertex3f(-0.9f, 0.0f, 0.0f);
	glVertex3f(-0.7f, 0.0f, 0.0f);

	glEnd();
}

void initOpenAL(void)
{
	// select the preferred device
	device = alcOpenDevice(NULL);

	if (device)
	{
		printf("\nDevice Created!!\n");
		context = alcCreateContext(device, NULL);
		alcMakeContextCurrent(context);
		printf("\nContext set!!\n");

		alGetError(); // clear the error

		alGenBuffers(1, &buffer);
		alGenSources(1, &source);

		int channel, sampleRate, bps, size;
		unsigned int format;

    	char* data = loadWav("AbTumhareHawale.wav", &channel, &sampleRate, &bps, &size);
		if (channel == 1) 
		{
			if (bps == 8) {
				format = AL_FORMAT_MONO8;
			} else {
				format = AL_FORMAT_MONO16;
			}
		}
		else 
		{
			if (bps == 8) {
				format = AL_FORMAT_STEREO8;
			} else {
				format = AL_FORMAT_STEREO16;
			}
		}

		alBufferData(buffer, format, data, size, sampleRate);
		alSourcei(source, AL_BUFFER, buffer);
		alSourcePlay(source);

		delete[] data;
	}
}

void uninitOpenAL(void)
{
	alDeleteSources(1, &source);
	alDeleteBuffers(1, &buffer);

	context = alcGetCurrentContext();
	device = alcGetContextsDevice(context);
	alcMakeContextCurrent(NULL);
	alcDestroyContext(context);
	alcCloseDevice(device);    
}
