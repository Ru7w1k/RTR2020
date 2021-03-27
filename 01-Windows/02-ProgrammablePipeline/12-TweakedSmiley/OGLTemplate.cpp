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

GLuint vaoQuad;
GLuint vboPositionQuad;
GLuint vboTexcoordQuad;

GLuint mvpMatrixUniform;
GLuint textureSamplerUniform;
GLuint textureEnableUniform;

mat4 perspectiveProjectionMatrix;

GLuint smileyTexture;
int pressedDigit = 0;

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
		TEXT("OpenGL | Tweaked Smiley Texture"),
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

		case 48:
		case VK_NUMPAD0:
			pressedDigit = 0;
			break;

		case 49:
		case VK_NUMPAD1:
			pressedDigit = 1;
			break;

		case 50:
		case VK_NUMPAD2:
			pressedDigit = 2;
			break;

		case 51:
		case VK_NUMPAD3:
			pressedDigit = 3;
			break;

		case 52:
		case VK_NUMPAD4:
			pressedDigit = 4;
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
	bool loadGLTexture(GLuint *, TCHAR[]);

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
		"in vec2 vTexcoord; \n" \

		"uniform mat4 u_mvpMatrix; \n" \

		"out vec2 out_Texcoord; \n" \

		"void main (void) \n" \
		"{ \n" \
		"	gl_Position = u_mvpMatrix * vPosition; \n" \
		"	out_Texcoord = vTexcoord; \n" \
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

		"in vec2 out_Texcoord; \n" \

		"uniform int u_textureEnable; \n"
		"uniform sampler2D u_textureSampler; \n"

		"out vec4 FragColor; \n" \

		"void main (void) \n" \
		"{ \n" \
		"	if (u_textureEnable == 1) "
		"		FragColor = texture(u_textureSampler, out_Texcoord); \n" \
		"	else "
		"		FragColor = vec4(1.0); \n" \
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
	glBindAttribLocation(gShaderProgramObject, RMC_ATTRIBUTE_TEXCOORD, "vTexcoord");

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
	mvpMatrixUniform      = glGetUniformLocation(gShaderProgramObject, "u_mvpMatrix");
	textureSamplerUniform = glGetUniformLocation(gShaderProgramObject, "u_textureSampler");
	textureEnableUniform  = glGetUniformLocation(gShaderProgramObject, "u_textureEnable");
	
	// position array of quad
	const GLfloat quadVertices[] = {
		  1.0f,  1.0f, 0.0f,
		 -1.0f,  1.0f, 0.0f,
		 -1.0f, -1.0f, 0.0f,
		  1.0f, -1.0f, 0.0f
	};

	// create vao for quad
	glGenVertexArrays(1, &vaoQuad);
	glBindVertexArray(vaoQuad);

	// create vbo for position
	glGenBuffers(1, &vboPositionQuad);
	glBindBuffer(GL_ARRAY_BUFFER, vboPositionQuad);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(RMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(RMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// create vbo for texcoords
	glGenBuffers(1, &vboTexcoordQuad);
	glBindBuffer(GL_ARRAY_BUFFER, vboTexcoordQuad);
	glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(GLfloat), NULL, GL_STATIC_DRAW);
	glVertexAttribPointer(RMC_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(RMC_ATTRIBUTE_TEXCOORD);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	// set clear color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// set clear depth
	glClearDepth(1.0f);

	// depth test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// load textures
	glEnable(GL_TEXTURE_2D);
	loadGLTexture(&smileyTexture, MAKEINTRESOURCE(SMILEY_BITMAP));

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

	perspectiveProjectionMatrix = vmath::perspective(45.0f, (float)width/(float)height, 0.1f, 100.0f);
}

void display(void)
{
	// code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// start using OpenGL program object
	glUseProgram(gShaderProgramObject);

	//declaration of matrices
	mat4 translateMatrix;
	mat4 modelViewMatrix;
	mat4 modelViewProjectionMatrix;

	//// quad ////////////////////////

	// intialize above matrices to identity
	translateMatrix = mat4::identity();
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();

	// transformations
	translateMatrix = translate(0.0f, 0.0f, -3.0f);
	modelViewMatrix = translateMatrix;

	// do necessary matrix multiplication
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	// send necessary matrices to shader in respective uniforms
	glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	// bind the texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, smileyTexture);
	glUniform1i(textureSamplerUniform, 0);

	// bind with vaoQuad (this will avoid many binding to vbo)
	glBindVertexArray(vaoQuad);

	// texture array of quad
	GLfloat quadTexcoords[] = {
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f
	};

	if (pressedDigit == 1)
	{
		quadTexcoords[0] = 0.5f;
		quadTexcoords[1] = 0.5f;
		quadTexcoords[2] = 0.0f;
		quadTexcoords[3] = 0.5f;
		quadTexcoords[4] = 0.0f;
		quadTexcoords[5] = 0.0f;
		quadTexcoords[6] = 0.5f;
		quadTexcoords[7] = 0.0f;
	}
	else if (pressedDigit == 2)
	{
		quadTexcoords[0] = 1.0f;
		quadTexcoords[1] = 1.0f;
		quadTexcoords[2] = 0.0f;
		quadTexcoords[3] = 1.0f;
		quadTexcoords[4] = 0.0f;
		quadTexcoords[5] = 0.0f;
		quadTexcoords[6] = 1.0f;
		quadTexcoords[7] = 0.0f;
	}
	else if (pressedDigit == 3)
	{
		quadTexcoords[0] = 2.0f;
		quadTexcoords[1] = 2.0f;
		quadTexcoords[2] = 0.0f;
		quadTexcoords[3] = 2.0f;
		quadTexcoords[4] = 0.0f;
		quadTexcoords[5] = 0.0f;
		quadTexcoords[6] = 2.0f;
		quadTexcoords[7] = 0.0f;
	}
	else if (pressedDigit == 4)
	{
		quadTexcoords[0] = 0.5f;
		quadTexcoords[1] = 0.5f;
		quadTexcoords[2] = 0.5f;
		quadTexcoords[3] = 0.5f;
		quadTexcoords[4] = 0.5f;
		quadTexcoords[5] = 0.5f;
		quadTexcoords[6] = 0.5f;
		quadTexcoords[7] = 0.5f;
	}

	if (pressedDigit != 0)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vboTexcoordQuad);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadTexcoords), quadTexcoords, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glUniform1i(textureEnableUniform, 1);
	}
	else
	{
		glUniform1i(textureEnableUniform, 0);
	}

	// draw necessary scene
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	// unbind vaoQuad
	glBindVertexArray(0);

	// stop using OpenGL program object
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

	glDeleteTextures(1, &smileyTexture);

	if (vaoQuad)
	{
		glDeleteVertexArrays(1, &vaoQuad);
		vaoQuad = 0;
	}

	if (vboPositionQuad)
	{
		glDeleteBuffers(1, &vboPositionQuad);
		vboPositionQuad = 0;
	}

	if (vboTexcoordQuad)
	{
		glDeleteBuffers(1, &vboTexcoordQuad);
		vboTexcoordQuad = 0;
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

bool loadGLTexture(GLuint *texture, TCHAR resourceID[])
{
	// variables
	bool bResult = false;
	HBITMAP hBitmap = NULL;
	BITMAP bmp;

	// code
	hBitmap = (HBITMAP) LoadImage(
		GetModuleHandle(NULL),      // hInstance
		resourceID,
		IMAGE_BITMAP,
		0,
		0,
		LR_CREATEDIBSECTION
	);

	if (hBitmap)
	{
		// OS dependent image handling code
		bResult = true;
		GetObject(hBitmap, sizeof(BITMAP), &bmp);

		// opengl code
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		
		glGenTextures(1, texture);
		glBindTexture(GL_TEXTURE_2D, *texture);

		// setting texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		// push data into graphics memory with the help of driver
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bmp.bmWidth, bmp.bmHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, bmp.bmBits);
		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);

		DeleteObject(hBitmap);
	}

	return bResult;
}



