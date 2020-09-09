// Headers
#include <Windows.h>
#include <stdio.h>

#include <GL/glew.h>
#include <gl/GL.h>

#include <cuda_gl_interop.h>
#include <cuda_runtime.h>

#include "vmath.h"
#include "resource.h"

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
const int gMeshWidth = 6 * 8;
const int gMeshHeight = 6 * 8;
const int gMeshTotal = gMeshWidth * gMeshHeight;

#define MY_ARRAY_SIZE gMeshWidth*gMeshHeight*4

float4 pos[gMeshTotal] = { 0 };
float4 pos1[gMeshTotal] = { 0 };
float4 vel[gMeshTotal] = { 0 };
float4 vel1[gMeshTotal] = { 0 };

FILE  *gpFile = NULL;
bool  gbActiveWindow = false;
bool  gbIsFullScreen = false;
HDC   ghDC = NULL;
HGLRC ghRC = NULL;
HWND  ghWnd = NULL;
DWORD dwStyle;

WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };

GLuint gShaderProgramObject;
struct cudaGraphicsResource *graphicsResource[5] = { 0 };
GLuint vao;
GLuint vbo;
GLuint vbo_norm;
GLuint vbo_gpu[6];
GLuint vbo_index;
GLuint texCloths[2];
float animationTime = 0.0f;
bool bOnGPU = true;
bool bWind = false;
cudaError_t error;
bool bAnimation = true;
GLuint mvpUniform;
mat4 perspectiveProjectionMatrix;


/* helper functions for float3 */
__host__ __device__ float3 operator+(const float3 &a, const float3 &b)
{
	return make_float3(a.x + b.x, a.y + b.y, a.z + b.z);
}

__host__ __device__ float3 operator-(const float3 &a, const float3 &b)
{
	return make_float3(a.x - b.x, a.y - b.y, a.z - b.z);
}

__host__ __device__ float3 operator*(const float3 &a, float b)
{
	return make_float3(a.x*b, a.y*b, a.z*b);
}

__host__ __device__ float3 operator*(float b, const float3 &a)
{
	return make_float3(a.x*b, a.y*b, a.z*b);
}

__host__ __device__ float3 operator/(const float3 &a, float b)
{
	return make_float3(a.x / b, a.y / b, a.z / b);
}

__host__ __device__ float3 operator/(float b, const float3 &a)
{
	return make_float3(a.x / b, a.y / b, a.z / b);
}

__host__ __device__ float length(const float3 &a)
{
	return sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
}

__host__ __device__ float3 normalize(const float3 &a)
{
	return a / length(a);
}

__host__ __device__ float3 cross(const float3 &a, const float3 &b)
{
	return make_float3(
		(a.y*b.z - a.z*b.y),
		(-(a.x*b.z - a.z*b.x)),
		(a.x*b.y - a.y*b.x)
	);
}

__host__ __device__ float3 make_float3(const float4 &b)
{
	return make_float3(b.x, b.y, b.z);
}



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
			if (!bOnGPU)
			{
				// copy data from vertex buffer of CPU to vertex buffer of GPU
				glBindBuffer(GL_COPY_READ_BUFFER, vbo);
				glBindBuffer(GL_COPY_WRITE_BUFFER, vbo_gpu[0]);

				glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, gMeshTotal * sizeof(float));

				glBindBuffer(GL_COPY_READ_BUFFER, 0);
				glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
				bOnGPU = true;
			}
			break;

		case 'W':
		case 'w':
			bWind = !bWind;
			break;

		case 'C':
		case 'c':
			if (bOnGPU)
			{
				glBindVertexArray(vao);
				glBindBuffer(GL_ARRAY_BUFFER, vbo_gpu[0]);

				vec4* p = (vec4*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
				memcpy_s(pos, gMeshTotal * sizeof(float4), p, gMeshTotal * sizeof(float4));
				glUnmapBuffer(GL_ARRAY_BUFFER);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindVertexArray(0);
				bOnGPU = false;
			}
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
	BOOL loadTexture(GLuint*, TCHAR[]);


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

		"   FragColor = vec4(phong_ads_light, 1.0) * texture(u_sampler, out_Texcoord);" \
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
	glBufferData(GL_ARRAY_BUFFER, MY_ARRAY_SIZE * sizeof(float), initial_positions, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &vbo_norm);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_norm);
	glBufferData(GL_ARRAY_BUFFER, gMeshTotal * 3 * sizeof(float), initial_velocities, GL_DYNAMIC_DRAW);

	// vertex positions
	glGenBuffers(6, vbo_gpu);

	// pos1 and pos2
	for (int i = 0; i < 2; i++)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo_gpu[i]);
		glBufferData(GL_ARRAY_BUFFER, MY_ARRAY_SIZE * sizeof(float), initial_positions, GL_DYNAMIC_DRAW);

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
	for (int i = 2; i < 4; i++)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo_gpu[i]);
		glBufferData(GL_ARRAY_BUFFER, MY_ARRAY_SIZE * sizeof(float), initial_velocities, GL_DYNAMIC_DRAW);

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
	glBufferData(GL_ARRAY_BUFFER, MY_ARRAY_SIZE * sizeof(float), initial_normals, GL_DYNAMIC_DRAW);

	// register our vbo with cuda graphics resource
	error = cudaGraphicsGLRegisterBuffer(&graphicsResource[4], vbo_gpu[4], cudaGraphicsMapFlagsWriteDiscard);
	if (error != cudaSuccess)
	{
		fprintf(gpFile, "cudaGraphicsGLRegisterBuffer failed..\n");
		uninitialize();
		DestroyWindow(ghWnd);
	}

	// texcoords
	glBindBuffer(GL_ARRAY_BUFFER, vbo_gpu[5]);
	glBufferData(GL_ARRAY_BUFFER, MY_ARRAY_SIZE * sizeof(float), initial_texcoords, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_TEXCOORD0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXCOORD0);

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

	delete[]initial_positions;
	delete[]initial_velocities;
	delete[]initial_normals;
	delete[]initial_texcoords;

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

	// textures
	glEnable(GL_TEXTURE_2D);
	loadTexture(&texCloths[0], MAKEINTRESOURCE(IDBITMAP_CLOTH1));
	loadTexture(&texCloths[1], MAKEINTRESOURCE(IDBITMAP_CLOTH2));

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
	void launchCUDAKernel(float4 *, float4 *, float4 *, float4 *, unsigned int, unsigned int, float3 *, float3, float);
	void launchCPUKernel(unsigned int, unsigned int, float3);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// use shader program
	glUseProgram(gShaderProgramObject);

	mat4 mMatrix = mat4::identity();
	//mMatrix *= rotate(0.0f, 100.0f*sinf(t), 0.0f);

	mat4 vMatrix = mat4::identity();
	static float angle = 0.0f;
	angle += 0.005f;
	vMatrix *= lookat(
		vec3(60.0f*sinf(angle), 0.0f, 60.0f*cosf(angle)),
		vec3(0.0f, 0.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f));

	glUniformMatrix4fv(glGetUniformLocation(gShaderProgramObject, "u_m_matrix"), 1, GL_FALSE, mMatrix);
	glUniformMatrix4fv(glGetUniformLocation(gShaderProgramObject, "u_v_matrix"), 1, GL_FALSE, vMatrix);
	glUniformMatrix4fv(glGetUniformLocation(gShaderProgramObject, "u_p_matrix"), 1, GL_FALSE, perspectiveProjectionMatrix);

	float3 wind = make_float3(0.0f, 0.0f, 0.0f);
	if (bWind) wind = make_float3(5.0f, 0.0f, 0.0f);


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
		float3 *norm = NULL;

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
		static float xOffset = 0.0f;
		launchCUDAKernel(ppos1, ppos2, pvel1, pvel2, gMeshWidth, gMeshHeight, norm, wind, xOffset);
		xOffset += 0.01f;

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
		launchCPUKernel(gMeshWidth, gMeshHeight, wind);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, MY_ARRAY_SIZE * sizeof(float), pos, GL_DYNAMIC_DRAW);
	}

	// bind to the respective buffer
	if (bOnGPU) glBindBuffer(GL_ARRAY_BUFFER, vbo_gpu[0]);
	else glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	if (bOnGPU) glBindBuffer(GL_ARRAY_BUFFER, vbo_gpu[4]);
	else glBindBuffer(GL_ARRAY_BUFFER, vbo_norm);

	glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texCloths[1]);

	int lines = (gMeshWidth * (gMeshHeight - 1)) + gMeshWidth;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_index);

	// draw now!
	// front side
	glUniform1f(glGetUniformLocation(gShaderProgramObject, "front"), -1.0f);
	glCullFace(GL_BACK);
	glDrawElements(GL_TRIANGLE_STRIP, lines * 2, GL_UNSIGNED_INT, NULL);

	// back side
	glUniform1f(glGetUniformLocation(gShaderProgramObject, "front"), -1.0f);
	glCullFace(GL_FRONT);
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
		for (int i = 0; i < 4; i++)
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

	for (int i = 0; i < 5; i++)
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

void launchCPUKernel(unsigned int width, unsigned int height, float3 wind)
{
	vec3 make_vec3(float4);
	vec3 make_vec3(float *);

	const float m = 1.0f;
	const float t = 0.000005 * 4;
	const float k = 6000.0;
	const float c = 0.95;
	const float rest_length = 1.00;
	const float rest_length_diag = 1.41;

	// latest position in global pos

	float4 *ppos1 = pos;
	float4 *ppos2 = pos1;
	float4 *pvel1 = vel;
	float4 *pvel2 = vel1;

	for (int count = 0; count < 1000; count++)
	{
		for (unsigned int x = 0; x < width; x++)
		{
			for (unsigned int y = 0; y < height; y++)
			{
				unsigned int idx = (y*width) + x;
				float3 p = make_float3(pos1[idx].x, pos1[idx].y, pos1[idx].z);
				float3 u = make_float3(vel1[idx].x, vel1[idx].y, vel1[idx].z);
				float3 F = make_float3(0.0f, -10.0f, 0.0f) * m - c * u;
				int i = 0;

				F = F + wind;

				if (true) // (vel1[idx].w >= 0.0f)
				{
					// calculate 8 connections
					// up
					if (y < height - 1)
					{
						i = idx + width;
						float3 q = make_float3(pos1[i].x, pos1[i].y, pos1[i].z);
						float3 d = q - p;
						float x = length(d);
						F = F + -k * (rest_length - x) * normalize(d);
					}
					// down
					if (y > 0)
					{
						i = idx - width;
						float3 q = make_float3(pos1[i].x, pos1[i].y, pos1[i].z);
						float3 d = q - p;
						float x = length(d);
						F = F + -k * (rest_length - x) * normalize(d);
					}
					// left
					if (x > 0)
					{
						i = idx - 1;
						float3 q = make_float3(pos1[i].x, pos1[i].y, pos1[i].z);
						float3 d = q - p;
						float x = length(d);
						F = F + -k * (rest_length - x) * normalize(d);
					}
					// right
					if (x < width - 1)
					{
						i = idx + 1;
						float3 q = make_float3(pos1[i].x, pos1[i].y, pos1[i].z);
						float3 d = q - p;
						float x = length(d);
						F = F + -k * (rest_length - x) * normalize(d);
					}

					// lower left
					if (x > 0 && y > 0)
					{
						i = idx - 1 - width;
						float3 q = make_float3(pos1[i].x, pos1[i].y, pos1[i].z);
						float3 d = q - p;
						float x = length(d);
						F = F + -k * (rest_length_diag - x) * normalize(d);
					}
					// upper right
					if (x < (width - 1) && y < (height - 1))
					{
						i = idx + 1 + width;
						float3 q = make_float3(pos1[i].x, pos1[i].y, pos1[i].z);
						float3 d = q - p;
						float x = length(d);
						F = F + -k * (rest_length_diag - x) * normalize(d);
					}
					// lower right
					if (x < (width - 1) && y > 0)
					{
						i = idx + 1 - width;
						float3 q = make_float3(pos1[i].x, pos1[i].y, pos1[i].z);
						float3 d = q - p;
						float x = length(d);
						F = F + -k * (rest_length_diag - x) * normalize(d);
					}
					// upper left
					if (x > 0 && y < (height - 1))
					{
						i = idx - 1 + width;
						float3 q = make_float3(pos1[i].x, pos1[i].y, pos1[i].z);
						float3 d = q - p;
						float x = length(d);
						F = F + -k * (rest_length_diag - x) * normalize(d);
					}

				}
				else
				{
					F = make_float3(0.0f, 0.0f, 0.0f);
				}

				// self collision!
				//int nbrs[] = {idx+width,idx-width,idx-1,idx+1,idx-1-width,idx+1+width,idx+1-width,idx-1+width};


				float3 a = F / m;
				float3 s = u * t + 0.5f * a * t * t;
				float3 v = u + a * t;


				// else if (vec3(p+s).y <= -4.0 && abs(vec3(p+s).x) < 5.5 && abs(vec3(p+s).z) < 5.5)
				// {	
				// 	s = vec3(0.0);
				// 	v = vec3(0.0);
				// }	


				// float force = length(F);
				// for(int i = 0; i < width*height && i!=idx; i++)
				// {
				// 	float3 q = make_float3(pos1[i].x, pos1[i].y, pos1[i].z);
				// 	float3 d = q - pos;
				// 	if(length(d) < 0.4)
				// 		v = v-force*normalize(d);

				// }

				// if (pos.y <= -2.0 && abs(pos.x) < 10.5 && abs(pos.z) < 10.5)
				// {	
				// 	pos = p;
				// 	v = make_float3(0.0f, 0.0f, 0.0f);
				// }
				// else 



				float3 op = p - make_float3(-15.0f, -4.0f, -15.0f);
				float lop = length(op);
				if (lop < 8.0)
				{
					s.y = 0.0f;
					v.y = 0.0f;
				}

				op = p - make_float3(15.0f, -4.0f, -15.0f);
				lop = length(op);
				if (lop < 8.0)
				{
					s.y = 0.0f;
					v.y = 0.0f;
				}

				op = p - make_float3(-15.0f, -4.0f, 15.0f);
				lop = length(op);
				if (lop < 8.0)
				{
					s.y = 0.0f;
					v.y = 0.0f;
				}

				op = p - make_float3(15.0f, -4.0f, 15.0f);
				lop = length(op);
				if (lop < 8.0)
				{
					s.y = 0.0f;
					v.y = 0.0f;
				}


				// if (p.y <= -4.0 && abs(p.x) < 15.5 && abs(p.z) < 15.5)
				// {
				// 	s.y = 0.0f;
				// 	v.y = 0.0f;
				// }



				float3 pos = p + s;

				ppos2[idx] = make_float4(pos.x, pos.y, pos.z, 1.0f);
				pvel2[idx] = make_float4(v.x, v.y, v.z, vel1[idx].w);
			}
		}
	}

	fprintf(gpFile, "\nCalculating normals!");

	// normals
	float3 *norm = new float3[gMeshTotal];
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			unsigned int idx = (y*width) + x;

			float3 p = make_float3(pos[idx].x, pos[idx].y, pos[idx].z);
			float3 n = make_float3(0.0f, 0.0f, 0.0f);
			float3 a, b, c;

			if (y < height - 1)
			{
				c = make_float3(pos[idx + width]) - p;
				if (x < width - 1)
				{
					a = make_float3(pos[idx + 1]) - p;
					b = make_float3(pos[idx + width + 1]) - p;
					n = n + cross(a, b);
					n = n + cross(b, c);
				}
				if (x > 0)
				{
					a = c;
					b = make_float3(pos[idx + width - 1]) - p;
					c = make_float3(pos[idx - 1]) - p;
					n = n + cross(a, b);
					n = n + cross(b, c);
				}
			}

			if (y > 0)
			{
				c = make_float3(pos[idx - width]) - p;
				if (x > 0)
				{
					a = make_float3(pos[idx - 1]) - p;
					b = make_float3(pos[idx - width - 1]) - p;
					n = n + cross(a, b);
					n = n + cross(b, c);
				}
				if (x < width - 1)
				{
					a = c;
					b = make_float3(pos[idx - width + 1]) - p;
					c = make_float3(pos[idx + 1]) - p;
					n = n + cross(a, b);
					n = n + cross(b, c);
				}
			}

			norm[idx] = n;
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbo_norm);
	glBufferData(GL_ARRAY_BUFFER, gMeshTotal * 3 * sizeof(float), norm, GL_DYNAMIC_DRAW);

	delete[]norm;
}

// Convert image resource to image data
BOOL loadTexture(GLuint* texture, TCHAR imageResourceID[])
{
	// variables
	HBITMAP hBitmap = NULL;
	BITMAP bmp;
	BOOL bStatus = false;

	// data
	hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL),
		imageResourceID,
		IMAGE_BITMAP,
		0, 0,
		LR_CREATEDIBSECTION
	);

	if (hBitmap)
	{
		bStatus = TRUE;
		GetObject(hBitmap, sizeof(BITMAP), &bmp);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glGenTextures(1, texture);
		glBindTexture(GL_TEXTURE_2D, *texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bmp.bmWidth, bmp.bmHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, bmp.bmBits);
		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);

		DeleteObject(hBitmap);
	}

	return bStatus;
}


// cloth update
__global__ void cloth_kernel(float4 *pos1, float4 *pos2, float4 *vel1, float4 *vel2, unsigned int width, unsigned int height, float3 wind, float xOffset)
{
	unsigned int x = (blockIdx.x*blockDim.x) + threadIdx.x;
	unsigned int y = (blockIdx.y*blockDim.y) + threadIdx.y;

	unsigned int idx = (y*width) + x;

	if (idx >= width * height) return;

	const float m = 1.0f;
	const float t = 0.000005 * 4;
	const float k = 6000.0;
	const float c = 0.95;
	const float rest_length = 1.00;
	const float rest_length_diag = 1.41;

	float3 p = make_float3(pos1[idx].x, pos1[idx].y, pos1[idx].z);
	float3 u = make_float3(vel1[idx].x, vel1[idx].y, vel1[idx].z);
	float3 F = make_float3(0.0f, -10.0f, 0.0f) * m - c * u;
	int i = 0;

	F = F + wind;

	if (true) // (vel1[idx].w >= 0.0f)
	{
		// calculate 8 connections
		// up
		if (y < height - 1)
		{
			i = idx + width;
			float3 q = make_float3(pos1[i].x, pos1[i].y, pos1[i].z);
			float3 d = q - p;
			float x = length(d);
			F = F + -k * (rest_length - x) * normalize(d);
		}
		// down
		if (y > 0)
		{
			i = idx - width;
			float3 q = make_float3(pos1[i].x, pos1[i].y, pos1[i].z);
			float3 d = q - p;
			float x = length(d);
			F = F + -k * (rest_length - x) * normalize(d);
		}
		// left
		if (x > 0)
		{
			i = idx - 1;
			float3 q = make_float3(pos1[i].x, pos1[i].y, pos1[i].z);
			float3 d = q - p;
			float x = length(d);
			F = F + -k * (rest_length - x) * normalize(d);
		}
		// right
		if (x < width - 1)
		{
			i = idx + 1;
			float3 q = make_float3(pos1[i].x, pos1[i].y, pos1[i].z);
			float3 d = q - p;
			float x = length(d);
			F = F + -k * (rest_length - x) * normalize(d);
		}

		// lower left
		if (x > 0 && y > 0)
		{
			i = idx - 1 - width;
			float3 q = make_float3(pos1[i].x, pos1[i].y, pos1[i].z);
			float3 d = q - p;
			float x = length(d);
			F = F + -k * (rest_length_diag - x) * normalize(d);
		}
		// upper right
		if (x < (width - 1) && y < (height - 1))
		{
			i = idx + 1 + width;
			float3 q = make_float3(pos1[i].x, pos1[i].y, pos1[i].z);
			float3 d = q - p;
			float x = length(d);
			F = F + -k * (rest_length_diag - x) * normalize(d);
		}
		// lower right
		if (x < (width - 1) && y > 0)
		{
			i = idx + 1 - width;
			float3 q = make_float3(pos1[i].x, pos1[i].y, pos1[i].z);
			float3 d = q - p;
			float x = length(d);
			F = F + -k * (rest_length_diag - x) * normalize(d);
		}
		// upper left
		if (x > 0 && y < (height - 1))
		{
			i = idx - 1 + width;
			float3 q = make_float3(pos1[i].x, pos1[i].y, pos1[i].z);
			float3 d = q - p;
			float x = length(d);
			F = F + -k * (rest_length_diag - x) * normalize(d);
		}

	}
	else
	{
		F = make_float3(0.0f, 0.0f, 0.0f);
	}

	// self collision!
	//int nbrs[] = {idx+width,idx-width,idx-1,idx+1,idx-1-width,idx+1+width,idx+1-width,idx-1+width};


	float3 a = F / m;
	float3 s = u * t + 0.5f * a * t * t;
	float3 v = u + a * t;


	// else if (vec3(p+s).y <= -4.0 && abs(vec3(p+s).x) < 5.5 && abs(vec3(p+s).z) < 5.5)
	// {	
	// 	s = vec3(0.0);
	// 	v = vec3(0.0);
	// }	


	// float force = length(F);
	// for(int i = 0; i < width*height && i!=idx; i++)
	// {
	// 	float3 q = make_float3(pos1[i].x, pos1[i].y, pos1[i].z);
	// 	float3 d = q - pos;
	// 	if(length(d) < 0.4)
	// 		v = v-force*normalize(d);

	// }

	// if (pos.y <= -2.0 && abs(pos.x) < 10.5 && abs(pos.z) < 10.5)
	// {	
	// 	pos = p;
	// 	v = make_float3(0.0f, 0.0f, 0.0f);
	// }
	// else 



	float3 op = p - make_float3(-15.0f, -4.0f, -15.0f);
	float lop = length(op);
	if (lop < 8.0)
	{
		s.y = 0.0f;
		v.y = 0.0f;
	}

	op = p - make_float3(15.0f, -4.0f, -15.0f);
	lop = length(op);
	if (lop < 8.0)
	{
		s.y = 0.0f;
		v.y = 0.0f;
	}

	op = p - make_float3(-15.0f, -4.0f, 15.0f);
	lop = length(op);
	if (lop < 8.0)
	{
		s.y = 0.0f;
		v.y = 0.0f;
	}

	op = p - make_float3(15.0f, -4.0f, 15.0f);
	lop = length(op);
	if (lop < 8.0)
	{
		s.y = 0.0f;
		v.y = 0.0f;
	}


	// if (p.y <= -4.0 && abs(p.x) < 15.5 && abs(p.z) < 15.5)
	// {
	// 	s.y = 0.0f;
	// 	v.y = 0.0f;
	// }



	float3 pos = p + s;

	pos2[idx] = make_float4(pos.x, pos.y, pos.z, 1.0f);
	vel2[idx] = make_float4(v.x, v.y, v.z, vel1[idx].w);

	return;
}

__global__ void cloth_normals(float4 *pos, float3 *norm, unsigned int width, unsigned int height)
{
	unsigned int x = (blockIdx.x*blockDim.x) + threadIdx.x;
	unsigned int y = (blockIdx.y*blockDim.y) + threadIdx.y;

	unsigned int idx = (y*width) + x;

	if (idx >= width * height) return;

	float3 p = make_float3(pos[idx].x, pos[idx].y, pos[idx].z);
	float3 n = make_float3(0.0f, 0.0f, 0.0f);
	float3 a, b, c;

	if (y < height - 1)
	{
		c = make_float3(pos[idx + width]) - p;
		if (x < width - 1)
		{
			a = make_float3(pos[idx + 1]) - p;
			b = make_float3(pos[idx + width + 1]) - p;
			n = n + cross(a, b);
			n = n + cross(b, c);
		}
		if (x > 0)
		{
			a = c;
			b = make_float3(pos[idx + width - 1]) - p;
			c = make_float3(pos[idx - 1]) - p;
			n = n + cross(a, b);
			n = n + cross(b, c);
		}
	}

	if (y > 0)
	{
		c = make_float3(pos[idx - width]) - p;
		if (x > 0)
		{
			a = make_float3(pos[idx - 1]) - p;
			b = make_float3(pos[idx - width - 1]) - p;
			n = n + cross(a, b);
			n = n + cross(b, c);
		}
		if (x < width - 1)
		{
			a = c;
			b = make_float3(pos[idx - width + 1]) - p;
			c = make_float3(pos[idx + 1]) - p;
			n = n + cross(a, b);
			n = n + cross(b, c);
		}
	}

	norm[idx] = n;
}

void launchCUDAKernel(float4 *pos1, float4 *pos2, float4 *vel1, float4 *vel2, unsigned int meshWidth, unsigned int meshHeight, float3 *norm, float3 wind, float xOffset)
{
	dim3 block(16, 16, 1);
	dim3 grid(meshWidth / block.x, meshHeight / block.y, 1);

	for (int i = 0; i < 500; i++)
	{
		cloth_kernel << <grid, block >> > (pos1, pos2, vel1, vel2, meshWidth, meshHeight, wind, xOffset);
		//cudaDeviceSynchronize();
		cloth_kernel << <grid, block >> > (pos2, pos1, vel2, vel1, meshWidth, meshHeight, wind, xOffset);
		//cudaDeviceSynchronize();

	}
	//cudaDeviceSynchronize();
	cloth_normals << <grid, block >> > (pos1, norm, meshWidth, meshHeight);
}


