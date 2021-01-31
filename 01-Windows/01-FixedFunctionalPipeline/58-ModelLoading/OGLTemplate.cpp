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

#define BUFFER_SIZE 	256
#define NR_FACE_TOKENS 	3

// error handling macros
#define ERRORBOX1(lpszErrorMsg, lpszCaption) { \
		MessageBox((HWND)NULL, TEXT(lpszErrorMsg), \
		TEXT(lpszCaption), MB_ICONERROR); \
		ExitProcess(EXIT_FAILURE); \
		}

#define ERRORBOX2(hwnd, lpszErrorMsg, lpszCaption) { \
		MessageBox((HWND)NULL, TEXT(lpszErrorMsg), \
		TEXT(lpszCaption), MB_ICONERROR); \
		DestroyWindow(hwnd); \
		}


// global function declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// structure definitions for vector of vector of integers and vector of vector of floating point numbers
typedef struct vec_2d_int
{
    GLint **pp_arr;
    size_t size;
} vec_2d_int_t;

typedef struct vec_2d_float
{
    GLfloat **pp_arr;
    size_t size;
} vec_2d_float_t;


// global variables
HDC   ghdc = NULL;
HGLRC ghrc = NULL;

bool gbFullscreen   = false;
bool gbActiveWindow = false;

HWND  ghwnd  = NULL;
FILE* gpFile = NULL;

DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };

GLfloat angle = 0.0f;

// model related data
vec_2d_float_t *gp_vertices;
vec_2d_float_t *gp_texture;
vec_2d_float_t *gp_normals;

vec_2d_int_t *gp_face_tri, *gp_face_texture, *gp_face_normals;

FILE *gpMeshFile = NULL;
char g_line[BUFFER_SIZE];

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
		TEXT("OpenGL | Mesh Loading"),
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
	void uninitialize(void);
	void LoadMeshData(void);

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

	// set clear color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// depth
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glShadeModel(GL_SMOOTH);

	// read mesh file and load global vectors with appropriate data
	LoadMeshData();

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
	// code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -5.0f);
	glRotatef(angle, 0.0f, 1.0f, 0.0f);
	
	// keep counter-clockwise winding order of vertices of geometry
	glFrontFace(GL_CCW);
	// set polygon mode mentioning front and back faces as GL_LINES
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	for (int i = 0; i < gp_face_tri->size; i++)
	{
		glBegin(GL_TRIANGLES);

		for (int j = 0; j < 3; j++)
		{
			int vi = gp_face_tri->pp_arr[i][j] - 1;
			glVertex3f(gp_vertices->pp_arr[vi][0], gp_vertices->pp_arr[vi][1], gp_vertices->pp_arr[vi][2]);
		}


		glEnd();
	}

	SwapBuffers(ghdc);
}

void update(void)
{
	angle += 1.0f;
	if (angle >= 360.0f) angle = 0.0f;
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


// mesh data function
void LoadMeshData(void)
{
	// release and destroy resources
	void uninitialize(void);

	// create vector of vector of integers for floating point numbers
	vec_2d_int_t *create_vec_2d_int(void);
	vec_2d_float_t *create_vec_2d_float(void);

	// push array of integers and floating point numbers in vector of vectors of integers and floats
	void push_back_vec_2d_int(vec_2d_int_t *p_vec, int *p_arr);
	void push_back_vec_2d_float(vec_2d_float_t *p_vec, float *p_arr);

	// wrapper around calloc
	void *xcalloc(int, size_t);

	// code
	gpMeshFile = fopen("monkey.obj", "r");
	if (!gpMeshFile)
		uninitialize();

	// instantiate three vector of vector of floats to hold cartesian, texture and normal coordinates
	gp_vertices = create_vec_2d_float();
	gp_texture = create_vec_2d_float();
	gp_normals = create_vec_2d_float();

	// instantiate three vector of vector of integers to hold the face data regarding cartesian, texture and normal coordinates
	gp_face_tri = create_vec_2d_int();
	gp_face_texture = create_vec_2d_int();
	gp_face_normals = create_vec_2d_int();

	// separator string
	// string holding space for strtok
	char *sep_space = " ";
	// string holding forward slash separafor for strtok
	char *sep_fslash = "/";

	// token pointers
	// character pointer for holding next word in a line
	char *first_token = NULL;
	// character pointer for holding the next word separated by specific separator for strtok
	char *token = NULL;

	// array of characters pointers to hold strings of face entries
	// face entries can be variable. In some files they are three and in some files they are four
	char *face_tokens[NR_FACE_TOKENS];
	// number of non-null tokens in above vector
	int nr_tokens;

	char *token_vertex_index = NULL;
	char *token_texture_index = NULL;
	char *token_normal_index = NULL;

	while (fgets(g_line, BUFFER_SIZE, gpMeshFile) != NULL)
	{
		// bind the line to a separator and get first token
		first_token = strtok(g_line, sep_space);

		// if first token indicates vertex data
		if (strcmp(first_token, "v") == 0)
		{
			// create vector of 3 number of floats to hold the coordinates
			GLfloat *pvec_point_coord = (GLfloat*)xcalloc(3, sizeof(GLfloat));

			for (int i = 0; i != 3; i++)
				pvec_point_coord[i] = atof(strtok(NULL, sep_space));
			
			push_back_vec_2d_float(gp_vertices, pvec_point_coord);
		}

		// if first token indicates texture data
		else if (strcmp(first_token, "vt") == 0)
		{
			// create vector of 2 number of floats to hold the coordinates
			GLfloat *pvec_texture_coord = (GLfloat*)xcalloc(2, sizeof(GLfloat));

			for (int i = 0; i != 2; i++)
				pvec_texture_coord[i] = atof(strtok(NULL, sep_space));
			
			push_back_vec_2d_float(gp_texture, pvec_texture_coord);
		}

		// if first token indicates normal data
		else if (strcmp(first_token, "vn") == 0)
		{
			// create vector of 3 number of floats to hold the coordinates
			GLfloat *pvec_normal_coord = (GLfloat*)xcalloc(3, sizeof(GLfloat));

			for (int i = 0; i != 3; i++)
				pvec_normal_coord[i] = atof(strtok(NULL, sep_space));
			
			push_back_vec_2d_float(gp_normals, pvec_normal_coord);
		}

		// if first token indicates face data
		else if (strcmp(first_token, "f") == 0)
		{
			// define three vector of ints with length 3 to hold indices of triangle's positional, texture and normal coordinates
			GLint *pvec_vertex_indices = (GLint*)xcalloc(3, sizeof(GLint));
			GLint *pvec_texture_indices = (GLint*)xcalloc(3, sizeof(GLint));
			GLint *pvec_normal_indices = (GLint*)xcalloc(3, sizeof(GLint));

			// initialize all char pointers in face_token to NULL
			memset((void*)face_tokens, 0, 3);

			// extract three fields of information in faec_tokens
			// and increment nr_tokens accordingly
			nr_tokens = 0;
			while (token = strtok(NULL, sep_space))
			{
				if (strlen(token) < 3)
					break;

				face_tokens[nr_tokens] = token;
				nr_tokens++;
			}

			for (int i = 0; i < 3; i++)
			{
				token_vertex_index = strtok(face_tokens[i], sep_fslash);
				token_texture_index = strtok(NULL, sep_fslash);
				token_normal_index = strtok(NULL, sep_fslash);

				pvec_vertex_indices[i] = atoi(token_vertex_index);
				pvec_texture_indices[i] = atoi(token_texture_index);
				pvec_normal_indices[i] = atoi(token_normal_index);
			}

			// add constructed vectors to global face vectors
			push_back_vec_2d_int(gp_face_tri, pvec_vertex_indices);
			push_back_vec_2d_int(gp_face_texture, pvec_texture_indices);
			push_back_vec_2d_int(gp_face_normals, pvec_normal_indices);
		}

		// initialize line buffer to NULL
		memset((void*)g_line, (int)'\0', BUFFER_SIZE);		
	}

	// close the mesh file
	fclose(gpMeshFile);
	gpMeshFile = NULL;

	// log vertex, texture and face data in log file
	fprintf(gpFile, "Vertices: %zu, Textures: %zu, Normals: %zu, Face Triangle: %zu\n", gp_vertices->size, gp_texture->size, gp_normals->size, gp_face_tri->size);
}

// vector routines implementations
vec_2d_int_t *create_vec_2d_int(void)
{
	// wrapper around calloc
	void *xcalloc(int nr_elements, size_t size_per_element);

	// code
	return (vec_2d_int_t*)xcalloc(1, sizeof(vec_2d_int_t));
}

vec_2d_float_t *create_vec_2d_float(void)
{
	// wrapper around calloc
	void *xcalloc(int nr_elements, size_t size_per_element);

	// code
	return (vec_2d_float_t*)xcalloc(1, sizeof(vec_2d_float_t));
}

void push_back_vec_2d_int(vec_2d_int_t *p_vec, GLint *p_arr)
{
	// wrapper around realloc
	void *xrealloc(void *p, size_t new_size);

	// code
	p_vec->pp_arr = (GLint**)xrealloc(p_vec->pp_arr, (p_vec->size+1)*sizeof(int*));
	p_vec->size++;
	p_vec->pp_arr[p_vec->size-1] = p_arr;
}

void push_back_vec_2d_float(vec_2d_float_t *p_vec, GLfloat *p_arr)
{
	// wrapper around realloc
	void *xrealloc(void *p, size_t new_size);

	// code
	p_vec->pp_arr = (GLfloat**)xrealloc(p_vec->pp_arr, (p_vec->size+1)*sizeof(float*));
	p_vec->size++;
	p_vec->pp_arr[p_vec->size-1] = p_arr;
}

void clean_vec_2d_int(vec_2d_int_t **pp_vec)
{
	vec_2d_int_t *p_vec = *pp_vec;
	for (size_t i = 0; i < p_vec->size; i++)
		free(p_vec->pp_arr[i]);
	free(p_vec);
	*pp_vec = NULL;
}

void clean_vec_2d_float(vec_2d_float_t **pp_vec)
{
	vec_2d_float_t *p_vec = *pp_vec;
	for (size_t i = 0; i < p_vec->size; i++)
		free(p_vec->pp_arr[i]);
	free(p_vec);
	*pp_vec = NULL;
}

void *xcalloc(int nr_element, size_t size_per_element)
{
	// release and destroy the resources
	void uninitialize(void);

	// code
	void *p = calloc(nr_element, size_per_element);

	if (!p)
	{
		fprintf(gpFile, "calloc: fatal: out of memory\n");
		uninitialize();
	}

	return(p);
}

void *xrealloc(void *p, size_t new_size)
{
	// release and destroy the resources
	void uninitialize(void);

	// code
	void *ptr = realloc(p, new_size);

	if (!ptr)
	{
		fprintf(gpFile, "realloc: fatal: out of memory\n");
		uninitialize();
	}
	
	return(ptr);
}
