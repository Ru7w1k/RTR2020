// Headers
#include <Windows.h>
#include <stdio.h>

#include <GL/glew.h>
#include <gl/GL.h>

#include <cuda_gl_interop.h>
#include <cuda_runtime.h>

#include "vmath.h"

// Linker Options
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "cudart.lib")

// Defines
#define WIN_WIDTH  800
#define WIN_HEIGHT 600

#define PRIMITIVE_RESTART 0xffffff

using namespace vmath;

enum {
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLOR, 
	AMC_ATTRIBUTE_NORMAL,
	AMC_ATTRIBUTE_TEXCOORD0
};

// Global Variables
const int gMeshWidth = 6*8;
const int gMeshHeight = 6*8;
const int gMeshTotal = gMeshWidth*gMeshHeight;

#define MY_ARRAY_SIZE gMeshWidth*gMeshHeight*4

FILE  *gpFile = NULL;
bool  gbActiveWindow = false;
bool  gbIsFullScreen = false;
HDC   ghDC = NULL;
HGLRC ghRC = NULL;
HWND  ghWnd = NULL;
DWORD dwStyle;

WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };

GLuint gShaderProgramObject;

float pos1[gMeshWidth][gMeshHeight][4];
float pos2[gMeshWidth][gMeshHeight][4];
float vel1[gMeshWidth][gMeshHeight][4];
float vel2[gMeshWidth][gMeshHeight][4];
struct cudaGraphicsResource *graphicsResource[5] = {0};
GLuint vao;
GLuint vbo;
GLuint vbo_gpu[5];
GLuint vbo_index;
float animationTime = 0.0f;
bool bOnGPU = true;
cudaError_t error;
bool bAnimation = true;
GLuint mvpUniform;
mat4 perspectiveProjectionMatrix;

// Global function declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// WinMain
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	// function declarations
	int initialize(void);
	void display(void);

	// variables 
	bool bDone = false;
	int iRet = 0;
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
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
		TEXT("CUDA OpenGL Interoperability"),
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
			if (gbActiveWindow == true && bAnimation)
			{
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

	case WM_CHAR:
		switch (wParam)
		{
		case 'G':
		case 'g':
			bOnGPU = true;
			break;

		case 'C':
		case 'c':
			bOnGPU = false;
			break;

		case 'A':
		case 'a':
			if (bAnimation == TRUE) {
				bAnimation = FALSE;
			}
			else {
				bAnimation = TRUE;
			}
			break;
		}

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

	//// C U D A /////////////////////////////////////////////////////////

	// cuda initialization
	int devCount;
	error = cudaGetDeviceCount(&devCount);
	if (error != cudaSuccess)
	{
		fprintf(gpFile, "cudaGetDeviceCount failed..\n");
		uninitialize();
		DestroyWindow(ghWnd);
	}
	else if (devCount == 0)
	{
		fprintf(gpFile, "No CUDA device detected..\n");
		uninitialize();
		DestroyWindow(ghWnd);
	}
	else
	{
		error = cudaSetDevice(0);
		if (error != cudaSuccess)
		{
			fprintf(gpFile, "cudaSetDevice failed..\n");
			uninitialize();
			DestroyWindow(ghWnd);
		}
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

		"in vec4 position;" \
		"in vec3 normal;" \
		"in vec2 texcoord;" \

		"uniform float front = 1.0f;" \
		"uniform mat4 u_m_matrix;" \
		"uniform mat4 u_v_matrix;" \
		"uniform mat4 u_p_matrix;" \
		"uniform vec4 u_light_position = vec4(0.0f, 5.0f, 0.0f, 1.0f);" \

		"out vec3 tnorm;" \
		"out vec3 light_direction;" \
		"out vec3 viewer_vector;" \
		"out vec2 out_Texcoord;" \

		"void main()" \
		"{" \

		"   vec4 eye_coordinates = u_v_matrix * u_m_matrix * position;" \
		"   tnorm = mat3(u_v_matrix * u_m_matrix) * normal * front;" \
		"   light_direction = vec3(u_light_position - eye_coordinates);" \
		"   float tn_dot_ldir = max(dot(tnorm, light_direction), 0.0);" \
		"   viewer_vector = vec3(-eye_coordinates.xyz);" \

		"	gl_Position = u_p_matrix * u_v_matrix * u_m_matrix * vec4(position.xyz , 1.0);" \
		"   out_Texcoord = texcoord;" \
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

	// create fragment shader object
	fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	// fragment shader source code
	const GLchar *fragmentShaderSourceCode = (GLchar *)
		"#version 450 core" \
		"\n" \
		
		"in vec3 tnorm;" \
		"in vec3 light_direction;" \
		"in vec3 viewer_vector;" \
		"in vec2 out_Texcoord;" \

		"uniform vec3 u_la = vec3(0.4, 0.4, 0.4);" \
		"uniform vec3 u_ld = vec3(0.8, 0.8, 0.8);" \
		"uniform vec3 u_ls = vec3(1.0, 1.0, 1.0);" \
		"uniform vec3 u_ka = vec3(0.4, 0.4, 0.4);" \
		"uniform vec3 u_kd = vec3(0.8, 0.8, 0.8);" \
		"uniform vec3 u_ks = vec3(1.0, 1.0, 1.0);" \
		"uniform float u_shininess = 25.0;" \

		"uniform sampler2D u_sampler;" \
		
		"out vec4 FragColor;" \

		"void main (void)" \
		"{" \
		"   vec3 ntnorm = normalize(tnorm);" \
		"   vec3 nlight_direction = normalize(light_direction);" \
		"   vec3 nviewer_vector = normalize(viewer_vector);" \
		"   vec3 reflection_vector = reflect(-nlight_direction, ntnorm);" \
		"   float tn_dot_ldir = max(dot(ntnorm, nlight_direction), 0.0);" \
		
		"   vec3 ambient  = u_la * u_ka;" \
		"   vec3 diffuse  = u_ld * u_kd * tn_dot_ldir;" \
		"   vec3 specular = u_ls * u_ks * pow(max(dot(reflection_vector, nviewer_vector), 0.0), u_shininess);" \
		
		"   vec3 phong_ads_light = ambient + diffuse;" \
		
		"   FragColor = vec4(phong_ads_light, 1.0);" \
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

	// attach fragment shader to shader program
	glAttachShader(gShaderProgramObject, fragmentShaderObject);

	// pre-linking binding to vertex attribute
	glBindAttribLocation(gShaderProgramObject, AMC_ATTRIBUTE_POSITION, "position");
	glBindAttribLocation(gShaderProgramObject, AMC_ATTRIBUTE_NORMAL, "normal");
	glBindAttribLocation(gShaderProgramObject, AMC_ATTRIBUTE_TEXCOORD0, "texcoord");

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

	///// cloth mesh coordinates generation ///////////////////////////////////////
	int i, j;

	vec4 *initial_positions = new vec4[gMeshTotal];
	vec4 *initial_velocities = new vec4[gMeshTotal];
	vec3 *initial_normals = new vec3[gMeshTotal];
	vec2 *initial_texcoords = new vec2[gMeshTotal];

	int n = 0;

	for (j = 0; j < gMeshHeight; j++)
	{
		float fj = (float)j / (float)gMeshHeight;
		for (i = 0; i < gMeshWidth; i++)
		{
			float fi = (float)i / (float)gMeshWidth;

			initial_positions[n] = vec4((fi - 0.5f) * (float)gMeshWidth,
				                        10.0f,
				                        (fj - 0.5f) * (float)gMeshHeight,
				                        1.0);

			initial_velocities[n] = vec4(0.0f);
			initial_normals[n] = vec3(0.0f);
	
			// texture coords
			initial_texcoords[n][0] = fi * 5.0f;
			initial_texcoords[n][1] = fj * 5.0f;

			n++;

		}
	}

	// create vao
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// vertex positions
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, MY_ARRAY_SIZE*sizeof(float), NULL, GL_DYNAMIC_DRAW);

	// vertex positions
	glGenBuffers(5, vbo_gpu);
	
	// pos1 and pos2
	for(int i = 0; i < 2; i++)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo_gpu[i]);
		glBufferData(GL_ARRAY_BUFFER, MY_ARRAY_SIZE*sizeof(float), initial_positions, GL_DYNAMIC_DRAW);
	
		// register our vbo with cuda graphics resource
		error = cudaGraphicsGLRegisterBuffer(&graphicsResource[i], vbo_gpu[i], cudaGraphicsMapFlagsWriteDiscard);
		if (error != cudaSuccess)
		{
			fprintf(gpFile, "cudaGraphicsGLRegisterBuffer failed..\n");
			uninitialize();
			DestroyWindow(ghWnd);
		}
	}

	// vel1 and vel2
	for(int i = 2; i < 4; i++)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo_gpu[i]);
		glBufferData(GL_ARRAY_BUFFER, MY_ARRAY_SIZE*sizeof(float), initial_velocities, GL_DYNAMIC_DRAW);
	
		// register our vbo with cuda graphics resource
		error = cudaGraphicsGLRegisterBuffer(&graphicsResource[i], vbo_gpu[i], cudaGraphicsMapFlagsWriteDiscard);
		if (error != cudaSuccess)
		{
			fprintf(gpFile, "cudaGraphicsGLRegisterBuffer failed..\n");
			uninitialize();
			DestroyWindow(ghWnd);
		}
	}

	// normals
	glBindBuffer(GL_ARRAY_BUFFER, vbo_gpu[4]);
	glBufferData(GL_ARRAY_BUFFER, MY_ARRAY_SIZE*sizeof(float), initial_normals, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);
 
	// register our vbo with cuda graphics resource
	error = cudaGraphicsGLRegisterBuffer(&graphicsResource[4], vbo_gpu[4], cudaGraphicsMapFlagsWriteDiscard);
	if (error != cudaSuccess)
	{
		fprintf(gpFile, "cudaGraphicsGLRegisterBuffer failed..\n");
		uninitialize();
		DestroyWindow(ghWnd);
	}


	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// index buffer for cloth mesh
	int lines = (gMeshWidth * (gMeshHeight - 1)) + gMeshWidth;

	glGenBuffers(1, &vbo_index);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_index);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, lines * 2 * sizeof(int), NULL, GL_STATIC_DRAW);

	int* e = (int*)glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, lines * 2 * sizeof(int), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

	// triangle mesh
	for (j = 0; j < gMeshHeight - 1; j++)
	{
		for (i = 0; i < gMeshWidth; i++)
		{
			*e++ = j * gMeshWidth + i;
			*e++ = (1 + j) * gMeshWidth + i;
		}
		*e++ = PRIMITIVE_RESTART;
	}

	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

	//////////////////////////////////////////////////////////////////////

	// clear the depth buffer
	glClearDepth(1.0f);

	// primitive restart
	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(PRIMITIVE_RESTART);

	// enable depth
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// enable blend
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
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

	perspectiveProjectionMatrix = perspective(45.0, (float)width / (float)height, 0.1f, 100.0f);
}


void display(void)
{
	void uninitialize(void);
	void launchCUDAKernel(float4 *, float4 *, float4 *, float4 *, unsigned int, unsigned int, float3 *);
	void launchCPUKernel(unsigned int, unsigned int, float);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// use shader program
	glUseProgram(gShaderProgramObject);

	mat4 mMatrix = mat4::identity();
	//mMatrix *= rotate(0.0f, 100.0f*sinf(t), 0.0f);

	mat4 vMatrix = mat4::identity();
	vMatrix *= lookat(
		vec3(0, 0.0f, 60.0f),
		vec3(0.0f, 0.0f, 0.0f), 
		vec3(0.0f, 1.0f, 0.0f));

	glUniformMatrix4fv(glGetUniformLocation(gShaderProgramObject, "u_m_matrix"), 1, GL_FALSE, mMatrix);
	glUniformMatrix4fv(glGetUniformLocation(gShaderProgramObject, "u_v_matrix"), 1, GL_FALSE, vMatrix);
	glUniformMatrix4fv(glGetUniformLocation(gShaderProgramObject, "u_p_matrix"), 1, GL_FALSE, perspectiveProjectionMatrix);

	glBindVertexArray(vao);

	if (bOnGPU)
	{
		// 1. map with the resource
		error = cudaGraphicsMapResources(1, &graphicsResource[0], 0);
		if (error != cudaSuccess)
		{
			fprintf(gpFile, "cudaGraphicsMapResource 0 failed..\n");
			uninitialize();
			DestroyWindow(ghWnd);
		}

		error = cudaGraphicsMapResources(1, &graphicsResource[1], 0);
		if (error != cudaSuccess)
		{
			fprintf(gpFile, "cudaGraphicsMapResource 1 failed..\n");
			uninitialize();
			DestroyWindow(ghWnd);
		}

		error = cudaGraphicsMapResources(1, &graphicsResource[2], 0);
		if (error != cudaSuccess)
		{
			fprintf(gpFile, "cudaGraphicsMapResource 2 failed..\n");
			uninitialize();
			DestroyWindow(ghWnd);
		}

		error = cudaGraphicsMapResources(1, &graphicsResource[3], 0);
		if (error != cudaSuccess)
		{
			fprintf(gpFile, "cudaGraphicsMapResource 3 failed..\n");
			uninitialize();
			DestroyWindow(ghWnd);
		}

		error = cudaGraphicsMapResources(1, &graphicsResource[4], 0);
		if (error != cudaSuccess)
		{
			fprintf(gpFile, "cudaGraphicsMapResource 4 failed..\n");
			uninitialize();
			DestroyWindow(ghWnd);
		}

		// 2. get pointer to mapped resource
		float4 *ppos1 = NULL;
		float4 *ppos2 = NULL;
		float4 *pvel1 = NULL;
		float4 *pvel2 = NULL;
		float3 *norm  = NULL;

		size_t byteCount;
		error = cudaGraphicsResourceGetMappedPointer((void **)&ppos1, &byteCount, graphicsResource[0]);
		if (error != cudaSuccess)
		{
			fprintf(gpFile, "cudaGraphicsResourceGetMappedPointer ppos1 failed..\n");
			uninitialize();
			DestroyWindow(ghWnd);
		}
		
		error = cudaGraphicsResourceGetMappedPointer((void **)&ppos2, &byteCount, graphicsResource[1]);
		if (error != cudaSuccess)
		{
			fprintf(gpFile, "cudaGraphicsResourceGetMappedPointer ppos2 failed..\n");
			uninitialize();
			DestroyWindow(ghWnd);
		}

		error = cudaGraphicsResourceGetMappedPointer((void **)&pvel1, &byteCount, graphicsResource[2]);
		if (error != cudaSuccess)
		{
			fprintf(gpFile, "cudaGraphicsResourceGetMappedPointer pvel1 failed..\n");
			uninitialize();
			DestroyWindow(ghWnd);
		}

		error = cudaGraphicsResourceGetMappedPointer((void **)&pvel2, &byteCount, graphicsResource[3]);
		if (error != cudaSuccess)
		{
			fprintf(gpFile, "cudaGraphicsResourceGetMappedPointer pvel2 failed..\n");
			uninitialize();
			DestroyWindow(ghWnd);
		}

		error = cudaGraphicsResourceGetMappedPointer((void **)&norm, &byteCount, graphicsResource[4]);
		if (error != cudaSuccess)
		{
			fprintf(gpFile, "cudaGraphicsResourceGetMappedPointer norm failed..\n");
			uninitialize();
			DestroyWindow(ghWnd);
		}


		// 3. launch the CUDA kernel
		launchCUDAKernel(ppos1, ppos2, pvel1, pvel2, gMeshWidth, gMeshHeight, norm);

		// 4. unmap the resource
		error = cudaGraphicsUnmapResources(1, &graphicsResource[0], 0);
		if (error != cudaSuccess)
		{
			fprintf(gpFile, "cudaGraphicsUnmapResources failed..\n");
			uninitialize();
			DestroyWindow(ghWnd);
		}

		error = cudaGraphicsUnmapResources(1, &graphicsResource[1], 0);
		if (error != cudaSuccess)
		{
			fprintf(gpFile, "cudaGraphicsUnmapResources failed..\n");
			uninitialize();
			DestroyWindow(ghWnd);
		}

		error = cudaGraphicsUnmapResources(1, &graphicsResource[2], 0);
		if (error != cudaSuccess)
		{
			fprintf(gpFile, "cudaGraphicsUnmapResources failed..\n");
			uninitialize();
			DestroyWindow(ghWnd);
		}

		error = cudaGraphicsUnmapResources(1, &graphicsResource[3], 0);
		if (error != cudaSuccess)
		{
			fprintf(gpFile, "cudaGraphicsUnmapResources failed..\n");
			uninitialize();
			DestroyWindow(ghWnd);
		}
				
		error = cudaGraphicsUnmapResources(1, &graphicsResource[4], 0);
		if (error != cudaSuccess)
		{
			fprintf(gpFile, "cudaGraphicsUnmapResources failed..\n");
			uninitialize();
			DestroyWindow(ghWnd);
		}

	}
	else
	{
		launchCPUKernel(gMeshWidth, gMeshHeight, animationTime);
		
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, MY_ARRAY_SIZE * sizeof(float), pos1, GL_DYNAMIC_DRAW);
	}

	// bind to the respective buffer
	if (bOnGPU) glBindBuffer(GL_ARRAY_BUFFER, vbo_gpu[0]);
	else glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	int lines = (gMeshWidth * (gMeshHeight - 1)) + gMeshWidth;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_index);

	// front side
	glUniform1f(glGetUniformLocation(gShaderProgramObject, "front"), 1.0f);
	// glCullFace(GL_BACK);
	glDrawElements(GL_TRIANGLE_STRIP, lines * 2, GL_UNSIGNED_INT, NULL);
	
	// back side
	glUniform1f(glGetUniformLocation(gShaderProgramObject, "front"), -1.0f);
	// glCullFace(GL_FRONT);
	glDrawElements(GL_TRIANGLE_STRIP, lines * 2, GL_UNSIGNED_INT, NULL);

	glBindVertexArray(0);
	
	//////////////////////////////////////////////////////////////////////////////////////////

	// unuse program
	glUseProgram(0);

	SwapBuffers(ghDC);
	animationTime += 0.05f;
}

void uninitialize(void)
{
	if (vbo_gpu)
	{
		glDeleteBuffers(4, vbo_gpu);
		for(int i = 0; i < 4; i++)
			vbo_gpu[i] = 0;
	}

	if (vbo)
	{
		glDeleteBuffers(1, &vbo);
		vbo = 0;
	}

	if (vao)
	{
		glDeleteVertexArrays(1, &vao);
		vao = 0;
	}

	for(int i = 0 ; i < 5; i++)
	{
		if (graphicsResource[i])
		{
			cudaGraphicsUnregisterResource(graphicsResource[i]);
			graphicsResource[i] = NULL;
		}
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

void launchCPUKernel(unsigned int meshWidth, unsigned int meshHeight, float time)
{
	for (int i = 0; i < meshWidth; i++) 
	{
		for (int j = 0; j < meshHeight; j++)
		{
			// for (int k = 0; k < 4; k++)
			// {
			// 	float u = i / (float)meshWidth;
			// 	float v = j / (float)meshHeight;

			// 	u = (u * 2.0) - 1.0;
			// 	v = (v * 2.0) - 1.0;

			// 	float freq = 4.0f;
			// 	float w = sinf(freq*u + time) * cosf(freq*v + time) * 0.5f;

			// 	if (k == 0) pos1[i][j][k] = u;
			// 	if (k == 1) pos1[i][j][k] = w;
			// 	if (k == 2) pos1[i][j][k] = v;
			// 	if (k == 3) pos1[i][j][k] = 1.0f;
			// }
		}

	}
}

