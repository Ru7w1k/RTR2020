// Headers
#include <Windows.h>
#include <stdio.h>

#include <GL/glew.h>
#include <gl/GL.h>

#include "vmath.h"

// Linker Options
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "opengl32.lib")

// Defines
#define WIN_WIDTH  800
#define WIN_HEIGHT 600

using namespace vmath;

enum {
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTRIBUTE_NORMAL,
	AMC_ATTRIBUTE_TEXCOORD0
};

// Global Variables
FILE  *gpFile = NULL;
bool  gbActiveWindow = false;
bool  gbIsFullScreen = false;
HDC   ghDC = NULL;
HGLRC ghRC = NULL;
HWND  ghWnd = NULL;
DWORD dwStyle;

WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };

GLuint gShaderProgramObject;

GLuint vao;			// vertex array object
GLuint vbo;			// vertex buffer object
GLuint mvUniform;	
GLuint pUniform;	

mat4   perspectiveProjectionMatrix;

// Global function declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// WinMain
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	// function declarations
	int  initialize(void);
	void display(void);
	void update(void);

	// variables 
	bool bDone = false;
	int  iRet  = 0;

	WNDCLASSEX wndclass;
	HWND  hwnd;
	MSG   msg;
	TCHAR szClassName[] = TEXT("MyApp");

	// code
	// create file for logging
	if (fopen_s(&gpFile, "log.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("Cannot Create log file!"), TEXT("Error"), MB_OK | MB_ICONERROR);
		exit(0);
	}
	else
	{
		fprintf(gpFile, "Log.txt file created...\n");
	}

	// initialization of WNDCLASSEX
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszClassName = szClassName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	// register class
	RegisterClassEx(&wndclass);

	// create window
	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szClassName,
		TEXT("OpenGL | Cubic Bezier Curve"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		100,
		100,
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ghWnd = hwnd;

	iRet = initialize();
	if (iRet == -1)
	{
		fprintf(gpFile, "ChoosePixelFormat failed...\n");
		DestroyWindow(hwnd);
	}
	else if (iRet == -2)
	{
		fprintf(gpFile, "SetPixelFormat failed...\n");
		DestroyWindow(hwnd);
	}
	else if (iRet == -3)
	{
		fprintf(gpFile, "wglCreateContext failed...\n");
		DestroyWindow(hwnd);
	}
	else if (iRet == -4)
	{
		fprintf(gpFile, "wglMakeCurrent failed...\n");
		DestroyWindow(hwnd);
	}
	else
	{
		fprintf(gpFile, "initialize() successful...\n");
	}

	ShowWindow(hwnd, iCmdShow);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	// Game Loop 
	while (bDone == false)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				bDone = true;
			}
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
				update();
			}
			display();
		}
	}

	return((int)msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	// function declarations
	void resize(int, int);
	void uninitialize();

	void ToggleFullScreen(void);

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

	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hwnd);
			break;

		case 0x46:
			ToggleFullScreen();
			break;
		}
		break;

		// returned from here, to block DefWindowProc
		// We have our own painter
	case WM_ERASEBKGND:
		return(0);
		break;

	case WM_DESTROY:
		uninitialize();
		PostQuitMessage(0);
		break;
	}

	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}

void ToggleFullScreen()
{
	MONITORINFO MI;

	if (gbIsFullScreen == false)
	{
		dwStyle = GetWindowLong(ghWnd, GWL_STYLE);
		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			MI = { sizeof(MONITORINFO) };
			if (GetWindowPlacement(ghWnd, &wpPrev)
				&& GetMonitorInfo(MonitorFromWindow(ghWnd, MONITORINFOF_PRIMARY), &MI))
			{
				SetWindowLong(ghWnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghWnd,
					HWND_TOP,
					MI.rcMonitor.left,
					MI.rcMonitor.top,
					MI.rcMonitor.right - MI.rcMonitor.left,
					MI.rcMonitor.bottom - MI.rcMonitor.top,
					SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
		gbIsFullScreen = true;
	}
	else
	{
		SetWindowLong(ghWnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghWnd, &wpPrev);
		SetWindowPos(ghWnd,
			HWND_TOP,
			0,
			0,
			0,
			0,
			SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER);

		ShowCursor(TRUE);
		gbIsFullScreen = false;
	}
}

int initialize(void)
{
	// function declarations
	void resize(int, int);
	void uninitialize(void);

	// variable declarations
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;
	GLenum result;

	GLuint vertexShaderObject;
	GLuint tessellationControlShaderObjcet;
	GLuint tessellationEvaluationShaderObjcet;
	GLuint fragmentShaderObject;

	// code
	// initialize pdf structure
	ZeroMemory((void *)&pfd, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 32;

	ghDC = GetDC(ghWnd);

	iPixelFormatIndex = ChoosePixelFormat(ghDC, &pfd);
	// iPixelFormatIndex is 1 based, so 0 indicates error
	if (iPixelFormatIndex == 0)
	{
		return(-1);
	}

	if (SetPixelFormat(ghDC, iPixelFormatIndex, &pfd) == FALSE)
	{
		return(-2);
	}

	ghRC = wglCreateContext(ghDC);
	if (ghRC == NULL)
	{
		return(-3);
	}

	if (wglMakeCurrent(ghDC, ghRC) == FALSE)
	{
		return(-4);
	}

	//// Programable Pipeline ////////////////////////////////////////////

	result = glewInit();
	if (result != GLEW_OK) {
		fprintf(gpFile, "GLEW initialization failed..\n");
		uninitialize();
		DestroyWindow(ghWnd);
	}

	// create vertex shader object
	vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	// vertex shader source code 
	const GLchar *vertexShaderSourceCode = (GLchar *)
		"#version 450 core" \
		"\n" \
		
		"uniform mat4 u_mv_matrix;" \
		
		"in vec4 vPosition;" \
		
		"void main (void)" \
		"{" \
		"	gl_Position = u_mv_matrix * vPosition;" \
		"}";

	// attach source code to vertex shader
	glShaderSource(vertexShaderObject, 1, (const GLchar **)&vertexShaderSourceCode, NULL);

	// compile vertex shader source code
	glCompileShader(vertexShaderObject);

	// compilation errors 
	GLint iShaderCompileStatus = 0;
	GLint iInfoLogLength = 0;
	GLchar *szInfoLog = NULL;

	glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(vertexShaderObject, GL_INFO_LOG_LENGTH, &written, szInfoLog);

				fprintf(gpFile, "Vertex Shader Compiler Info Log: %s", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(ghWnd);
			}
		}
	}

	// create tessellation control shader object
	tessellationControlShaderObjcet = glCreateShader(GL_TESS_CONTROL_SHADER);

	// tessellation control shader source code
	const GLchar *tessellationControlShaderSourceCode = (GLchar *)
		"#version 450 core" \
		"\n" \
		"layout(vertices=16)out;" \
		"void main (void)" \
		"{" \
		"	if(gl_InvocationID == 0)" \
		"	{ "\
		"		gl_TessLevelInner[0] = 16.0; " \
		"		gl_TessLevelInner[1] = 16.0; " \
		"		gl_TessLevelOuter[0] = 16.0; " \
		"		gl_TessLevelOuter[1] = 16.0; " \
		"		gl_TessLevelOuter[2] = 16.0; " \
		"		gl_TessLevelOuter[3] = 16.0; " \
		"	}" \
		"	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;" \
		"}";

	// attach source code to fragment shader
	glShaderSource(tessellationControlShaderObjcet, 1, (const GLchar **)&tessellationControlShaderSourceCode, NULL);

	// compile fragment shader source code
	glCompileShader(tessellationControlShaderObjcet);

	// compile errors
	iShaderCompileStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;

	glGetShaderiv(tessellationControlShaderObjcet, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(tessellationControlShaderObjcet, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(tessellationControlShaderObjcet, GL_INFO_LOG_LENGTH, &written, szInfoLog);

				fprintf(gpFile, "Tessellation Control Shader Compiler Info Log: %s", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(ghWnd);
			}
		}
	}

	// create tessellation evaluation shader object
	tessellationEvaluationShaderObjcet = glCreateShader(GL_TESS_EVALUATION_SHADER);

	// tessellation evaluation shader source code
	const GLchar *tessellationEvaluationShaderSourceCode = (GLchar *)
		"#version 450 core" \
		"\n" \
		"layout(quads, equal_spacing, cw)in;" \

		"uniform mat4 u_mv_matrix;" \
		"uniform mat4 u_p_matrix;" \

		"out TES_OUT " \
		"{" \
		"	vec3 N;" \
		"} tes_out;" \

		"vec4 quadratic_bezier(vec4 A, vec4 B, vec4 C, float t)" \
		"{" \
		"	vec4 D = mix(A, B, t);" \
		"	vec4 E = mix(B, C, t);" \
		"	return mix(D, E, t);" \
		"}" \

		"vec4 cubic_bezier(vec4 A, vec4 B, vec4 C, vec4 D, float t)" \
		"{" \
		"	vec4 E = mix(A, B, t);" \
		"	vec4 F = mix(B, C, t);" \
		"	vec4 G = mix(C, D, t);" \
		"	return quadratic_bezier(E, F, G, t);" \
		"}" \

		"vec4 evaluate_patch(vec2 at)" \
		"{" \
		"	vec4 P[4];" \
		"	int i;" \

		"	for (i = 0; i < 4; i++)" \
		"	{" \
		"		P[i] = cubic_bezier(gl_in[i+0].gl_Position, " \
		"							gl_in[i+4].gl_Position, " \
		"							gl_in[i+8].gl_Position, " \
		"							gl_in[i+12].gl_Position," \
		"							at.y);" \
		"	}" \
		"	return cubic_bezier(P[0], P[1], P[2], P[3], at.x);" \
		"}" \

		"const float epsilon = 0.001;" \

		"void main (void)" \
		"{" \
		"	vec4 p1 = evaluate_patch(gl_TessCoord.xy);" \
		"	vec4 p2 = evaluate_patch(gl_TessCoord.xy + vec2(0.0, epsilon));" \
		"	vec4 p3 = evaluate_patch(gl_TessCoord.xy + vec2(epsilon, 0.0));" \
		
		"   vec3 v1 = normalize(p2.xyz - p1.xyz);" \
		"   vec3 v2 = normalize(p3.xyz - p1.xyz);" \

		"   tes_out.N = cross(v1, v2);" \

		"	gl_Position = u_p_matrix * vec4(gl_TessCoord, 1.0);" \
		"}";

	// attach source code to fragment shader
	glShaderSource(tessellationEvaluationShaderObjcet, 1, (const GLchar **)&tessellationEvaluationShaderSourceCode, NULL);

	// compile fragment shader source code
	glCompileShader(tessellationEvaluationShaderObjcet);

	// compile errors
	iShaderCompileStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;

	glGetShaderiv(tessellationEvaluationShaderObjcet, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(tessellationEvaluationShaderObjcet, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(tessellationEvaluationShaderObjcet, GL_INFO_LOG_LENGTH, &written, szInfoLog);

				fprintf(gpFile, "Tessellation evaluation Shader Compiler Info Log: %s", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(ghWnd);
			}
		}
	}

	// create fragment shader object
	fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	// fragment shader source code
	const GLchar *fragmentShaderSourceCode = (GLchar *)
		"#version 450 core" \
		"\n" \
		
		"out vec4 FragColor;" \

		"in TES_OUT " \
		"{" \
		"	vec3 N;" \
		"} fs_in;" \

		"void main (void)" \
		"{" \
		"	vec3 N = normalize(fs_in.N);" \

		"	vec4 c = vec4(1.0, -1.0, 0.0, 0.0) * N.z + vec4(0.0, 0.0, 0.0, 1.0);" \
		"	FragColor = clamp(c, vec4(0.0), vec4(1.0));" \
		"	FragColor = vec4(1.0);" \
		"}";

	// attach source code to fragment shader
	glShaderSource(fragmentShaderObject, 1, (const GLchar **)&fragmentShaderSourceCode, NULL);

	// compile fragment shader source code
	glCompileShader(fragmentShaderObject);

	// compile errors
	iShaderCompileStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;

	glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(fragmentShaderObject, GL_INFO_LOG_LENGTH, &written, szInfoLog);

				fprintf(gpFile, "Fragment Shader Compiler Info Log: %s", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(ghWnd);
			}
		}
	}

	// create shader program object 
	gShaderProgramObject = glCreateProgram();

	// attach vertex shader to shader program
	glAttachShader(gShaderProgramObject, vertexShaderObject);

	// attach tessellation control shader to shader program
	glAttachShader(gShaderProgramObject, tessellationControlShaderObjcet);

	// attach tessellation evaluation shader to shader program
	glAttachShader(gShaderProgramObject, tessellationEvaluationShaderObjcet);

	// attach fragment shader to shader program
	glAttachShader(gShaderProgramObject, fragmentShaderObject);

	// pre-linking binding to vertex attribute
	glBindAttribLocation(gShaderProgramObject, AMC_ATTRIBUTE_POSITION, "vPosition");

	// link the shader program
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

				fprintf(gpFile, ("Shader Program Linking Info Log: %s"), szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(ghWnd);
			}
		}
	}

	// post-linking retrieving uniform locations
	mvUniform = glGetUniformLocation(gShaderProgramObject, "u_mv_matrix");
	pUniform = glGetUniformLocation(gShaderProgramObject, "u_p_matrix");

	// vertex array
	float vertices[] = {
		-1.0f,  -1.0f,  0.0f,
		-0.33f, -1.0f,  0.0f,
		 0.33f, -1.0f,  0.0f,
		 1.0f,  -1.0f,  0.0f,

		-1.0f,  -0.33f, 0.0f,
		-0.33f, -0.33f, 0.0f,
		 0.33f, -0.33f, 0.0f,
		 1.0f,  -0.33f, 0.0f,

		-1.0f,   0.33f, 0.0f,
		-0.33f,  0.33f, 0.0f,
		 0.33f,  0.33f, 0.0f,
		 1.0f,   0.33f, 0.0f,

		-1.0f,   1.0f,  0.0f,
		-0.33f,  1.0f,  0.0f,
		 0.33f,  1.0f,  0.0f,
		 1.0f,   1.0f,  0.0f
	};

	for (int i = 0; i < 16; i++)
	{
		float fi = (float)i / 16.0f;
		vertices[2 + (i * 3)] = sinf(0.2f + fi * 0.3f);
	}

	// create vao
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// vertex positions
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	//////////////////////////////////////////////////////////////////////

	// clear the depth buffer
	glClearDepth(1.0f);

	// enable depth
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// face culling
	// glEnable(GL_CULL_FACE);
	// glCullFace(GL_FRONT);

	glPatchParameteri(GL_PATCH_VERTICES, 16);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// clear the screen by OpenGL
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	perspectiveProjectionMatrix = mat4::identity();

	// warm-up call to resize
	resize(WIN_WIDTH, WIN_HEIGHT);

	return(0);
}

void resize(int width, int height)
{
	if (height == 0)
	{
		height = 1;
	}

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	perspectiveProjectionMatrix = perspective(50.0, (float)width / (float)height, 1.0f, 1000.0f);
}


void display(void)
{
	static float t = 0.0f;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// use shader program
	glUseProgram(gShaderProgramObject);

	//declaration of matrices
	mat4 modelViewMatrix;

	///// RECTANGLE   ////////////////////////////////////////////////////////////////////////

	// intialize above matrices to identity
	modelViewMatrix = mat4::identity();

	// perform necessary transformations	
	modelViewMatrix = translate(0.0f, 0.0f, -4.0f);/*
		*rotate(t * 10.0f, 0.0f, 1.0f, 0.0f) 
		*rotate(t * 17.0f, 1.0f, 0.0f, 0.0f);*/

	// send necessary matrices to shader in respective uniforms
	glUniformMatrix4fv(mvUniform, 1, GL_FALSE, modelViewMatrix);
	glUniformMatrix4fv(pUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

	// bind with vao (this will avoid many binding to vbo_vertex)
	glBindVertexArray(vao);

	// vertex array
	float vertices[] = {
		-1.0f,  -1.0f,  0.0f,
		-0.33f, -1.0f,  0.0f,
		 0.33f, -1.0f,  0.0f,
		 1.0f,  -1.0f,  0.0f,

		-1.0f,  -0.33f, 0.0f,
		-0.33f, -0.33f, 0.0f,
		 0.33f, -0.33f, 0.0f,
		 1.0f,  -0.33f, 0.0f,

		-1.0f,   0.33f, 0.0f,
		-0.33f,  0.33f, 0.0f,
		 0.33f,  0.33f, 0.0f,
		 1.0f,   0.33f, 0.0f,

		-1.0f,   1.0f,  0.0f,
		-0.33f,  1.0f,  0.0f,
		 0.33f,  1.0f,  0.0f,
		 1.0f,   1.0f,  0.0f
	};

	for (int i = 0; i < 16; i++)
	{
		float fi = (float)i / 16.0f;
		vertices[2 + (i * 3)] = sinf(t * (0.2f + fi * 0.3f));
	}
	
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

	glPatchParameteri(GL_PATCH_VERTICES, 16);
	glDrawArrays(GL_PATCHES, 0, 16);

	// unbind vao
	glBindVertexArray(0);

	// unuse program
	glUseProgram(0);

	SwapBuffers(ghDC);
	t += 0.01f;
}

void update(void)
{
	

}

void uninitialize(void)
{
	
	if (vbo)
	{
		glDeleteBuffers(1, &vbo);
		vbo= 0;
	}

	if (vao)
	{
		glDeleteVertexArrays(1, &vao);
		vao = 0;
	}

	if (gShaderProgramObject)
	{
		GLsizei shaderCount;
		GLsizei shaderNumber;

		glUseProgram(gShaderProgramObject);
		glGetProgramiv(gShaderProgramObject, GL_ATTACHED_SHADERS, &shaderCount);

		GLuint *pShaders = (GLuint *)malloc(sizeof(GLuint) * shaderCount);
		if (pShaders)
		{
			glGetAttachedShaders(gShaderProgramObject, shaderCount, &shaderCount, pShaders);

			for (shaderNumber = 0; shaderNumber < shaderCount; shaderNumber++)
			{
				// detach shader
				glDetachShader(gShaderProgramObject, pShaders[shaderNumber]);

				// delete shader
				glDeleteShader(pShaders[shaderNumber]);
				pShaders[shaderNumber] = 0;
			}
			free(pShaders);
		}

		glDeleteProgram(gShaderProgramObject);
		gShaderProgramObject = 0;
		glUseProgram(0);

	}

	// fullscreen check
	if (gbIsFullScreen == true)
	{
		SetWindowLong(ghWnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghWnd, &wpPrev);
		SetWindowPos(ghWnd,
			HWND_TOP,
			0,
			0,
			0,
			0,
			SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER);

		ShowCursor(TRUE);
	}

	// break the current context
	if (wglGetCurrentContext() == ghRC)
	{
		wglMakeCurrent(NULL, NULL);
	}

	if (ghRC)
	{
		wglDeleteContext(ghRC);
	}

	if (ghDC)
	{
		ReleaseDC(ghWnd, ghDC);
		ghDC = NULL;
	}

	if (gpFile)
	{
		fprintf(gpFile, "Log file is closed...\n");
		fclose(gpFile);
		gpFile = NULL;
	}
}

