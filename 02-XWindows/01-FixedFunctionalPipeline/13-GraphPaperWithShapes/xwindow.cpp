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

#define _USE_MATH_DEFINES
#include <math.h>

// namespace
using namespace std;

// global variable declarations
Display *gpDisplay = NULL;
Window gWindow;
XVisualInfo *gpXVisualInfo = NULL;
Colormap gColormap;

GLXContext gGLXContext;

bool bFullscreen = false;
int giWindowWidth = 800;
int giWindowHeight = 600;

// entry-point function
int main(void)
{
	// function prototypes
	void CreateWindow(void);
	void ToggleFullscreen(void);

	void initialize(void);
	void display(void);
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
	
	XStoreName(gpDisplay, gWindow, "OpenGL | Graphpaper with Shapes");

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

	// code
	gGLXContext = glXCreateContext(gpDisplay, gpXVisualInfo, NULL, GL_TRUE);
	glXMakeCurrent(gpDisplay, gWindow, gGLXContext);

	// opengl
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

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
	// code
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -3.0f);
	
	glLineWidth(1.0f);
	glBegin(GL_LINES);

		// horizontal lines
		glColor3f(0.0f, 0.0f, 1.0f);
		for (float fOffset = -1.0f; fOffset <= 0; fOffset += (1.0f / 20.0f))
		{
			glVertex3f( -1.0f, fOffset, 0.0f);
			glVertex3f(  1.0f, fOffset, 0.0f);

			glVertex3f( -1.0f, fOffset + 1.0f+(1.0f / 20.0f), 0.0f);
			glVertex3f(  1.0f, fOffset + 1.0f+(1.0f / 20.0f), 0.0f);
		}

		// vertical lines
		glColor3f(0.0f, 0.0f, 1.0f);
		for (float fOffset = -1.0; fOffset <= 0; fOffset += (1.0 / 20.0f))
		{
			glVertex3f(fOffset, -1.0f, 0.0f);
			glVertex3f(fOffset,  1.0f, 0.0f);

			glVertex3f(fOffset + 1.0+(1.0 / 20.0f), -1.0f, 0.0f);
			glVertex3f(fOffset + 1.0+(1.0 / 20.0f),  1.0f, 0.0f);
		}
	glEnd();

	glLineWidth(2.0f);
	glBegin(GL_LINES);

		// X-axis
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(-1.0f, 0.0f, 0.0f);
		glVertex3f( 1.0f, 0.0f, 0.0f);

		// Y-axis
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(0.0f, -1.0f, 0.0f);
		glVertex3f(0.0f, 1.0f, 0.0f);

	glEnd();

	GLfloat fLength = 0.5f;
	GLfloat fRadius = 1.0f;
	GLfloat fAngle = 0.0f;
	
	/////////// outer circle //////////////////////////////////
	fRadius = sqrt(pow(fLength-0.0f, 2.0) + pow(fLength-0.0f, 2.0));
	
	glBegin(GL_LINE_LOOP);
		glColor3f(1.0f, 1.0f, 0.0f);
		for (fAngle = 0.0f; fAngle < 2 * M_PI; fAngle += 0.01f)
		{
			glVertex3f(fRadius * cosf(fAngle), fRadius * sinf(fAngle), 0.0f);
		}
	glEnd();

	/////////// quad //////////////////////////////////////////
	glBegin(GL_LINE_LOOP);
	{
		// triangle
		glLineWidth(3.0f);
		glColor3f(1.0f, 1.0f, 0.0f);

		glVertex3f(fLength, fLength, 0.0f);
		glVertex3f(-fLength, fLength, 0.0f);
		glVertex3f(-fLength, -fLength, 0.0f);
		glVertex3f(fLength, -fLength, 0.0f);

	}
	glEnd();

	/////////// triangle //////////////////////////////////////
	glBegin(GL_LINE_LOOP);
	{
		// triangle
		glLineWidth(3.0f);
		glColor3f(1.0f, 1.0f, 0.0f);

		glVertex3f(0.0f, fLength, 0.0f);
		glVertex3f(-fLength, -fLength, 0.0f);
		glVertex3f(fLength, -fLength, 0.0f);

	}
	glEnd();

	/////////// incircle //////////////////////////////////////
	float s, a, b, c;

	/* Radius Of Incircle */
	a = sqrt(pow((fLength - (-fLength)), 2.0) + pow(-fLength - (-fLength), 2.0));
	b = sqrt(pow((fLength - 0.0), 2.0) + pow(-fLength - fLength, 2.0));
	c = sqrt(pow((-fLength - 0.0), 2.0) + pow(-fLength - fLength, 2.0));
	s = (a + b + c) / 2.0;
	fRadius = sqrt(s * (s - a) * (s - b) * (s - c)) / s;

	GLfloat axbxcx = (a*0.0f) + (-fLength*b) + (fLength*c);
	GLfloat aybycy = (a*fLength) + (-fLength*b) + (-fLength*c);
	GLfloat abc = a + b + c;

	/* Move MATRIX to the center of Incircle */
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(axbxcx/abc, aybycy/abc, -3.0f);
	glBegin(GL_LINE_LOOP);
	for (fAngle = 0.0f; fAngle < 2 * M_PI; fAngle += 0.01f)
	{
		glVertex3f(fRadius * cosf(fAngle), fRadius * sinf(fAngle), 0.0f);
	}
	glEnd();

	glXSwapBuffers(gpDisplay, gWindow);
}

void uninitialize(void)
{
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
