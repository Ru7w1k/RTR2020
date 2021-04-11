// headers
#include <windows.h>
#include <stdio.h>

#include <gl/glew.h> 
#include <gl/GL.h>

#include "vmath.h"
#include "OGLTemplate.h"
#include "Sphere.h"

#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "Sphere.lib")

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

//// per vertex /////////////////////////////
GLuint gShaderProgramObjectPerVert;

GLuint mMatrixUniformPerVert;
GLuint vMatrixUniformPerVert;
GLuint pMatrixUniformPerVert;

GLuint laUniformPerVert[3];
GLuint ldUniformPerVert[3];
GLuint lsUniformPerVert[3];
GLuint lightPositionUniformPerVert[3];

GLuint kaUniformPerVert;
GLuint kdUniformPerVert;
GLuint ksUniformPerVert;
GLuint shininessUniformPerVert;

GLuint enableLightUniformPerVert;
//////////////////////////////////////////////

//// per fragment ////////////////////////////
GLuint gShaderProgramObjectPerFrag;

GLuint mMatrixUniformPerFrag;
GLuint vMatrixUniformPerFrag;
GLuint pMatrixUniformPerFrag;

GLuint laUniformPerFrag[3];
GLuint ldUniformPerFrag[3];
GLuint lsUniformPerFrag[3];
GLuint lightPositionUniformPerFrag[3];

GLuint kaUniformPerFrag;
GLuint kdUniformPerFrag;
GLuint ksUniformPerFrag;
GLuint shininessUniformPerFrag;

GLuint enableLightUniformPerFrag;
//////////////////////////////////////////////

GLuint vaoSphere;
GLuint vboPositionSphere;
GLuint vboNormalSphere;
GLuint vboElementSphere;

mat4 perspectiveProjectionMatrix;

float sphereVertices[1146];
float sphereNormals[1146];
float sphereTextures[1146];
unsigned short sphereElements[2280];
int gNumVertices = 0;
int gNumElements = 0;

bool bLight = false;
bool bFragment = false;

typedef struct light {
	vec3 la;
	vec3 ld;
	vec3 ls;
	vec4 position;
} Light;

Light lights[3];

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
		TEXT("OpenGL | Three Lights on Sphere"),
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

		case 'v':
		case 'V':
			bFragment = false;
			break;

		case 'f':
		case 'F':
			bFragment = true;
			break;

		case 'q':
		case 'Q':
			DestroyWindow(hwnd);
			break;
		
		default:
			break;
		}
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
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

	/// P E R   V E R T E X ////////////////////////////////////////////////////

	//// vertex shader
	// create shader
	GLuint gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	// provide source code to shader
	const GLchar *vertexShaderSourceCodePerVert = 
		"#version 450 core \n" \

		"in vec4 vPosition; \n" \
		"in vec3 vNormal; \n" \

		"uniform mat4 u_mMatrix; \n" \
		"uniform mat4 u_vMatrix; \n" \
		"uniform mat4 u_pMatrix; \n" \

		"uniform vec3 u_La[3]; \n" \
		"uniform vec3 u_Ld[3]; \n" \
		"uniform vec3 u_Ls[3]; \n" \
		"uniform vec3 u_Ka; \n" \
		"uniform vec3 u_Kd; \n" \
		"uniform vec3 u_Ks; \n" \

		"uniform float u_Shininess; \n" \
		"uniform vec4 u_LightPos[3]; \n" \
		"uniform int u_bLight; \n" \

		"out vec3 out_PhongLight; \n" \

		"void main (void) \n" \
		"{ \n" \
		"	out_PhongLight = vec3(0.0); \n" \
		"	if (u_bLight == 1)" \
		"	{ \n" \
		"		vec4 eyeCoordinates = u_vMatrix * u_mMatrix * vPosition; \n" \
		"		vec3 tNorm = normalize(mat3(u_vMatrix * u_mMatrix) * vNormal); \n" \
		"		vec3 viewerVector = normalize(vec3(-eyeCoordinates.xyz)); \n" \

		"		for (int i = 0; i < 3; i++) \n" \
		"		{ \n" \
		"			vec3 lightDir = normalize(vec3(u_LightPos[i] - eyeCoordinates)); \n" \
		"			float tNormDotLightDir = max(dot(tNorm, lightDir), 0.0); \n" \
		"			vec3 reflectionVector = reflect(-lightDir, tNorm); \n" \

		"			vec3 ambient = u_La[i] * u_Ka; \n" \
		"			vec3 diffuse = u_Ld[i] * u_Kd * tNormDotLightDir; \n" \
		"			vec3 specular = u_Ls[i] * u_Ks * pow(max(dot(reflectionVector, viewerVector), 0.0), u_Shininess); \n" \

		"			out_PhongLight += ambient + diffuse + specular; \n" \
		"		} \n" \
		"	} \n" \
		"	else \n" \
		"	{ \n" \
		"		out_PhongLight = vec3(1.0, 1.0, 1.0); \n" \
		"	} \n" \
		"	gl_Position = u_pMatrix * u_vMatrix * u_mMatrix * vPosition; \n" \
		"} \n";

	glShaderSource(gVertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCodePerVert, NULL);

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
	const GLchar *fragmentShaderSourceCodePerVert = 
		"#version 450 core \n" \

		"in vec3 out_PhongLight; \n" \
		"out vec4 FragColor; \n" \

		"void main (void) \n" \
		"{ \n" \
		"	FragColor = vec4(out_PhongLight, 1.0); \n" \
		"} \n";

	glShaderSource(gFragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCodePerVert, NULL);

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
	gShaderProgramObjectPerVert = glCreateProgram();

	// attach shaders
	glAttachShader(gShaderProgramObjectPerVert, gVertexShaderObject);
	glAttachShader(gShaderProgramObjectPerVert, gFragmentShaderObject);

	// pre-linking binding to vertex attribute
	glBindAttribLocation(gShaderProgramObjectPerVert, RMC_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(gShaderProgramObjectPerVert, RMC_ATTRIBUTE_NORMAL, "vNormal");

	// link shader
	glLinkProgram(gShaderProgramObjectPerVert);

	// linking errors
	GLint iProgramLinkStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;

	glGetProgramiv(gShaderProgramObjectPerVert, GL_LINK_STATUS, &iProgramLinkStatus);
	if (iProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObjectPerVert, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObjectPerVert, GL_INFO_LOG_LENGTH, &written, szInfoLog);

				fprintf(gpFile, ("Shader Program Linking Info Log: \n%s\n"), szInfoLog);
				free(szInfoLog);
				DestroyWindow(ghwnd);
			}
		}
	}

	// post-linking retrieving uniform locations
	mMatrixUniformPerVert = glGetUniformLocation(gShaderProgramObjectPerVert, "u_mMatrix");
	vMatrixUniformPerVert = glGetUniformLocation(gShaderProgramObjectPerVert, "u_vMatrix");
	pMatrixUniformPerVert = glGetUniformLocation(gShaderProgramObjectPerVert, "u_pMatrix");

	for (int i = 0; i < 3; i++)
	{
		char str[64];

		sprintf(str, "u_La[%d]", i);
		laUniformPerVert[i] = glGetUniformLocation(gShaderProgramObjectPerVert, str);

		sprintf(str, "u_Ld[%d]", i);
		ldUniformPerVert[i] = glGetUniformLocation(gShaderProgramObjectPerVert, str);

		sprintf(str, "u_Ls[%d]", i);
		lsUniformPerVert[i] = glGetUniformLocation(gShaderProgramObjectPerVert, str);
	
		sprintf(str, "u_LightPos[%d]", i);
		lightPositionUniformPerVert[i] = glGetUniformLocation(gShaderProgramObjectPerVert, str);
	}

	kaUniformPerVert = glGetUniformLocation(gShaderProgramObjectPerVert, "u_Ka");
	kdUniformPerVert = glGetUniformLocation(gShaderProgramObjectPerVert, "u_Kd");
	ksUniformPerVert = glGetUniformLocation(gShaderProgramObjectPerVert, "u_Ks");

	shininessUniformPerVert = glGetUniformLocation(gShaderProgramObjectPerVert, "u_Shininess");
	enableLightUniformPerVert = glGetUniformLocation(gShaderProgramObjectPerVert, "u_bLight");

	////////////////////////////////////////////////////////////////////////////

	/// P E R   F R A G M E N T ////////////////////////////////////////////////

	//// vertex shader
	// create shader
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	// provide source code to shader
	const GLchar *vertexShaderSourceCodePerFrag = 
		"#version 450 core \n" \

		"in vec4 vPosition; \n" \
		"in vec3 vNormal; \n" \

		"uniform mat4 u_mMatrix; \n" \
		"uniform mat4 u_vMatrix; \n" \
		"uniform mat4 u_pMatrix; \n" \

		"uniform vec4 u_LightPos[3]; \n" \
		"uniform int u_bLight; \n" \

		"out vec3 tNorm; \n" \
		"out vec3 lightDir[3]; \n" \
		"out vec3 viewerVector; \n" \

		"void main (void) \n" \
		"{ \n" \
		"	if (u_bLight == 1)" \
		"	{ \n" \
		"		vec4 eyeCoordinates = u_vMatrix * u_mMatrix * vPosition; \n" \
		"		tNorm = mat3(u_vMatrix * u_mMatrix) * vNormal; \n" \
		"		viewerVector = normalize(vec3(-eyeCoordinates.xyz)); \n" \
		"		for (int i = 0; i < 3; i++) \n" \
		"		{ \n" \
		"			lightDir[i] = vec3(u_LightPos[i] - eyeCoordinates); \n" \
		"		} \n" \
		"	} \n" \
		"	gl_Position = u_pMatrix * u_vMatrix * u_mMatrix * vPosition; \n" \
		"} \n";

	glShaderSource(gVertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCodePerFrag, NULL);

	// compile shader
	glCompileShader(gVertexShaderObject);

	// compilation errors 
	iShaderCompileStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;

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
	gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	// provide source code to shader
	const GLchar *fragmentShaderSourceCodePerFrag = 
		"#version 450 core \n" \

		"in vec3 tNorm; \n" \
		"in vec3 lightDir[3]; \n" \
		"in vec3 viewerVector; \n" \

		"uniform vec3 u_La[3]; \n" \
		"uniform vec3 u_Ld[3]; \n" \
		"uniform vec3 u_Ls[3]; \n" \
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
		"		vec3 phongLight = vec3(0.0); \n" \
		"		vec3 normTNorm = normalize(tNorm); \n" \
		"		vec3 normViewerVector = normalize(viewerVector); \n" \

		"		for (int i = 0; i < 3; i++) \n" \
		"		{ \n" \
		"			vec3 normLightDir = normalize(lightDir[i]); \n" \
		"			vec3 reflectionVector = reflect(-normLightDir, normTNorm); \n" \
		"			float tNormDotLightDir = max(dot(normTNorm, normLightDir), 0.0); \n" \

		"			vec3 ambient = u_La[i] * u_Ka; \n" \
		"			vec3 diffuse = u_Ld[i] * u_Kd * tNormDotLightDir; \n" \
		"			vec3 specular = u_Ls[i] * u_Ks * pow(max(dot(reflectionVector, normViewerVector), 0.0), u_Shininess); \n" \
		"			phongLight += ambient + diffuse + specular; \n" \

		"		} \n" \
		"		FragColor = vec4(phongLight, 1.0); \n" \
		"	} \n" \
		"	else \n" \
		"	{ \n" \
		"		FragColor = vec4(1.0); \n" \
		"	} \n" \
		"} \n";

	glShaderSource(gFragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCodePerFrag, NULL);

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
	gShaderProgramObjectPerFrag = glCreateProgram();

	// attach shaders
	glAttachShader(gShaderProgramObjectPerFrag, gVertexShaderObject);
	glAttachShader(gShaderProgramObjectPerFrag, gFragmentShaderObject);

	// pre-linking binding to vertex attribute
	glBindAttribLocation(gShaderProgramObjectPerFrag, RMC_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(gShaderProgramObjectPerFrag, RMC_ATTRIBUTE_NORMAL, "vNormal");

	// link shader
	glLinkProgram(gShaderProgramObjectPerFrag);

	// linking errors
	iProgramLinkStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;

	glGetProgramiv(gShaderProgramObjectPerFrag, GL_LINK_STATUS, &iProgramLinkStatus);
	if (iProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObjectPerFrag, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObjectPerFrag, GL_INFO_LOG_LENGTH, &written, szInfoLog);

				fprintf(gpFile, ("Shader Program Linking Info Log: \n%s\n"), szInfoLog);
				free(szInfoLog);
				DestroyWindow(ghwnd);
			}
		}
	}

	// post-linking retrieving uniform locations
	mMatrixUniformPerFrag = glGetUniformLocation(gShaderProgramObjectPerFrag, "u_mMatrix");
	vMatrixUniformPerFrag = glGetUniformLocation(gShaderProgramObjectPerFrag, "u_vMatrix");
	pMatrixUniformPerFrag = glGetUniformLocation(gShaderProgramObjectPerFrag, "u_pMatrix");

	for (int i = 0; i < 3; i++)
	{
		char str[64];

		sprintf(str, "u_La[%d]", i);
		laUniformPerFrag[i] = glGetUniformLocation(gShaderProgramObjectPerFrag, str);

		sprintf(str, "u_Ld[%d]", i);
		ldUniformPerFrag[i] = glGetUniformLocation(gShaderProgramObjectPerFrag, str);

		sprintf(str, "u_Ls[%d]", i);
		lsUniformPerFrag[i] = glGetUniformLocation(gShaderProgramObjectPerFrag, str);
	
		sprintf(str, "u_LightPos[%d]", i);
		lightPositionUniformPerFrag[i] = glGetUniformLocation(gShaderProgramObjectPerFrag, str);
	}

	kaUniformPerFrag = glGetUniformLocation(gShaderProgramObjectPerFrag, "u_Ka");
	kdUniformPerFrag = glGetUniformLocation(gShaderProgramObjectPerFrag, "u_Kd");
	ksUniformPerFrag = glGetUniformLocation(gShaderProgramObjectPerFrag, "u_Ks");

	shininessUniformPerFrag = glGetUniformLocation(gShaderProgramObjectPerFrag, "u_Shininess");
	enableLightUniformPerFrag = glGetUniformLocation(gShaderProgramObjectPerFrag, "u_bLight");
	
	////////////////////////////////////////////////////////////////////////////

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
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// set clear depth
	glClearDepth(1.0f);

	// depth test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// light settings
	lights[0].la = vec3(0.0f, 0.0f, 0.0f);
	lights[0].ld = vec3(1.0f, 0.0f, 0.0f);
	lights[0].ls = vec3(1.0f, 0.0f, 0.0f);
	lights[0].position = vec4(1.0f, 0.0f, 0.0f, 1.0f);

	lights[1].la = vec3(0.0f, 0.0f, 0.0f);
	lights[1].ld = vec3(0.0f, 1.0f, 0.0f);
	lights[1].ls = vec3(0.0f, 1.0f, 0.0f);
	lights[1].position = vec4(1.0f, 0.0f, 0.0f, 1.0f);

	lights[2].la = vec3(0.0f, 0.0f, 0.0f);
	lights[2].ld = vec3(0.0f, 0.0f, 1.0f);
	lights[2].ls = vec3(0.0f, 0.0f, 1.0f);
	lights[2].position = vec4(1.0f, 0.0f, 0.0f, 1.0f);

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
	translateMatrix = translate(0.0f, 0.0f, -3.0f);
	modelMatrix = translateMatrix;

	// send necessary matrices to shader in respective uniforms
	if (bFragment)
	{
		glUseProgram(gShaderProgramObjectPerFrag);
		glUniformMatrix4fv(mMatrixUniformPerFrag, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(vMatrixUniformPerFrag, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(pMatrixUniformPerFrag, 1, GL_FALSE, perspectiveProjectionMatrix);

		if (bLight)
		{
			for (int i = 0; i < 3; i++)
			{
				glUniform3fv(laUniformPerFrag[i], 1, lights[i].la);
				glUniform3fv(ldUniformPerFrag[i], 1, lights[i].ld);
				glUniform3fv(lsUniformPerFrag[i], 1, lights[i].ls);
				glUniform4fv(lightPositionUniformPerFrag[i], 1, lights[i].position);
			}
			
			glUniform3f(kaUniformPerFrag, 0.0f, 0.0f, 0.0f);
			glUniform3f(kdUniformPerFrag, 1.0f, 1.0f, 1.0f);
			glUniform3f(ksUniformPerFrag, 1.0f, 1.0f, 1.0f);
			glUniform1f(shininessUniformPerFrag, 128.0f);

			glUniform1i(enableLightUniformPerFrag, 1);
		}
		else
		{
			glUniform1i(enableLightUniformPerFrag, 0);
		}
	}
	else
	{
		glUseProgram(gShaderProgramObjectPerVert);
		glUniformMatrix4fv(mMatrixUniformPerVert, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(vMatrixUniformPerVert, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(pMatrixUniformPerVert, 1, GL_FALSE, perspectiveProjectionMatrix);

		if (bLight)
		{
			for (int i = 0; i < 3; i++)
			{
				glUniform3fv(laUniformPerVert[i], 1, lights[i].la);
				glUniform3fv(ldUniformPerVert[i], 1, lights[i].ld);
				glUniform3fv(lsUniformPerVert[i], 1, lights[i].ls);
				glUniform4fv(lightPositionUniformPerVert[i], 1, lights[i].position);
			}
			
			glUniform3f(kaUniformPerVert, 0.0f, 0.0f, 0.0f);
			glUniform3f(kdUniformPerVert, 1.0f, 1.0f, 1.0f);
			glUniform3f(ksUniformPerVert, 1.0f, 1.0f, 1.0f);
			glUniform1f(shininessUniformPerVert, 128.0f);

			glUniform1i(enableLightUniformPerVert, 1);
		}
		else
		{
			glUniform1i(enableLightUniformPerVert, 0);
		}
	}

	// bind with vaoPyramid (this will avoid many binding to vbo)
	glBindVertexArray(vaoSphere);

	// draw necessary scene
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboElementSphere);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	// unbind vaoPyramid
	glBindVertexArray(0);

	// stop using OpenGL program object
	glUseProgram(0);

	SwapBuffers(ghdc);
}

void update(void)
{
	// code
	static float angle = 0.0f;
	static float radius = 100.0f;

	float cosA = cosf(radians(angle));
	float sinA = sinf(radians(angle));

	lights[0].position = vec4(0.0f, radius * cosA, radius * sinA, 1.0f);
	lights[1].position = vec4(radius * cosA, 0.0f, radius * sinA, 1.0f);
	lights[2].position = vec4(radius * cosA, radius * sinA, 0.0f, 1.0f);

	angle += 1.0f;
	if (angle >= 360.0f) 
		angle = 0.0f;
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
	if (gShaderProgramObjectPerFrag)
	{
		GLsizei shaderCount;
		GLsizei i;

		glUseProgram(gShaderProgramObjectPerFrag);
		glGetProgramiv(gShaderProgramObjectPerFrag, GL_ATTACHED_SHADERS, &shaderCount);
		
		GLuint *pShaders = (GLuint*) malloc(shaderCount * sizeof(GLuint));
		if (pShaders)
		{
			glGetAttachedShaders(gShaderProgramObjectPerFrag, shaderCount, &shaderCount, pShaders);

			for (i = 0; i < shaderCount; i++)
			{
				// detach shader
				glDetachShader(gShaderProgramObjectPerFrag, pShaders[i]);

				// delete shader
				glDeleteShader(pShaders[i]);
				pShaders[i] = 0;
			}

			free(pShaders);
		}

		glDeleteProgram(gShaderProgramObjectPerFrag);
		gShaderProgramObjectPerFrag = 0;
		glUseProgram(0);
	}

	if (gShaderProgramObjectPerVert)
	{
		GLsizei shaderCount;
		GLsizei i;

		glUseProgram(gShaderProgramObjectPerVert);
		glGetProgramiv(gShaderProgramObjectPerVert, GL_ATTACHED_SHADERS, &shaderCount);
		
		GLuint *pShaders = (GLuint*) malloc(shaderCount * sizeof(GLuint));
		if (pShaders)
		{
			glGetAttachedShaders(gShaderProgramObjectPerVert, shaderCount, &shaderCount, pShaders);

			for (i = 0; i < shaderCount; i++)
			{
				// detach shader
				glDetachShader(gShaderProgramObjectPerVert, pShaders[i]);

				// delete shader
				glDeleteShader(pShaders[i]);
				pShaders[i] = 0;
			}

			free(pShaders);
		}

		glDeleteProgram(gShaderProgramObjectPerVert);
		gShaderProgramObjectPerVert = 0;
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


