// headers
#include <windows.h>
#include <stdio.h>
#include <gl/GL.h>
#include <gl/GLU.h>

#include "OGLTemplate.h"

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "winmm.lib") 

// macros
#define WIN_WIDTH  1280
#define WIN_HEIGHT 720

#define STEPS 20

// global function declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// global variables
HDC   ghdc = NULL;
HGLRC ghrc = NULL;

bool gbFullscreen   = false;
bool gbActiveWindow = false;

HWND  ghwnd  = NULL;
FILE* gpFile = NULL;

DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };

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


// WinMain()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	// function declarations
	void initialize(void);
	void display(void);
	void update(void);

	// variable declarations
	bool bDone = false;
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("MyApp");

	// code
	// open file for logging
	if (fopen_s(&gpFile, "RMCLog.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("Cannot open RMCLog.txt file.."), TEXT("Error"), MB_OK | MB_ICONERROR);
		exit(0);
	}
	fprintf(gpFile, "==== Application Started ====\n");

	// initialization of WNDCLASSEX
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(RMC_ICON));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(RMC_ICON));

	// register above class
	RegisterClassEx(&wndclass);

	// get the screen size
	int width = GetSystemMetrics(SM_CXSCREEN);
	int height = GetSystemMetrics(SM_CYSCREEN);

	// create window
	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szAppName,
		TEXT("OpenGL | Dynamic India"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		(width / 2) - (WIN_WIDTH/2),
		(height / 2) - (WIN_HEIGHT/2),
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ghwnd = hwnd;

	initialize();

	ShowWindow(hwnd, iCmdShow);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	// Game Loop!
	while (bDone == false)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				bDone = true;
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if (gbActiveWindow == true)
			{
				// call update() here for OpenGL rendering
				update();
				// call display() here for OpenGL rendering
				display();
			}
		}
	}

	return((int)msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	// function declaration
	void display(void);
	void resize(int, int);
	void uninitialize();
	void ToggleFullscreen(void);

	// code
	switch (iMsg)
	{

	case WM_SETFOCUS:
		gbActiveWindow = true;
		break;

	case WM_KILLFOCUS:
		gbActiveWindow = false;
		break;

	case WM_SIZE:
		resize(LOWORD(lParam), HIWORD(lParam));
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hwnd);
			break;

		case 0x46:
		case 0x66:
			ToggleFullscreen();
			break;

		default:
			break;
		}
		break;

	case WM_ERASEBKGND:
		return(0);

	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;

	case WM_DESTROY:
		uninitialize();
		PostQuitMessage(0);
		break;

	default:
		break;
	}

	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}

void ToggleFullscreen(void)
{
	// local variables
	MONITORINFO mi = { sizeof(MONITORINFO) };

	// code
	if (gbFullscreen == false)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			if (GetWindowPlacement(ghwnd, &wpPrev) &&
				GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd, HWND_TOP,
					mi.rcMonitor.left,
					mi.rcMonitor.top,
					mi.rcMonitor.right - mi.rcMonitor.left,
					mi.rcMonitor.bottom - mi.rcMonitor.top,
					SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
		gbFullscreen = true;
	}
	else
	{
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP,
			0, 0, 0, 0,
			SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER);
		ShowCursor(TRUE);
		gbFullscreen = false;
	}
}


void initialize(void)
{
	// function declarations
	void resize(int, int);

	// variable declarations
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;

	// code
	ghdc = GetDC(ghwnd);

	ZeroMemory((void *)&pfd, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL| PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;

	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if (iPixelFormatIndex == 0)
	{
		fprintf(gpFile, "ChoosePixelFormat() failed..\n");
		DestroyWindow(ghwnd);
	}

	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
	{
		fprintf(gpFile, "SetPixelFormat() failed..\n");
		DestroyWindow(ghwnd);
	}

	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL)
	{
		fprintf(gpFile, "wglCreateContext() failed..\n");
		DestroyWindow(ghwnd);
	}

	if (wglMakeCurrent(ghdc, ghrc) == FALSE)
	{
		fprintf(gpFile, "wglMakeCurrent() failed..\n");
		DestroyWindow(ghwnd);
	}

	// set clear color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// play backgroud theme song
	PlaySound(MAKEINTRESOURCE(RMC_AUDIO), // ID of WAVE resource
		GetModuleHandle(NULL), 			  // handle of this module, which contains the resource
		SND_RESOURCE | SND_ASYNC);		  // ID is of type resource | play async (i.e. non-blocking)

	// warm-up resize call
	resize(WIN_WIDTH, WIN_HEIGHT);
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

	SwapBuffers(ghdc);
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
	// code
	if (gbFullscreen == true)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd,
			HWND_TOP,
			0,
			0,
			0,
			0,
			SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER);

		ShowCursor(TRUE);
	}

	if (wglGetCurrentContext() == ghrc)
	{
		wglMakeCurrent(NULL, NULL);
	}

	if (ghrc)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
	}

	if (ghdc)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	if (gpFile)
	{
		fprintf(gpFile, "==== Application Terminated ====\n");
		fclose(gpFile);
		gpFile = NULL;
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

