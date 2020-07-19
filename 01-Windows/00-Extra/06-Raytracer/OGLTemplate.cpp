// headers
#include <windows.h>
#include <stdio.h>

#include <GL/glew.h>
#include <gl/GL.h>

#include "vmath.h"

#include "OGLTemplate.h"

// shaders
#include "bilt.h"
#include "raytracer.h"
#include "trace-prepaer.h"

// linker options
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "opengl32.lib")

// namespaces
using namespace vmath;

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

int gWidth, gHeight;

DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };

// shaders and uniforms
GLuint gPrepareShaderProgram;
GLuint gTraceShaderProgram;
GLuint gBlitShaderProgram;

struct uniforms_block
{
	mat4 mv_matrix;
	mat4 view_matrix;
	mat4 proj_matrix;
};

GLuint uniforms_buffer;
GLuint sphere_buffer;
GLuint plane_buffer;
GLuint light_buffer;

struct
{
	GLint ray_origin;
	GLint ray_lookat;
	GLint aspect;
} uniforms;

GLuint vao;

struct sphere
{
	vec3  center;
	float radius;
	vec4  color;
};

struct plane
{
	vec3  normal;
	float d;
};

struct light
{
	vec3 position;
	unsigned int : 32; // padding
};

enum
{
	MAX_RECURSION_DEPTH = 5,
	MAX_FB_WIDTH        = 2045,
	MAX_FB_HEIGHT       = 1024
};

enum DEBUG_MODE
{
	DEBUG_NONE,
	DEBUG_REFLECTED,
	DEBUG_REFRACTED,
	DEBUG_REFLECTED_COLOR,
	DEBUG_REFRACTED_COLOR
};

GLuint tex_composite;
GLuint ray_fbo[MAX_RECURSION_DEPTH];
GLuint tex_position[MAX_RECURSION_DEPTH];
GLuint tex_reflected[MAX_RECURSION_DEPTH];
GLuint tex_reflected_intensity[MAX_RECURSION_DEPTH];
GLuint tex_refracted[MAX_RECURSION_DEPTH];
GLuint tex_refracted_intesity[MAX_RECURSION_DEPTH];

int        max_depth = 1;
int        debug_depth;
DEBUG_MODE debug_mode;
bool       paused;



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
	if (fopen_s(&gpFile, "OGLTemplate.log", "w") != 0)
	{
		MessageBox(NULL, TEXT("Cannot open OGLTemplate.log file.."), TEXT("Error"), MB_OK | MB_ICONERROR);
		exit(0);
	}
	else
	{
		fprintf(gpFile, "==== Application Started ====\n");
	}

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
		TEXT("OpenGL | Raytracer"),
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
	void uninitialize(void);
	void CompileShader(GLuint, const char*);
	void LinkProgram(GLuint, const char*);

	// variable declarations
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;
	GLenum result;
	int i;

	// code
	ghdc = GetDC(ghwnd);

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
	pfd.cDepthBits = 24;

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

	// enable GLEW for Programmable Pipeline!
	result = glewInit();
	if (result != GLEW_OK) {
		fprintf(gpFile, "GLEW initialization failed..\n");
		uninitialize();
		DestroyWindow(ghwnd);
	}

	// shaders and shader program
	
	///// Trace Shader Program ///////////////////////////////////////////////////////////////////

	// create vertex shader
	GLuint traceVertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(traceVertexShader, 1, (const GLchar**)&vsTraceShaderSourceCode, NULL);
	CompileShader(traceVertexShader, "Prepare Vertex Shader");

	// create fragment shader
	GLuint traceFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(traceFragmentShader, 1, (const GLchar**)&fsTraceShaderSourceCode, NULL);
	CompileShader(traceFragmentShader, "Prepare Fragment Shader");

	// create program
	gPrepareShaderProgram = glCreateProgram();
	glAttachShader(gPrepareShaderProgram, traceVertexShader);
	glAttachShader(gPrepareShaderProgram, traceFragmentShader);
	LinkProgram(gPrepareShaderProgram, "Prepare");

	// uniforms
	uniforms.ray_origin = glGetUniformLocation(gPrepareShaderProgram, "ray_origin");
	uniforms.ray_lookat = glGetUniformLocation(gPrepareShaderProgram, "ray_lookat");
	uniforms.aspect     = glGetUniformLocation(gPrepareShaderProgram, "aspect");

	///////////////////////////////////////////////////////////////////////////////////////////////

	///// Raytracer Shader Program ///////////////////////////////////////////////////////////////////

	// create vertex shader
	GLuint raytracerVertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(raytracerVertexShader, 1, (const GLchar**)&vsRaytracerSourceCode, NULL);
	CompileShader(raytracerVertexShader, "Trace Vertex Shader");

	// create fragment shader
	GLuint raytracerFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(raytracerFragmentShader, 1, (const GLchar**)&fsRaytracerSourceCode, NULL);
	CompileShader(raytracerFragmentShader, "Trace Fragment Shader");

	// create program
	gTraceShaderProgram = glCreateProgram();
	glAttachShader(gTraceShaderProgram, raytracerVertexShader);
	glAttachShader(gTraceShaderProgram, raytracerFragmentShader);
	LinkProgram(gTraceShaderProgram, "Trace");

	///////////////////////////////////////////////////////////////////////////////////////////////

	///// Blit Shader Program ///////////////////////////////////////////////////////////////////

	// create vertex shader
	GLuint blitVertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(blitVertexShader, 1, (const GLchar**)&vsBlitShaderSourceCode, NULL);
	CompileShader(blitVertexShader, "Blit Vertex Shader");

	// create fragment shader
	GLuint blitFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(blitFragmentShader, 1, (const GLchar**)&fsBlitShaderSourceCode, NULL);
	CompileShader(blitFragmentShader, "Blit Fragment Shader");

	// create program
	gBlitShaderProgram = glCreateProgram();
	glAttachShader(gBlitShaderProgram, blitVertexShader);
	glAttachShader(gBlitShaderProgram, blitFragmentShader);
	LinkProgram(gBlitShaderProgram, "Blit");

	///////////////////////////////////////////////////////////////////////////////////////////////

	////// buffers and textures ///////////////////////////////////////////////////////////////////

	glGenBuffers(1, &uniforms_buffer);
	glBindBuffer(GL_UNIFORM_BUFFER, uniforms_buffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(uniforms_block), NULL, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &sphere_buffer);
	glBindBuffer(GL_UNIFORM_BUFFER, sphere_buffer);
	glBufferData(GL_UNIFORM_BUFFER, 128 * sizeof(sphere), NULL, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &plane_buffer);
	glBindBuffer(GL_UNIFORM_BUFFER, plane_buffer);
	glBufferData(GL_UNIFORM_BUFFER, 128 * sizeof(plane), NULL, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &light_buffer);
	glBindBuffer(GL_UNIFORM_BUFFER, light_buffer);
	glBufferData(GL_UNIFORM_BUFFER, 128 * sizeof(light), NULL, GL_DYNAMIC_DRAW);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glEnable(GL_TEXTURE_2D);

	glGenFramebuffers(MAX_RECURSION_DEPTH, ray_fbo);
	glGenTextures(1, &tex_composite);
	glGenTextures(MAX_RECURSION_DEPTH, tex_position);
	glGenTextures(MAX_RECURSION_DEPTH, tex_reflected);
	glGenTextures(MAX_RECURSION_DEPTH, tex_refracted);
	glGenTextures(MAX_RECURSION_DEPTH, tex_reflected_intensity);
	glGenTextures(MAX_RECURSION_DEPTH, tex_refracted_intesity);

	glBindTexture(GL_TEXTURE_2D, tex_composite);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB16F, MAX_FB_WIDTH, MAX_FB_HEIGHT);

	for (i = 0; i < MAX_RECURSION_DEPTH; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, ray_fbo[i]);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, tex_composite, 0);

		glBindTexture(GL_TEXTURE_2D, tex_position[i]);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, MAX_FB_WIDTH, MAX_FB_HEIGHT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, tex_position[i], 0);

		glBindTexture(GL_TEXTURE_2D, tex_reflected[i]);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB16F, MAX_FB_WIDTH, MAX_FB_HEIGHT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, tex_reflected[i], 0);

		glBindTexture(GL_TEXTURE_2D, tex_refracted[i]);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB16F, MAX_FB_WIDTH, MAX_FB_HEIGHT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, tex_refracted[i], 0);

		glBindTexture(GL_TEXTURE_2D, tex_reflected_intensity[i]);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB16F, MAX_FB_WIDTH, MAX_FB_HEIGHT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, tex_reflected_intensity[i], 0);

		glBindTexture(GL_TEXTURE_2D, tex_refracted_intesity[i]);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB16F, MAX_FB_WIDTH, MAX_FB_HEIGHT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, tex_refracted_intesity[i], 0);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	///////////////////////////////////////////////////////////////////////////////////////////////

	// set clear color
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

	// warm-up resize call
	resize(WIN_WIDTH, WIN_HEIGHT);
}

void resize(int width, int height)
{
	// code
	if (height == 0)
		height = 1;

	gWidth = width;
	gHeight = height;

	/*glViewport(0, 0, (GLsizei)width, (GLsizei)height);*/
}

void display(void)
{
	// function declarations
	void recurse(int);

	// variables
	static const GLfloat zeros[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	static const GLfloat gray[] = { 0.1f, 0.1f, 0.1f, 0.1f };
	static const GLfloat ones[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	static LARGE_INTEGER last_time = { 0 };
	static LARGE_INTEGER total_time = { 0 };
	
	LARGE_INTEGER current_time;
	QueryPerformanceCounter(&current_time);

	// code
	// glClear(GL_COLOR_BUFFER_BIT);
	if (!paused)
		total_time.QuadPart += (current_time.QuadPart - last_time.QuadPart);
	last_time.QuadPart = current_time.QuadPart;

	// float f = (float)total_time.QuadPart;
	static float f = 0.0f;
	f += 0.01f;
	fprintf(gpFile, "Display() with f = %g\n", f);

	vec3 view_position = vec3(sinf(f * 0.3234f) * 28.0f, cosf(f * 0.4234f) * 28.0f, cosf(f * 0.1234f) * 28.0f);
	vec3 lookat_point = vec3(sinf(f * 0.214f) * 8.0f, cosf(f * 0.153f) * 8.0f, sinf(f * 0.734f) * 8.0f);
	mat4 view_matrix = lookat(view_position, lookat_point, vec3(0.0f, 1.0f, 0.0f));

	glBindBufferBase(GL_UNIFORM_BUFFER, 0, uniforms_buffer);
	uniforms_block* block = (uniforms_block*)glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(uniforms_block), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

	mat4 model_matrix = scale(7.0f);

	block->mv_matrix = view_matrix * model_matrix;
	block->view_matrix = view_matrix;
	block->proj_matrix = perspective(50.0f, (float)gWidth / (float)gHeight, 0.1f, 1000.0f);
	glUnmapBuffer(GL_UNIFORM_BUFFER);

	glBindBufferBase(GL_UNIFORM_BUFFER, 1, sphere_buffer);
	sphere* s = (sphere*)glMapBufferRange(GL_UNIFORM_BUFFER, 0, 128 * sizeof(sphere), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

	int i;
	for (i = 0; i < 128; i++)
	{
		float fi = (float)i / 128.0f;
		s[i].center = vec3(sinf(fi * 123.0f + f) * 15.75f, cosf(fi * 456.0f + f) * 15.75f, (sinf(fi * 300.0f + f) * cosf(fi * 200.0f + f)) * 20.0f);
		s[i].radius = fi * 2.3f + 3.5f;
		float r = fi * 61.0f;
		float g = r + 0.25f;
		float b = g + 0.25f;
		r = (r - floorf(r)) * 0.8f + 0.2f;
		g = (g - floorf(g)) * 0.8f + 0.2f;
		b = (b - floorf(b)) * 0.8f + 0.2f;
		s[i].color = vec4(r, g, b, 1.0f);
	}

	glUnmapBuffer(GL_UNIFORM_BUFFER);

	glBindBufferBase(GL_UNIFORM_BUFFER, 2, plane_buffer);
	plane* p = (plane*)glMapBufferRange(GL_UNIFORM_BUFFER, 0, 128 * sizeof(plane), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

	p[0].normal = vec3(0.0f, 0.0f, -1.0f);
	p[0].d = 30.0f;

	p[1].normal = vec3(0.0f, 0.0f, 1.0f);
	p[1].d = 30.0f;

	p[2].normal = vec3(-1.0f, 0.0f, 0.0f);
	p[2].d = 30.0f;

	p[3].normal = vec3(1.0f, 0.0f, 0.0f);
	p[3].d = 30.0f;

	p[4].normal = vec3(0.0f, -1.0f, 0.0f);
	p[4].d = 30.0f;

	p[5].normal = vec3(0.0f, 1.0f, 0.0f);
	p[5].d = 30.0f;

	glUnmapBuffer(GL_UNIFORM_BUFFER);

	glBindBufferBase(GL_UNIFORM_BUFFER, 3, light_buffer);
	light* l = (light*)glMapBufferRange(GL_UNIFORM_BUFFER, 0, 128 * sizeof(light), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

	f *= 1.0f;
	for (i = 0; i < 128; i++)
	{
		float fi = 3.33f - (float)i;
		l[i].position = vec3(sinf(fi * 2.0f - f) * 15.75f, cosf(fi * 5.0f - f) * 5.75f, (sinf(fi * 3.0f - f) * cosf(fi * 2.5f - f)) * 19.4f);
	}

	glUnmapBuffer(GL_UNIFORM_BUFFER);

	glBindVertexArray(vao);
	glViewport(0, 0, gWidth, gHeight);

	glUseProgram(gPrepareShaderProgram);
	glUniformMatrix4fv(uniforms.ray_lookat, 1, GL_FALSE, view_matrix);
	glUniform3fv(uniforms.ray_origin, 1, view_position);
	glUniform1f(uniforms.aspect, (float)gWidth / (float)gHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, ray_fbo[0]);
	static const GLenum draw_buffers[] =
	{
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2,
		GL_COLOR_ATTACHMENT3,
		GL_COLOR_ATTACHMENT4,
		GL_COLOR_ATTACHMENT5
	};
	glDrawBuffers(6, draw_buffers);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glUseProgram(gTraceShaderProgram);
	recurse(0);

	glUseProgram(gBlitShaderProgram);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDrawBuffer(GL_BACK);

	glActiveTexture(GL_TEXTURE0);
	switch (debug_mode)
	{
	case DEBUG_NONE:
		glBindTexture(GL_TEXTURE_2D, tex_composite);
		break;

	case DEBUG_REFLECTED:
		glBindTexture(GL_TEXTURE_2D, tex_reflected[debug_depth]);
		break;

	case DEBUG_REFRACTED:
		glBindTexture(GL_TEXTURE_2D, tex_refracted[debug_depth]);
		break;

	case DEBUG_REFLECTED_COLOR:
		glBindTexture(GL_TEXTURE_2D, tex_reflected_intensity[debug_depth]);
		break;

	case DEBUG_REFRACTED_COLOR:
		glBindTexture(GL_TEXTURE_2D, tex_reflected_intensity[debug_depth]);
		break;
	}

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

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

void recurse(int depth)
{
	glBindFramebuffer(GL_FRAMEBUFFER, ray_fbo[depth + 1]);

	static const GLenum draw_buffers[] =
	{
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2,
		GL_COLOR_ATTACHMENT3,
		GL_COLOR_ATTACHMENT4,
		GL_COLOR_ATTACHMENT5
	};
	glDrawBuffers(6, draw_buffers);

	glEnablei(GL_BLEND, 0);
	glBlendFunci(0, GL_ONE, GL_ONE);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_position[depth]);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex_reflected[depth]);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, tex_reflected_intensity[depth]);

	// render
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	if (depth != (max_depth - 1))
	{
		recurse(depth + 1);
	}

	glDisablei(GL_BLEND, 0);
}

void CompileShader(GLuint shader, const char *name)
{
	// compilation errors 
	GLint   iShaderCompileStatus = 0;
	GLint   iInfoLogLength = 0;
	GLchar* szInfoLog = NULL;

	glCompileShader(shader);

	glGetShaderiv(shader, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(shader, GL_INFO_LOG_LENGTH, &written, szInfoLog);

				fprintf(gpFile, "%s Shader Compiler Info Log: \n%s\n", name, szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(ghwnd);
			}
		}
	}
}

void LinkProgram(GLuint program, const char* name)
{
	// linking errors
	GLint   iProgramLinkStatus = 0;
	GLint   iInfoLogLength = 0;
	GLchar* szInfoLog = NULL;

	// link the shader program
	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &iProgramLinkStatus);
	if (iProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(program, GL_INFO_LOG_LENGTH, &written, szInfoLog);

				fprintf(gpFile, ("%s Shader Program Linking Info Log: \n%s\n"), name, szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(ghwnd);
			}
		}
	}

}