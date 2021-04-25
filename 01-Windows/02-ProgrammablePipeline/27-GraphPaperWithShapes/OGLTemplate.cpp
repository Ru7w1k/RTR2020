// headers
#include <windows.h>
#include <stdio.h>

#include <gl/glew.h> 
#include <gl/GL.h>

#include "vmath.h"
#include "OGLTemplate.h"

#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "opengl32.lib")

// macros
#define WIN_WIDTH  800
#define WIN_HEIGHT 600

using namespace vmath;

enum {
	RMC_ATTRIBUTE_POSITION = 0,
	RMC_ATTRIBUTE_COLOR,
	RMC_ATTRIBUTE_NORMAL,
	RMC_ATTRIBUTE_TEXCOORD,
};

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

GLuint gShaderProgramObject;

GLuint vao;			// vertex array object
GLuint vbo_vertex;	// vertex buffer object
GLuint vbo_color;   // vertex buffer object

GLuint vaoAxes;			// vertex array object
GLuint vbo_vertexAxes;	// vertex buffer object
GLuint vbo_colorAxes;	// vertex buffer object

GLuint vaoShapes;
GLuint vbo_vertexShapes;

GLuint mvpUniform;
mat4   perspectiveProjectionMatrix;

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
		TEXT("OpenGL | GraphPaper with Shapes"),
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

	int GenerateGraphCoordinates(GLfloat **ppos);
	int generateTriangleAndIncircleCoords(float fY, float fX, GLfloat *coords, int idx);
	int generateSquareCoords(GLfloat fX, GLfloat fY, GLfloat *coords, int idx);
	int generateOuterCircleCoords(GLfloat *coords, int idx);

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

	// glew initialization for programmable pipeline
	GLenum glew_error = glewInit();
	if (glew_error != GLEW_OK)
	{
		fprintf(gpFile, "glewInit() failed..\n");
		DestroyWindow(ghwnd);
	}

	// fetch OpenGL related details
	fprintf(gpFile, "OpenGL Vendor:   %s\n", glGetString(GL_VENDOR));
	fprintf(gpFile, "OpenGL Renderer: %s\n", glGetString(GL_RENDERER));
	fprintf(gpFile, "OpenGL Version:  %s\n", glGetString(GL_VERSION));
	fprintf(gpFile, "GLSL Version:    %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	// fetch OpenGL enabled extensions
	GLint numExtensions;
	glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);

	fprintf(gpFile, "==== OpenGL Extensions ====\n");
	for (int i = 0; i < numExtensions; i++)
	{
		fprintf(gpFile, "  %s\n", glGetStringi(GL_EXTENSIONS, i));
	}
	fprintf(gpFile, "===========================\n\n");

	//// vertex shader
	// create shader
	GLuint gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	// provide source code to shader
	const GLchar *vertexShaderSourceCode = 
		"#version 450 core \n" \

		"in vec4 vPosition; \n" \
		"in vec4 vColor; \n" \
		"uniform mat4 u_mvpMatrix; \n" \
		"out vec4 out_Color; \n" \

		"void main (void) \n" \
		"{ \n" \
		"	gl_Position = u_mvpMatrix * vPosition; \n" \
		"	out_Color = vColor; \n" \
		"} \n";

	glShaderSource(gVertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);

	// compile shader
	glCompileShader(gVertexShaderObject);

	// compilation errors 
	GLint iShaderCompileStatus = 0;
	GLint iInfoLogLength = 0;
	GLchar *szInfoLog = NULL;

	glGetShaderiv(gVertexShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject, GL_INFO_LOG_LENGTH, &written, szInfoLog);

				fprintf(gpFile, "Vertex Shader Compiler Info Log: \n%s\n", szInfoLog);
				free(szInfoLog);
				DestroyWindow(ghwnd);
			}
		}
	}

	//// fragment shader
	// create shader
	GLuint gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	// provide source code to shader
	const GLchar *fragmentShaderSourceCode = 
		"#version 450 core \n" \

		"in vec4 out_Color; \n" \
		"out vec4 FragColor; \n" \

		"void main (void) \n" \
		"{ \n" \
		"	FragColor = out_Color; \n" \
		"} \n";

	glShaderSource(gFragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);

	// compile shader
	glCompileShader(gFragmentShaderObject);

	// compile errors
	iShaderCompileStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;

	glGetShaderiv(gFragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObject, GL_INFO_LOG_LENGTH, &written, szInfoLog);

				fprintf(gpFile, "Fragment Shader Compiler Info Log: \n%s\n", szInfoLog);
				free(szInfoLog);
				DestroyWindow(ghwnd);
			}
		}
	}

	//// shader program
	// create
	gShaderProgramObject = glCreateProgram();

	// attach shaders
	glAttachShader(gShaderProgramObject, gVertexShaderObject);
	glAttachShader(gShaderProgramObject, gFragmentShaderObject);

	// pre-linking binding to vertex attribute
	glBindAttribLocation(gShaderProgramObject, RMC_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(gShaderProgramObject, RMC_ATTRIBUTE_COLOR, "vColor");

	// link shader
	glLinkProgram(gShaderProgramObject);

	// linking errors
	GLint iProgramLinkStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;

	glGetProgramiv(gShaderProgramObject, GL_LINK_STATUS, &iProgramLinkStatus);
	if (iProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObject, GL_INFO_LOG_LENGTH, &written, szInfoLog);

				fprintf(gpFile, ("Shader Program Linking Info Log: \n%s\n"), szInfoLog);
				free(szInfoLog);
				DestroyWindow(ghwnd);
			}
		}
	}

	// post-linking retrieving uniform locations
	mvpUniform = glGetUniformLocation(gShaderProgramObject, "u_mvpMatrix");
	
	// vertex array
	GLfloat *graphCoords = NULL;
	int coords = GenerateGraphCoordinates(&graphCoords);

	// color array 
	const GLfloat axisCoords[] = {
		-1.0f,  0.0f, 0.0f,
		 1.0f,  0.0f, 0.0f,
		 0.0f, -1.0f, 0.0f,
	 	 0.0f,  1.0f, 0.0f
	};

	const GLfloat axisColors[] = {
		 1.0f,  0.0f, 0.0f,
		 1.0f,  0.0f, 0.0f,
		 0.0f,  1.0f, 0.0f,
		 0.0f,  1.0f, 0.0f
	};

	GLfloat *smallAxisColors = (GLfloat *)malloc(coords * 3 * sizeof(GLfloat));
	for (int i = 0; i < (coords * 3); i += 3) {
		smallAxisColors[i + 0] = 0.0f;
		smallAxisColors[i + 1] = 0.0f;
		smallAxisColors[i + 2] = 1.0f;
	}

	GLfloat shapesCoords[1300 * 3];
	int shapesCoordsCount = 0;
	float fX, fY;

	shapesCoordsCount = generateOuterCircleCoords(shapesCoords, shapesCoordsCount);

	fX = fY = (GLfloat)cos(M_PI / 4.0);
	shapesCoordsCount = generateSquareCoords(fX, fY, shapesCoords, shapesCoordsCount);
	shapesCoordsCount = generateTriangleAndIncircleCoords(fX, fY, shapesCoords, shapesCoordsCount);

	// create vao
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// vbo position axes
	glGenBuffers(1, &vbo_vertex);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertex);
	glBufferData(GL_ARRAY_BUFFER, coords * 3 * sizeof(GLfloat), graphCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(RMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(RMC_ATTRIBUTE_POSITION);

	// vbo color axes
	glGenBuffers(1, &vbo_color);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
	glBufferData(GL_ARRAY_BUFFER, coords * 3 * sizeof(GLfloat), smallAxisColors, GL_STATIC_DRAW);
	glVertexAttribPointer(RMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(RMC_ATTRIBUTE_COLOR);

	// vao for Major axis
	glGenVertexArrays(1, &vaoAxes);
	glBindVertexArray(vaoAxes);

	// vbo position axes
	glGenBuffers(1, &vbo_vertexAxes);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertexAxes);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axisCoords), axisCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(RMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(RMC_ATTRIBUTE_POSITION);

	// vbo color axes
	glGenBuffers(1, &vbo_colorAxes);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_colorAxes);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axisColors), axisColors, GL_STATIC_DRAW);
	glVertexAttribPointer(RMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(RMC_ATTRIBUTE_COLOR);

	// vao for shapes
	glGenVertexArrays(1, &vaoShapes);
	glBindVertexArray(vaoShapes);

	// shapes vertices
	glGenBuffers(1, &vbo_vertexShapes);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertexShapes);

	glBufferData(GL_ARRAY_BUFFER, sizeof(shapesCoords), shapesCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(RMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(RMC_ATTRIBUTE_POSITION);

	glVertexAttrib3f(RMC_ATTRIBUTE_COLOR, 1.0f, 1.0f, 0.0f);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// set clear color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// set clear depth
	glClearDepth(1.0f);

	// depth test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	perspectiveProjectionMatrix = mat4::identity();

	// warm-up resize call
	resize(WIN_WIDTH, WIN_HEIGHT);
}

void resize(int width, int height)
{
	// code
	if (height == 0)
		height = 1;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	perspectiveProjectionMatrix = perspective(45.0f, (float)width / (float)height, 0.1f, 100.0f);
}

void display(void)
{
	// code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// use shader program
	glUseProgram(gShaderProgramObject);

	//declaration of matrices
	mat4 translationMatrix;
	mat4 modelViewMatrix;
	mat4 modelViewProjectionMatrix;

	// intialize above matrices to identity
	translationMatrix = mat4::identity();
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();

	// perform necessary transformations
	translationMatrix = translate(0.0f, 0.0f, -2.5f);

	// do necessary matrix multiplication
	modelViewMatrix *= translationMatrix;
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	// send necessary matrices to shader in respective uniforms
	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	// bind with vao (this will avoid many binding to vbo_vertex)
	glBindVertexArray(vao);

	// bind with textures

	// draw necessary scene
	glLineWidth(1.0f);
	glDrawArrays(GL_LINES, 0, 160);

	// bind with vao (this will avoid many binding to vbo_vertex)
	glBindVertexArray(vaoAxes);

	// draw necessary scene
	glLineWidth(3.0f);
	glDrawArrays(GL_LINES, 0, 4);

	// shapes
	glBindVertexArray(vaoShapes);

	// draw necessary scene
	glLineWidth(2.0f);
	glDrawArrays(GL_LINE_LOOP, 0, 629);
	glDrawArrays(GL_LINE_LOOP, 629, 4);
	glDrawArrays(GL_LINE_LOOP, 633, 3);
	glDrawArrays(GL_LINE_LOOP, 636, 629);

	// unbind vao
	glBindVertexArray(0);

	// unuse program
	glUseProgram(0);

	SwapBuffers(ghdc);
}

void update(void)
{
	// code
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

	if (vao)
	{
		glDeleteVertexArrays(1, &vao);
		vao = 0;
	}

	if (vbo_vertex)
	{
		glDeleteBuffers(1, &vbo_vertex);
		vbo_vertex = 0;
	}

	if (vbo_color)
	{
		glDeleteBuffers(1, &vbo_color);
		vbo_color = 0;
	}

	if (vaoAxes)
	{
		glDeleteVertexArrays(1, &vaoAxes);
		vaoAxes = 0;
	}

	if (vbo_vertexAxes)
	{
		glDeleteBuffers(1, &vbo_vertexAxes);
		vbo_vertexAxes = 0;
	}

	if (vbo_colorAxes)
	{
		glDeleteBuffers(1, &vbo_colorAxes);
		vbo_colorAxes = 0;
	}

	if (vaoShapes)
	{
		glDeleteVertexArrays(1, &vaoShapes);
		vaoShapes = 0;
	}

	if (vbo_vertexShapes)
	{
		glDeleteBuffers(1, &vbo_vertexShapes);
		vbo_vertexShapes = 0;
	}

	// destroy shader programs
	if (gShaderProgramObject)
	{
		GLsizei shaderCount;
		GLsizei i;

		glUseProgram(gShaderProgramObject);
		glGetProgramiv(gShaderProgramObject, GL_ATTACHED_SHADERS, &shaderCount);
		
		GLuint *pShaders = (GLuint*) malloc(shaderCount * sizeof(GLuint));
		if (pShaders)
		{
			glGetAttachedShaders(gShaderProgramObject, shaderCount, &shaderCount, pShaders);

			for (i = 0; i < shaderCount; i++)
			{
				// detach shader
				glDetachShader(gShaderProgramObject, pShaders[i]);

				// delete shader
				glDeleteShader(pShaders[i]);
				pShaders[i] = 0;
			}

			free(pShaders);
		}

		glDeleteProgram(gShaderProgramObject);
		gShaderProgramObject = 0;
		glUseProgram(0);
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

int GenerateGraphCoordinates(GLfloat **ppos)
{
	int iNoOfCoords = 0;

	*ppos = (GLfloat *)malloc(3 * sizeof(GLfloat) * 160);

	GLfloat *pos = *ppos;

	for (float fOffset = -1.0f; fOffset <= 0; fOffset += (1.0f / 20.0f))
	{
		pos[(iNoOfCoords * 3) + 0] = -1.0f;
		pos[(iNoOfCoords * 3) + 1] = fOffset;
		pos[(iNoOfCoords * 3) + 2] = 0.0f;
		iNoOfCoords++;

		pos[(iNoOfCoords * 3) + 0] = 1.0f;
		pos[(iNoOfCoords * 3) + 1] = fOffset;
		pos[(iNoOfCoords * 3) + 2] = 0.0f;
		iNoOfCoords++;

		pos[(iNoOfCoords * 3) + 0] = -1.0f;
		pos[(iNoOfCoords * 3) + 1] = fOffset + 1.0f + (1.0f / 20.0f);
		pos[(iNoOfCoords * 3) + 2] = 0.0f;
		iNoOfCoords++;

		pos[(iNoOfCoords * 3) + 0] = 1.0f;
		pos[(iNoOfCoords * 3) + 1] = fOffset + 1.0f + (1.0f / 20.0f);
		pos[(iNoOfCoords * 3) + 2] = 0.0f;
		iNoOfCoords++;
	}

	for (float fOffset = -1.0f; fOffset <= 0; fOffset += (1.0f / 20.0f))
	{
		pos[(iNoOfCoords * 3) + 0] = fOffset;
		pos[(iNoOfCoords * 3) + 1] = -1.0f;
		pos[(iNoOfCoords * 3) + 2] = 0.0f;
		iNoOfCoords++;

		pos[(iNoOfCoords * 3) + 0] = fOffset;
		pos[(iNoOfCoords * 3) + 1] = 1.0f;
		pos[(iNoOfCoords * 3) + 2] = 0.0f;
		iNoOfCoords++;

		pos[(iNoOfCoords * 3) + 0] = fOffset + 1.0f + (1.0f / 20.0f);
		pos[(iNoOfCoords * 3) + 1] = -1.0f;
		pos[(iNoOfCoords * 3) + 2] = 0.0f;
		iNoOfCoords++;

		pos[(iNoOfCoords * 3) + 0] = fOffset + 1.0f + (1.0f / 20.0f);
		pos[(iNoOfCoords * 3) + 1] = 1.0f;
		pos[(iNoOfCoords * 3) + 2] = 0.0f;
		iNoOfCoords++;		
	}

	return iNoOfCoords;
}

int generateTriangleAndIncircleCoords(float fY, float fX, GLfloat *coords, int idx) 
{
	// variables 
	GLfloat s, a, b, c;
	GLfloat fRadius = 1.0f;
	GLfloat fAngle = 0.0f;

	/* Triangle */
	coords[idx++] = 0.0f;
	coords[idx++] = fY;
	coords[idx++] = 0.0f;

	coords[idx++] = -fX;
	coords[idx++] = -fY;
	coords[idx++] = 0.0f;

	coords[idx++] = fX;
	coords[idx++] = -fY;
	coords[idx++] = 0.0f;

	/* Radius Of Incircle */
	a = (GLfloat)sqrt(pow((-fX - 0.0f), 2.0f) + pow(-fY - fY, 2.0f));
	b = (GLfloat)sqrt(pow((fX - (-fX)), 2.0f) + pow(-fY - (-fY), 2.0f));
	c = (GLfloat)sqrt(pow((fX - 0.0f), 2.0f) + pow(-fY - fY, 2.0f));
	s = (a + b + c) / 2.0f;
	fRadius = (GLfloat)sqrt(s * (s - a) * (s - b) * (s - c)) / s;

	/* Incircle */
	for (fAngle = 0.0f; fAngle < 2 * M_PI; fAngle += 0.01f)
	{
		coords[idx++] = fRadius * (GLfloat)cos(fAngle);
		coords[idx++] = (fRadius * (GLfloat)sin(fAngle)) - fX + fRadius;
		coords[idx++] = 0.0f;
	}

	return idx;
}

int generateSquareCoords(GLfloat fX, GLfloat fY, GLfloat *coords, int idx) {

	coords[idx++] = fX;
	coords[idx++] = fY;
	coords[idx++] = 0.0f;

	coords[idx++] = -fX;
	coords[idx++] = fY;
	coords[idx++] = 0.0f;

	coords[idx++] = -fX;
	coords[idx++] = -fY;
	coords[idx++] = 0.0f;

	coords[idx++] = fX;
	coords[idx++] = -fY;
	coords[idx++] = 0.0f;

	return idx;
}

int generateOuterCircleCoords(GLfloat *coords, int idx) {
	float fRadius = 1.0f;

	for (float fAngle = 0.0f; fAngle < 2 * M_PI; fAngle += 0.01f)
	{
		coords[idx++] = fRadius * (GLfloat)cos(fAngle);
		coords[idx++] = fRadius * (GLfloat)sin(fAngle);
		coords[idx++] = 0.0f;
	}

	return idx;
}

