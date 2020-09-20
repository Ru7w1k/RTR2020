// headers
#include <windows.h>
#include <stdio.h>
#include <gl/GL.h>
#include <gl/GLU.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include "OGLTemplate.h"

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

// macros
#define WIN_WIDTH  800
#define WIN_HEIGHT 600

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


// WinMain()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	// function declarations
	void initialize(void);
	void display(void);

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
		TEXT("OpenGL | Animation of Deathly Hallows"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		(width / 2) - 400,
		(height / 2) - 300,
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
	bool CloakOfInvisibility(GLfloat);
	bool ResurrectionStone(GLfloat);
	bool ElderWand(GLfloat);

	// variables
	static int state = 0;
	static GLfloat angle = 0.0f;

	// code
	glClear(GL_COLOR_BUFFER_BIT);

	switch (state)
	{
		case 0:
			if(CloakOfInvisibility(angle)) state++;
			break;

		case 1:
			CloakOfInvisibility(0.0f);
			if(ResurrectionStone(angle)) state++;
			break;

		case 2:
			CloakOfInvisibility(0.0f);
			ResurrectionStone(0.0f);
			if(ElderWand(angle)) state++;
			break;

		case 3:
			CloakOfInvisibility(0.0f);
			ResurrectionStone(0.0f);
			ElderWand(0.0f);
			break;
	}

	angle += 1.0f;
	if (angle >= 360.0f)
		angle = 0.0f;

	SwapBuffers(ghdc);
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

bool CloakOfInvisibility(GLfloat fAngle)
{	
	static GLfloat xOffset = -3.50f;
	static GLfloat yOffset = -1.60f;
	GLfloat fLength = 0.75f;

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(xOffset, yOffset, -6.0f);
	glRotatef(fAngle, 0.0f, 1.0f, 0.0f);

	glLineWidth(2.0f);
	glBegin(GL_LINE_LOOP);
	glVertex3f(0.0f , fLength , 0.0f);
	glVertex3f(-fLength , -fLength, 0.0f);
	glVertex3f(fLength , -fLength, 0.0f);
	glEnd();

	if (xOffset >= 0.0)
		return true;
	else
	{
		xOffset += (3.50f / (360.0f));
		yOffset += (1.60f / (360.0f));
	}

	return false;
}

bool ResurrectionStone(GLfloat gAngle)
{
	static GLfloat xOffset = 3.50f;
	static GLfloat yOffset = -1.60f;

	GLfloat fLength = 0.75f;
	GLfloat fRadius = 1.0f;
	GLfloat fAngle = 0.0f;
	float s, a, b, c;

	/* Radius Of Incircle */
	a = sqrt(pow((-fLength - 0.0), 2.0) + pow(-fLength - fLength, 2.0));
	b = sqrt(pow((fLength - (-fLength)), 2.0) + pow(-fLength - (-fLength), 2.0));
	c = sqrt(pow((fLength - 0.0), 2.0) + pow(-fLength - fLength, 2.0));
	s = (a + b + c) / 2.0;
	fRadius = sqrt(s * (s - a) * (s - b) * (s - c)) / s;

	/* Move MATRIX to the center of Incircle */
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(xOffset, yOffset - fLength + fRadius, -6.0f);
	glRotatef(gAngle, 0.0f, 1.0f, 0.0f);

	/*  Resurrection Stone */
	glBegin(GL_LINE_LOOP);
	for (fAngle = 0.0f; fAngle < 2 * M_PI; fAngle += 0.01f)
	{
		glVertex3f(fRadius * cos(fAngle), fRadius * sin(fAngle), 0.0f);
	}
	glEnd();

	if (xOffset <= 0.0)
		return true;
	else
	{
		xOffset -= (3.50f / (360.0f));
		yOffset += (1.60f / (360.0f));
	}

	return false;
}

bool ElderWand(GLfloat fAngle)
{
	static GLfloat yOffset = 2.80f;
	GLfloat fLength = 0.75f;

	/* Elder Wand */
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0.0f, yOffset, -6.0f);
	glRotatef(fAngle, 0.0f, 1.0f, 0.0f);

	glBegin(GL_LINES);
	{
		glVertex3f(0.0f, fLength, 0.0f);
		glVertex3f(0.0f, -fLength, 0.0f);
	}
	glEnd();

	if (yOffset <= 0.0)
		return true;
	else
	{
		yOffset -= (1.80f / (300.0f));
	}

	return false;
}
