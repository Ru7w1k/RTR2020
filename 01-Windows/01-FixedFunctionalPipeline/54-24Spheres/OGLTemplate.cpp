// headers
#include <windows.h>
#include <stdio.h>
#include <gl/GL.h>
#include <gl/GLU.h>

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

bool bLight = false;

GLfloat lightAmbient[]  = { 0.1f, 0.1f, 0.1f, 1.0f };
GLfloat lightDiffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightPosition[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

GLfloat materialAmbient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat materialDiffuse[]  = { 0.5f, 0.2f, 0.7f, 1.0f };
GLfloat materialSpecular[] = { 0.7f, 0.7f, 0.7f, 1.0f };
GLfloat materialShininess = 128.0f;

GLfloat light_model_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
GLfloat light_model_local_viewer[] = { 0.0f };

GLUquadric *quadric[24];
GLfloat angleOfXRotation = 0.0f;
GLfloat angleOfYRotation = 0.0f;
GLfloat angleOfZRotation = 0.0f;

GLuint keyPressed = 0;


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
		TEXT("OpenGL | 24 Spheres"),
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

	case WM_CHAR:
		switch(wParam)
		{
			case 'X':
			case 'x':
				keyPressed = 1;
				angleOfXRotation = 0.0f;
				break;

			case 'Y':
			case 'y':
				keyPressed = 2;
				angleOfYRotation = 0.0f;
				break;

			case 'Z':
			case 'z':
				keyPressed = 3;
				angleOfZRotation = 0.0f;
				break;

			case 'L':
			case 'l':
				bLight = !bLight;
				if (bLight)
				{
					glEnable(GL_LIGHTING);
				}
				else
				{
					glDisable(GL_LIGHTING);
				}
			break;
		}
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
	pfd.cDepthBits = 32;

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

	// depth
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glShadeModel(GL_SMOOTH);

	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, light_model_ambient);
	glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, light_model_local_viewer);

	// light
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glEnable(GL_LIGHT0);

	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	glMaterialf(GL_FRONT, GL_SHININESS, materialShininess);

	for (int i = 0; i < 24; i++)
	{
		quadric[i] = gluNewQuadric();
	}

	// set clear color
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

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
	
	if (width <= height)
	{
		glOrtho(0.0,
			15.5,
			(0.0 * ((GLdouble)height / (GLdouble)width)),
			(15.5 * ((GLdouble)height / (GLdouble)width)),
			-10.0,
			10.0);
	}
	else
	{
		glOrtho((0.0 * ((GLdouble)width / (GLdouble)height)),
			(15.5 * ((GLdouble)width / (GLdouble)height)),
			0.0,
			15.5,
			-10.0,
			10.0);
	}
	
}

void display(void)
{
	// function declarations
	void Draw24Spheres(void);

	// code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (keyPressed == 1)
	{
		glRotatef(angleOfXRotation, 1.0f, 0.0f, 0.0f);
		lightPosition[1] = 100.0f;
	}
	else if (keyPressed == 2)
	{
		glRotatef(angleOfYRotation, 0.0f, 1.0f, 0.0f);
		lightPosition[2] = 100.0f;
	}
	else if (keyPressed == 3)
	{
		glRotatef(angleOfZRotation, 0.0f, 0.0f, 1.0f);
		lightPosition[0] = 100.0f;
	}

	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

	Draw24Spheres();

	SwapBuffers(ghdc);
}

void update(void)
{
	// code
	if (angleOfXRotation < 360)
	{
		angleOfXRotation += 0.75;
	}
	else
	{
		angleOfXRotation = 0.0;
	}

	if (angleOfYRotation < 360)
	{
		angleOfYRotation += 0.75;
	}
	else
	{
		angleOfYRotation = 0.0;
	}

	if (angleOfZRotation < 360)
	{
		angleOfZRotation += 0.75;
	}
	else
	{
		angleOfZRotation = 0.0;
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

	if (quadric)
	{
		for (int i = 0; i < 24; i++)
		{
			gluDeleteQuadric(quadric[i]);
		}
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

void Draw24Spheres(void) 
{
	GLfloat dX = 2.5f;
	GLfloat dY = 2.0f;
	GLfloat MaterialAmbient[4];
	GLfloat MaterialDiffuse[4];
	GLfloat MaterialSpecular[4];
	GLfloat MaterialShininess[1];
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	#pragma region Gems

	////// Emerald /////////////////////////////////////
	// Ambient
	MaterialAmbient[0] = 0.0215f; // r
	MaterialAmbient[1] = 0.1745f; // g
	MaterialAmbient[2] = 0.0215f; // b
	MaterialAmbient[3] = 1.0f;    // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// Diffuse
	MaterialDiffuse[0] = 0.07568f; // r
	MaterialDiffuse[1] = 0.61424f; // g
	MaterialDiffuse[2] = 0.07568f; // b
	MaterialDiffuse[3] = 1.0f;     // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// Specular
	MaterialSpecular[0] = 0.633f;    // r
	MaterialSpecular[1] = 0.727811f; // g
	MaterialSpecular[2] = 0.633f;    // b
	MaterialSpecular[3] = 1.0f;      // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// Shininess
	MaterialShininess[0] = 0.6 * 128.0f;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(dX, dY, 0.0f);
	gluSphere(quadric[0], 1.0f, 30, 30);
	dX += 4.5f;

	////// Jade /////////////////////////////////////
	// Ambient
	MaterialAmbient[0] = 0.135f;  // r
	MaterialAmbient[1] = 0.2225f; // g
	MaterialAmbient[2] = 0.1575f; // b
	MaterialAmbient[3] = 1.0f;    // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// Diffuse
	MaterialDiffuse[0] = 0.54f; // r
	MaterialDiffuse[1] = 0.89f; // g
	MaterialDiffuse[2] = 0.63f; // b
	MaterialDiffuse[3] = 1.0f;  // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// Specular
	MaterialSpecular[0] = 0.316228f; // r
	MaterialSpecular[1] = 0.316228f; // g
	MaterialSpecular[2] = 0.316228f; // b
	MaterialSpecular[3] = 1.0f;      // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// Shininess
	MaterialShininess[0] = 0.1 * 128.0f;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(dX, dY, 0.0f);
	gluSphere(quadric[1], 1.0f, 30, 30);
	dX += 4.5f;

	////// Obsidian /////////////////////////////////////
	// Ambient
	MaterialAmbient[0] = 0.05375f; // r
	MaterialAmbient[1] = 0.05f;    // g
	MaterialAmbient[2] = 0.06625f; // b
	MaterialAmbient[3] = 1.0f;     // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// Diffuse
	MaterialDiffuse[0] = 0.18275f; // r
	MaterialDiffuse[1] = 0.17f;    // g
	MaterialDiffuse[2] = 0.22525f; // b
	MaterialDiffuse[3] = 1.0f;     // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// Specular
	MaterialSpecular[0] = 0.332741f; // r
	MaterialSpecular[1] = 0.328634f; // g
	MaterialSpecular[2] = 0.346435f; // b
	MaterialSpecular[3] = 1.0f;      // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// Shininess
	MaterialShininess[0] = 0.3 * 128.0f;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(dX, dY, 0.0f);
	gluSphere(quadric[2], 1.0f, 30, 30);
	dX += 4.5f;

	////// Pearl /////////////////////////////////////
	// Ambient
	MaterialAmbient[0] = 0.25f;    // r
	MaterialAmbient[1] = 0.20725f; // g
	MaterialAmbient[2] = 0.20725f; // b
	MaterialAmbient[3] = 1.0f;     // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// Diffuse
	MaterialDiffuse[0] = 1.0f;   // r
	MaterialDiffuse[1] = 0.829f; // g
	MaterialDiffuse[2] = 0.829f; // b
	MaterialDiffuse[3] = 1.0f;   // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// Specular
	MaterialSpecular[0] = 0.296648f; // r
	MaterialSpecular[1] = 0.296648f; // g
	MaterialSpecular[2] = 0.296648f; // b
	MaterialSpecular[3] = 1.0f;      // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// Shininess
	MaterialShininess[0] = 0.088 * 128.0f;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(dX, dY, 0.0f);
	gluSphere(quadric[3], 1.0f, 30, 30);
	dX += 4.5f;

	////// Ruby /////////////////////////////////////
	// Ambient
	MaterialAmbient[0] = 0.1745f;  // r
	MaterialAmbient[1] = 0.01175f; // g
	MaterialAmbient[2] = 0.01175f; // b
	MaterialAmbient[3] = 1.0f;     // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// Diffuse
	MaterialDiffuse[0] = 0.61424f; // r
	MaterialDiffuse[1] = 0.04136f; // g
	MaterialDiffuse[2] = 0.04136f; // b
	MaterialDiffuse[3] = 1.0f;     // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// Specular
	MaterialSpecular[0] = 0.727811f; // r
	MaterialSpecular[1] = 0.626959f; // g
	MaterialSpecular[2] = 0.626959f; // b
	MaterialSpecular[3] = 1.0f;      // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// Shininess
	MaterialShininess[0] = 0.6 * 128.0f;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(dX, dY, 0.0f);
	gluSphere(quadric[4], 1.0f, 30, 30);
	dX += 4.5f;

	////// Torquoise /////////////////////////////////////
	// Ambient
	MaterialAmbient[0] = 0.1f;     // r
	MaterialAmbient[1] = 0.18725f; // g
	MaterialAmbient[2] = 0.1745f;  // b
	MaterialAmbient[3] = 1.0f;     // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// Diffuse
	MaterialDiffuse[0] = 0.396f;   // r
	MaterialDiffuse[1] = 0.74151f; // g
	MaterialDiffuse[2] = 0.69102f; // b
	MaterialDiffuse[3] = 1.0f;     // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// Specular
	MaterialSpecular[0] = 0.297254f;    // r
	MaterialSpecular[1] = 0.30829f; // g
	MaterialSpecular[2] = 0.306678f;    // b
	MaterialSpecular[3] = 1.0f;     // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// Shininess
	MaterialShininess[0] = 0.1 * 128.0f;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(dX, dY, 0.0f);
	gluSphere(quadric[5], 1.0f, 30, 30);
	dX += 4.5f;

	dX = 2.5f;
	dY += 4.0f;
	#pragma endregion

	#pragma region Metals

	////// Brass /////////////////////////////////////
	// Ambient
	MaterialAmbient[0] = 0.329412f; // r
	MaterialAmbient[1] = 0.223529f; // g
	MaterialAmbient[2] = 0.027451f; // b
	MaterialAmbient[3] = 1.0f;    // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// Diffuse
	MaterialDiffuse[0] = 0.780392f; // r
	MaterialDiffuse[1] = 0.568627f; // g
	MaterialDiffuse[2] = 0.113725f; // b
	MaterialDiffuse[3] = 1.0f;     // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// Specular
	MaterialSpecular[0] = 0.992157f;    // r
	MaterialSpecular[1] = 0.941176f; // g
	MaterialSpecular[2] = 0.807843f;    // b
	MaterialSpecular[3] = 1.0f;      // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// Shininess
	MaterialShininess[0] = 0.21794872 * 128.0f;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(dX, dY, 0.0f);
	gluSphere(quadric[6], 1.0f, 30, 30);
	dX += 4.5f;

	////// Bronze /////////////////////////////////////
	// Ambient
	MaterialAmbient[0] = 0.2125f;  // r
	MaterialAmbient[1] = 0.1275f; // g
	MaterialAmbient[2] = 0.054f; // b
	MaterialAmbient[3] = 1.0f;    // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// Diffuse
	MaterialDiffuse[0] = 0.714f; // r
	MaterialDiffuse[1] = 0.4284f; // g
	MaterialDiffuse[2] = 0.18144f; // b
	MaterialDiffuse[3] = 1.0f;  // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// Specular
	MaterialSpecular[0] = 0.393548f; // r
	MaterialSpecular[1] = 0.271906f; // g
	MaterialSpecular[2] = 0.166721f; // b
	MaterialSpecular[3] = 1.0f;      // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// Shininess
	MaterialShininess[0] = 0.2 * 128.0f;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(dX, dY, 0.0f);
	gluSphere(quadric[7], 1.0f, 30, 30);
	dX += 4.5f;

	////// Chrome /////////////////////////////////////
	// Ambient
	MaterialAmbient[0] = 0.25; // r
	MaterialAmbient[1] = 0.25; // g
	MaterialAmbient[2] = 0.25; // b
	MaterialAmbient[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// Diffuse
	MaterialDiffuse[0] = 0.4;  // r
	MaterialDiffuse[1] = 0.4;  // g
	MaterialDiffuse[2] = 0.4;  // b
	MaterialDiffuse[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// Specular
	MaterialSpecular[0] = 0.774597f; // r
	MaterialSpecular[1] = 0.774597f; // g
	MaterialSpecular[2] = 0.774597f; // b
	MaterialSpecular[3] = 1.0f;      // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// Shininess
	MaterialShininess[0] = 0.6 * 128.0f;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(dX, dY, 0.0f);
	gluSphere(quadric[8], 1.0f, 30, 30);
	dX += 4.5f;

	////// Copper /////////////////////////////////////
	// Ambient
	MaterialAmbient[0] = 0.19125f;    // r
	MaterialAmbient[1] = 0.0735f; // g
	MaterialAmbient[2] = 0.0225f; // b
	MaterialAmbient[3] = 1.0f;     // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// Diffuse
	MaterialDiffuse[0] = 1.7038f;   // r
	MaterialDiffuse[1] = 0.27048f; // g
	MaterialDiffuse[2] = 0.0828f; // b
	MaterialDiffuse[3] = 1.0f;   // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// Specular
	MaterialSpecular[0] = 0.256777f; // r
	MaterialSpecular[1] = 0.137622f; // g
	MaterialSpecular[2] = 0.086014f; // b
	MaterialSpecular[3] = 1.0f;      // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// Shininess
	MaterialShininess[0] = 0.1 * 128.0f;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(dX, dY, 0.0f);
	gluSphere(quadric[9], 1.0f, 30, 30);
	dX += 4.5f;

	////// Gold /////////////////////////////////////
	// Ambient
	MaterialAmbient[0] = 0.24725f; // r
	MaterialAmbient[1] = 0.1995f;  // g
	MaterialAmbient[2] = 0.0745f;  // b
	MaterialAmbient[3] = 1.0f;     // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// Diffuse
	MaterialDiffuse[0] = 0.75164f; // r
	MaterialDiffuse[1] = 0.60648f; // g
	MaterialDiffuse[2] = 0.22648f; // b
	MaterialDiffuse[3] = 1.0f;     // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// Specular
	MaterialSpecular[0] = 0.628281f; // r
	MaterialSpecular[1] = 0.555802f; // g
	MaterialSpecular[2] = 0.366065f; // b
	MaterialSpecular[3] = 1.0f;      // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// Shininess
	MaterialShininess[0] = 0.4 * 128.0f;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(dX, dY, 0.0f);
	gluSphere(quadric[10], 1.0f, 30, 30);
	dX += 4.5f;

	////// Silver /////////////////////////////////////
	// Ambient
	MaterialAmbient[0] = 0.19225f; // r
	MaterialAmbient[1] = 0.19225f; // g
	MaterialAmbient[2] = 0.19225f; // b
	MaterialAmbient[3] = 1.0f;    // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// Diffuse
	MaterialDiffuse[0] = 0.50754f; // r
	MaterialDiffuse[1] = 0.50754f; // g
	MaterialDiffuse[2] = 0.50754f; // b
	MaterialDiffuse[3] = 1.0f;    // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// Specular
	MaterialSpecular[0] = 0.508273f; // r
	MaterialSpecular[1] = 0.508273f; // g
	MaterialSpecular[2] = 0.508273f; // b
	MaterialSpecular[3] = 1.0f;      // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// Shininess
	MaterialShininess[0] = 0.4 * 128.0f;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(dX, dY, 0.0f);
	gluSphere(quadric[11], 1.0f, 30, 30);
	dX += 4.5f;

	dX = 2.5f;
	dY += 4.0f;
	#pragma endregion

	#pragma region Plastic

	////// Black /////////////////////////////////////
	// Ambient
	MaterialAmbient[0] = 0.0f; // r
	MaterialAmbient[1] = 0.0f; // g
	MaterialAmbient[2] = 0.0f; // b
	MaterialAmbient[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// Diffuse
	MaterialDiffuse[0] = 0.01f; // r
	MaterialDiffuse[1] = 0.01f; // g
	MaterialDiffuse[2] = 0.01f; // b
	MaterialDiffuse[3] = 1.0f;  // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// Specular
	MaterialSpecular[0] = 0.50f; // r
	MaterialSpecular[1] = 0.50f; // g
	MaterialSpecular[2] = 0.50f; // b
	MaterialSpecular[3] = 1.0f;  // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// Shininess
	MaterialShininess[0] = 0.25 * 128.0f;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(dX, dY, 0.0f);
	gluSphere(quadric[12], 1.0f, 30, 30);
	dX += 4.5f;

	////// Cyan /////////////////////////////////////
	// Ambient
	MaterialAmbient[0] = 0.0f;  // r
	MaterialAmbient[1] = 0.1f;  // g
	MaterialAmbient[2] = 0.06f; // b
	MaterialAmbient[3] = 1.0f;  // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// Diffuse
	MaterialDiffuse[0] = 0.0f; // r
	MaterialDiffuse[1] = 0.50980392f; // g
	MaterialDiffuse[2] = 0.50980392f; // b
	MaterialDiffuse[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// Specular
	MaterialSpecular[0] = 0.50196078f; // r
	MaterialSpecular[1] = 0.50196078f; // g
	MaterialSpecular[2] = 0.50196078f; // b
	MaterialSpecular[3] = 1.0f;      // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// Shininess
	MaterialShininess[0] = 0.25 * 128.0f;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(dX, dY, 0.0f);
	gluSphere(quadric[13], 1.0f, 30, 30);
	dX += 4.5f;

	////// Green /////////////////////////////////////
	// Ambient
	MaterialAmbient[0] = 0.0f; // r
	MaterialAmbient[1] = 0.0f; // g
	MaterialAmbient[2] = 0.0f; // b
	MaterialAmbient[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// Diffuse
	MaterialDiffuse[0] = 0.1;  // r
	MaterialDiffuse[1] = 0.35;  // g
	MaterialDiffuse[2] = 0.1;  // b
	MaterialDiffuse[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// Specular
	MaterialSpecular[0] = 0.45f; // r
	MaterialSpecular[1] = 0.55f; // g
	MaterialSpecular[2] = 0.45f; // b
	MaterialSpecular[3] = 1.0f;      // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// Shininess
	MaterialShininess[0] = 0.25 * 128.0f;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(dX, dY, 0.0f);
	gluSphere(quadric[14], 1.0f, 30, 30);
	dX += 4.5f;

	////// Red /////////////////////////////////////
	// Ambient
	MaterialAmbient[0] = 0.0f; // r
	MaterialAmbient[1] = 0.0f; // g
	MaterialAmbient[2] = 0.0f; // b
	MaterialAmbient[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// Diffuse
	MaterialDiffuse[0] = 0.5f; // r
	MaterialDiffuse[1] = 0.0f; // g
	MaterialDiffuse[2] = 0.0f; // b
	MaterialDiffuse[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// Specular
	MaterialSpecular[0] = 0.7f; // r
	MaterialSpecular[1] = 0.6f; // g
	MaterialSpecular[2] = 0.6f; // b
	MaterialSpecular[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// Shininess
	MaterialShininess[0] = 0.25 * 128.0f;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(dX, dY, 0.0f);
	gluSphere(quadric[15], 1.0f, 30, 30);
	dX += 4.5f;

	////// White /////////////////////////////////////
	// Ambient
	MaterialAmbient[0] = 0.0f; // r
	MaterialAmbient[1] = 0.0f; // g
	MaterialAmbient[2] = 0.0f; // b
	MaterialAmbient[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// Diffuse
	MaterialDiffuse[0] = 0.55f; // r
	MaterialDiffuse[1] = 0.55f; // g
	MaterialDiffuse[2] = 0.55f; // b
	MaterialDiffuse[3] = 1.0f;  // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// Specular
	MaterialSpecular[0] = 0.70f; // r
	MaterialSpecular[1] = 0.70f; // g
	MaterialSpecular[2] = 0.70f; // b
	MaterialSpecular[3] = 1.0f;  // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// Shininess
	MaterialShininess[0] = 0.25 * 128.0f;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(dX, dY, 0.0f);
	gluSphere(quadric[16], 1.0f, 30, 30);
	dX += 4.5f;

	////// Yellow /////////////////////////////////////
	// Ambient
	MaterialAmbient[0] = 0.0f; // r
	MaterialAmbient[1] = 0.0f; // g
	MaterialAmbient[2] = 0.0f; // b
	MaterialAmbient[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// Diffuse
	MaterialDiffuse[0] = 0.5f; // r
	MaterialDiffuse[1] = 0.50f; // g
	MaterialDiffuse[2] = 0.0f; // b
	MaterialDiffuse[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// Specular
	MaterialSpecular[0] = 0.60f; // r
	MaterialSpecular[1] = 0.60f; // g
	MaterialSpecular[2] = 0.50f; // b
	MaterialSpecular[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// Shininess
	MaterialShininess[0] = 0.25 * 128.0f;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(dX, dY, 0.0f);
	gluSphere(quadric[16], 1.0f, 30, 30);
	dX += 4.5f;

	dX = 2.5f;
	dY += 4.0f;
	#pragma endregion

	#pragma region Rubber

	////// Black /////////////////////////////////////
	// Ambient
	MaterialAmbient[0] = 0.02f; // r
	MaterialAmbient[1] = 0.02f; // g
	MaterialAmbient[2] = 0.02f; // b
	MaterialAmbient[3] = 1.0f;  // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// Diffuse
	MaterialDiffuse[0] = 0.01f; // r
	MaterialDiffuse[1] = 0.01f; // g
	MaterialDiffuse[2] = 0.01f; // b
	MaterialDiffuse[3] = 1.0f;  // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// Specular
	MaterialSpecular[0] = 0.4f; // r
	MaterialSpecular[1] = 0.4f; // g
	MaterialSpecular[2] = 0.4f; // b
	MaterialSpecular[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// Shininess
	MaterialShininess[0] = 0.0878125 * 128.0f;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(dX, dY, 0.0f);
	gluSphere(quadric[17], 1.0f, 30, 30);
	dX += 4.5f;

	////// Cyan /////////////////////////////////////
	// Ambient
	MaterialAmbient[0] = 0.0f ; // r
	MaterialAmbient[1] = 0.05f; // g
	MaterialAmbient[2] = 0.05f; // b
	MaterialAmbient[3] = 1.0f;  // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// Diffuse
	MaterialDiffuse[0] = 0.4f; // r
	MaterialDiffuse[1] = 0.5f; // g
	MaterialDiffuse[2] = 0.5f; // b
	MaterialDiffuse[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// Specular
	MaterialSpecular[0] = 0.04f; // r
	MaterialSpecular[1] = 0.7f;  // g
	MaterialSpecular[2] = 0.7f;  // b
	MaterialSpecular[3] = 1.0f;  // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// Shininess
	MaterialShininess[0] = 0.25 * 128.0f;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(dX, dY, 0.0f);
	gluSphere(quadric[18], 1.0f, 30, 30);
	dX += 4.5f;

	////// Green /////////////////////////////////////
	// Ambient
	MaterialAmbient[0] = 0.0f; // r
	MaterialAmbient[1] = 0.05f; // g
	MaterialAmbient[2] = 0.0f; // b
	MaterialAmbient[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// Diffuse
	MaterialDiffuse[0] = 0.4;  // r
	MaterialDiffuse[1] = 0.5;  // g
	MaterialDiffuse[2] = 0.4;  // b
	MaterialDiffuse[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// Specular
	MaterialSpecular[0] = 0.04f; // r
	MaterialSpecular[1] = 0.7f; // g
	MaterialSpecular[2] = 0.04f; // b
	MaterialSpecular[3] = 1.0f;      // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// Shininess
	MaterialShininess[0] = 0.078125 * 128.0f;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(dX, dY, 0.0f);
	gluSphere(quadric[19], 1.0f, 30, 30);
	dX += 4.5f;

	////// Red /////////////////////////////////////
	// Ambient
	MaterialAmbient[0] = 0.05f; // r
	MaterialAmbient[1] = 0.0f; // g
	MaterialAmbient[2] = 0.0f; // b
	MaterialAmbient[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// Diffuse
	MaterialDiffuse[0] = 0.5f; // r
	MaterialDiffuse[1] = 0.4f; // g
	MaterialDiffuse[2] = 0.4f; // b
	MaterialDiffuse[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// Specular
	MaterialSpecular[0] = 0.7f; // r
	MaterialSpecular[1] = 0.04f; // g
	MaterialSpecular[2] = 0.04f; // b
	MaterialSpecular[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// Shininess
	MaterialShininess[0] = 0.078125 * 128.0f;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(dX, dY, 0.0f);
	gluSphere(quadric[20], 1.0f, 30, 30);
	dX += 4.5f;

	////// White /////////////////////////////////////
	// Ambient
	MaterialAmbient[0] = 0.05f; // r
	MaterialAmbient[1] = 0.05f; // g
	MaterialAmbient[2] = 0.05f; // b
	MaterialAmbient[3] = 1.0f;  // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// Diffuse
	MaterialDiffuse[0] = 0.5f; // r
	MaterialDiffuse[1] = 0.5f; // g
	MaterialDiffuse[2] = 0.5f; // b
	MaterialDiffuse[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// Specular
	MaterialSpecular[0] = 0.7f; // r
	MaterialSpecular[1] = 0.7f; // g
	MaterialSpecular[2] = 0.7f; // b
	MaterialSpecular[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// Shininess
	MaterialShininess[0] = 0.078125 * 128.0f;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(dX, dY, 0.0f);
	gluSphere(quadric[21], 1.0f, 30, 30);
	dX += 4.5f;

	////// Yellow /////////////////////////////////////
	// Ambient
	MaterialAmbient[0] = 0.05f; // r
	MaterialAmbient[1] = 0.05f; // g
	MaterialAmbient[2] = 0.0f; // b
	MaterialAmbient[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// Diffuse
	MaterialDiffuse[0] = 0.5f; // r
	MaterialDiffuse[1] = 0.5f; // g
	MaterialDiffuse[2] = 0.4f; // b
	MaterialDiffuse[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// Specular
	MaterialSpecular[0] = 0.7f; // r
	MaterialSpecular[1] = 0.7f; // g
	MaterialSpecular[2] = 0.04f; // b
	MaterialSpecular[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// Shininess
	MaterialShininess[0] = 0.078125 * 128.0f;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(dX, dY, 0.0f);
	gluSphere(quadric[22], 1.0f, 30, 30);
	dX += 4.5f;

	dX = 2.5f;
	dY += 4.0f;
	#pragma endregion
	
}


