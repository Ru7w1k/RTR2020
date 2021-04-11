// headers
#include <windows.h>
#include <stdio.h>

#include <gl/glew.h> 
#include <gl/GL.h>

#include "vmath.h"
#include "OGLTemplate.h"
#include "Sphere.h"
#include "Materials.h"

#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "Sphere.lib")

// macros
#define WIN_WIDTH  800
#define WIN_HEIGHT 600

#define X_AXIS 0
#define Y_AXIS 1
#define Z_AXIS 2
#define PAUSE  3

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

int gWidth;
int gHeight;

bool gbFullscreen   = false;
bool gbActiveWindow = false;

HWND  ghwnd  = NULL;
FILE* gpFile = NULL;

DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };

GLuint gShaderProgramObject;

GLuint vaoSphere;
GLuint vboPositionSphere;
GLuint vboNormalSphere;
GLuint vboElementSphere;

GLuint mMatrixUniform;
GLuint vMatrixUniform;
GLuint pMatrixUniform;

GLuint laUniform;
GLuint ldUniform;
GLuint lsUniform;
GLuint lightPositionUniform;

GLuint kaUniform;
GLuint kdUniform;
GLuint ksUniform;
GLuint shininessUniform;

GLuint enableLightUniform;

mat4 perspectiveProjectionMatrix;

float sphereVertices[1146];
float sphereNormals[1146];
float sphereTextures[1146];
unsigned short sphereElements[2280];
int gNumVertices = 0;
int gNumElements = 0;

bool bLight = false;
vec4 lightPos;
float lightAngle = 0.0f;
int lightAxis = X_AXIS;

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
		TEXT("OpenGL | Materials"),
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
		switch (wParam)
		{
		case 'l':
		case 'L':
			bLight = !bLight;
			break;

		case 'x':
		case 'X':
			if (lightAxis == X_AXIS)
				lightAxis = PAUSE;
			else
				lightAxis = X_AXIS;
			break;

		case 'y':
		case 'Y':
			if (lightAxis == Y_AXIS)
				lightAxis = PAUSE;
			else
				lightAxis = Y_AXIS;
			break;

		case 'z':
		case 'Z':
			if (lightAxis == Z_AXIS)
				lightAxis = PAUSE;
			else
				lightAxis = Z_AXIS;
			break;
		
		default:
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
	void uninitialize(void);

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
		"in vec3 vNormal; \n" \

		"uniform mat4 u_mMatrix; \n" \
		"uniform mat4 u_vMatrix; \n" \
		"uniform mat4 u_pMatrix; \n" \

		"uniform vec4 u_LightPos; \n" \
		"uniform int u_bLight; \n" \

		"out vec3 tNorm; \n" \
		"out vec3 lightDir; \n" \
		"out vec3 viewerVector; \n" \

		"void main (void) \n" \
		"{ \n" \
		"	if (u_bLight == 1)" \
		"	{ \n" \
		"		vec4 eyeCoordinates = u_vMatrix * u_mMatrix * vPosition; \n" \
		"		tNorm = mat3(u_vMatrix * u_mMatrix) * vNormal; \n" \
		"		lightDir = vec3(u_LightPos - eyeCoordinates); \n" \
		"		viewerVector = normalize(vec3(-eyeCoordinates.xyz)); \n" \
		"	} \n" \
		"	gl_Position = u_pMatrix * u_vMatrix * u_mMatrix * vPosition; \n" \
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

		"in vec3 tNorm; \n" \
		"in vec3 lightDir; \n" \
		"in vec3 viewerVector; \n" \

		"uniform vec3 u_La; \n" \
		"uniform vec3 u_Ld; \n" \
		"uniform vec3 u_Ls; \n" \
		"uniform vec3 u_Ka; \n" \
		"uniform vec3 u_Kd; \n" \
		"uniform vec3 u_Ks; \n" \

		"uniform float u_Shininess; \n" \
		"uniform int u_bLight; \n" \

		"out vec4 FragColor; \n" \

		"void main (void) \n" \
		"{ \n" \
		"	if (u_bLight == 1)" \
		"	{ \n" \
		"		vec3 normTNorm = normalize(tNorm); \n" \
		"		vec3 normLightDir = normalize(lightDir); \n" \
		"		vec3 normViewerVector = normalize(viewerVector); \n" \

		"		vec3 reflectionVector = reflect(-normLightDir, normTNorm); \n" \
		"		float tNormDotLightDir = max(dot(normTNorm, normLightDir), 0.0); \n" \

		"		vec3 ambient = u_La * u_Ka; \n" \
		"		vec3 diffuse = u_Ld * u_Kd * tNormDotLightDir; \n" \
		"		vec3 specular = u_Ls * u_Ks * pow(max(dot(reflectionVector, normViewerVector), 0.0), u_Shininess); \n" \
		"		vec3 phongLight = ambient + diffuse + specular; \n" \

		"		FragColor = vec4(phongLight, 1.0); \n" \
		"	} \n" \
		"	else \n" \
		"	{ \n" \
		"		FragColor = vec4(1.0); \n" \
		"	} \n" \
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
	glBindAttribLocation(gShaderProgramObject, RMC_ATTRIBUTE_NORMAL, "vNormal");

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
	mMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_mMatrix");
	vMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_vMatrix");
	pMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_pMatrix");

	laUniform = glGetUniformLocation(gShaderProgramObject, "u_La");
	ldUniform = glGetUniformLocation(gShaderProgramObject, "u_Ld");
	lsUniform = glGetUniformLocation(gShaderProgramObject, "u_Ls");

	kaUniform = glGetUniformLocation(gShaderProgramObject, "u_Ka");
	kdUniform = glGetUniformLocation(gShaderProgramObject, "u_Kd");
	ksUniform = glGetUniformLocation(gShaderProgramObject, "u_Ks");

	shininessUniform = glGetUniformLocation(gShaderProgramObject, "u_Shininess");
	enableLightUniform = glGetUniformLocation(gShaderProgramObject, "u_bLight");
	lightPositionUniform = glGetUniformLocation(gShaderProgramObject, "u_LightPos");
	
	// sphere data
	getSphereVertexData(sphereVertices, sphereNormals, sphereTextures, sphereElements);
	gNumVertices = getNumberOfSphereVertices();
	gNumElements = getNumberOfSphereElements();	

	// create vao for cube
	glGenVertexArrays(1, &vaoSphere);
	glBindVertexArray(vaoSphere);

	// create vbo for position
	glGenBuffers(1, &vboPositionSphere);
	glBindBuffer(GL_ARRAY_BUFFER, vboPositionSphere);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphereVertices), sphereVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(RMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(RMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// create vbo for normal
	glGenBuffers(1, &vboNormalSphere);
	glBindBuffer(GL_ARRAY_BUFFER, vboNormalSphere);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphereNormals), sphereNormals, GL_STATIC_DRAW);
	glVertexAttribPointer(RMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(RMC_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// create vbo for elements
	glGenBuffers(1, &vboElementSphere);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboElementSphere);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphereElements), sphereElements, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	// set clear color
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	// set clear depth
	glClearDepth(1.0f);

	// depth test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	perspectiveProjectionMatrix = mat4::identity();

	// materials
	InitMaterials();

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

	perspectiveProjectionMatrix = vmath::perspective(60.0f, ((float)width/6.0f) / ((float)height/4.0f), 0.1f, 100.0f);
}

void display(void)
{
	// code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// start using OpenGL program object
	glUseProgram(gShaderProgramObject);

	//declaration of matrices
	mat4 translateMatrix;
	mat4 modelMatrix;
	mat4 viewMatrix;

	//// cube ////////////////////////

	// intialize above matrices to identity
	translateMatrix = mat4::identity();
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();

	// transformations
	translateMatrix = translate(0.0f, 0.0f, -2.0f);
	modelMatrix = translateMatrix;

	// send necessary matrices to shader in respective uniforms
	glUniformMatrix4fv(mMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(vMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(pMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

	if (bLight)
	{
		glUniform3f(laUniform, 0.0f, 0.0f, 0.0f);
		glUniform3f(ldUniform, 1.0f, 1.0f, 1.0f);
		glUniform3f(lsUniform, 1.0f, 1.0f, 1.0f);
		glUniform4fv(lightPositionUniform, 1, lightPos);
		glUniform1i(enableLightUniform, 1);
	}
	else
	{
		glUniform1i(enableLightUniform, 0);
	}

	// bind with vaoPyramid (this will avoid many binding to vbo)
	glBindVertexArray(vaoSphere);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboElementSphere);

	float viewportW = (float)gWidth/6.0f;
	float viewportH = (float)gHeight/4.0f;

	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			glViewport(i*viewportW, j*viewportH, viewportW, viewportH);

			glUniform3fv(kaUniform, 1, Materials[i][j].MaterialAmbient);
			glUniform3fv(kdUniform, 1, Materials[i][j].MaterialDiffuse);
			glUniform3fv(ksUniform, 1, Materials[i][j].MaterialSpecular);
			glUniform1f(shininessUniform, Materials[i][j].MaterialShininess[0]);

			// draw necessary scene
			glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
		}
	}

	// unbind vaoPyramid
	glBindVertexArray(0);

	// stop using OpenGL program object
	glUseProgram(0);

	SwapBuffers(ghdc);
}

void update(void)
{
	// code

	if (lightAxis != PAUSE)
	{
		lightAngle += 1.0f;
		if (lightAngle >= 360.0f) 
			lightAngle = 0.0f;
	}

	float cosA = cosf(radians(lightAngle));
	float sinA = sinf(radians(lightAngle));

	switch (lightAxis)
	{
	case X_AXIS:
		lightPos = vec4(0.0f, 100.0f * cosA, 100.0f * sinA, 1.0f);
		break;

	case Y_AXIS:
		lightPos = vec4(100.0f * cosA, 0.0f, 100.0f * sinA, 1.0f);
		break;

	case Z_AXIS:
		lightPos = vec4(100.0f * cosA, 100.0f * sinA, 0.0f, 1.0f);
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

	if (vaoSphere)
	{
		glDeleteVertexArrays(1, &vaoSphere);
		vaoSphere = 0;
	}

	if (vboPositionSphere)
	{
		glDeleteBuffers(1, &vboPositionSphere);
		vboPositionSphere = 0;
	}

	if (vboNormalSphere)
	{
		glDeleteBuffers(1, &vboNormalSphere);
		vboNormalSphere = 0;
	}

	if (vboElementSphere)
	{
		glDeleteBuffers(1, &vboElementSphere);
		vboElementSphere = 0;
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


