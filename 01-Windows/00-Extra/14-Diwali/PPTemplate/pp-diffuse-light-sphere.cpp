// Headers
#include <Windows.h>
#include <stdio.h>
#include <shellscalingapi.h>

#include <GL/glew.h>
#include <gl/GL.h>

#include "vmath.h"
#include "Sphere.h"

// Linker Options
#pragma comment(lib, "shcore.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "sphere.lib")

// Defines
#define WIN_WIDTH  1920
#define WIN_HEIGHT 1080

#define SIZE 10

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
GLuint gBlurShader;
GLuint gFinalShader;

GLuint vao_sphere;					// vertex array object
GLuint vbo_position_sphere;		// vertex buffer object
GLuint vbo_normal_sphere;			// vertex buffer object
GLuint vbo_element_sphere;			// vertex buffer object

GLuint vaoCube;					// vertex array object
GLuint vbo_position_cube;		// vertex buffer object
GLuint vbo_normal_cube;			// vertex buffer object

GLuint vaoQuad;
GLuint vbo_position_quad;
GLuint vbo_texture_quad;

GLuint mUniform;
GLuint vUniform;
GLuint pUniform;

GLuint kdUniform;

GLuint enableLightUniform;
GLuint lightsUniform;

GLuint laUniform;
GLuint ldUniform;
GLuint lightPositionUniform;
GLuint lightStrengthUniform;
GLuint constAttUniform;
GLuint linearAttUniform;
GLuint quadAttUniform;

mat4   perspectiveProjectionMatrix;

int lightCount = 50;
vec3 lightPos[100];
vec3 lightPosPrev[100];
vec3 lightVel[100];
vec3 lightAcc[100];
vec3 lightAmbient[100];
vec3 lightDiffuse[100];
float lightStrength[100];
float lightConstAtt[100];
float lightLinearAtt[100];
float lightQuadAtt[100];

float delta[SIZE*SIZE];


int state = 0;
float colorCount = 0.0f;

float angleCube = 0.0f;

BOOL bAnimation = TRUE;
BOOL bLight = TRUE;
bool bloom = true;

float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];
int gNumVertices;
int gNumElements;

// bloom and HDR
unsigned int hdrFBO;
unsigned int colorBuffers[2];
unsigned int rboDepth;

unsigned int pingpongFBO[2];
unsigned int pingpongColorBuffers[2];

// Global function declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
float getRand(float min, float max);

// WinMain
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	// function declarations
	int initialize(void);
	void display(void);
	void update(void);

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
		TEXT("PP Diffuse Light on Sphere"),
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

	case WM_CHAR:
		switch (wParam)
		{
		case 'L':
		case 'l':
			if (bLight == TRUE) {
				bLight = FALSE;
			}
			else {
				bLight = TRUE;
			}
			break;

		case 'B':
		case 'b':
			bloom = !bloom;
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
					1920,//MI.rcMonitor.right - MI.rcMonitor.left,
					1080,//MI.rcMonitor.bottom - MI.rcMonitor.top,
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
	void initLights(void);

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
		
		"in vec4 vPosition;" \
		"in vec3 vNormal;" \
		
		"uniform mat4 u_m_matrix;" \
		"uniform mat4 u_v_matrix;" \
		"uniform mat4 u_p_matrix;" \
		"uniform lowp int u_enable_light;" \
		"uniform float inv = 1.0;" \
		
		"out vec4 Pos;" \
		"out vec3 Normal;" \
		
		"void main(void)" \
		"{" \
		"   if (u_enable_light == 1) " \
		"   { " \
		"       Pos = (u_m_matrix*vPosition);" \
		"       mat4 matNormal = transpose(inverse(u_m_matrix));" \
		"       Normal = inv*normalize((matNormal*vec4(vNormal, 0.0)).xyz);" \
		"   } " \
		"   gl_Position = u_p_matrix * u_v_matrix * u_m_matrix * vPosition;" \
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
	
		"in vec4 Pos;\n" \
		"in vec3 Normal;\n" \
		
		"uniform vec4 u_kd = vec4(1.0);\n" \
		"uniform float u_shininess = 2.0;\n" \
		
		"uniform lowp int u_lights = 10;\n" \
		"uniform  vec3 u_light_position[100];\n" \
		"uniform  vec4 u_la[100];\n" \
		"uniform  vec3 u_ld[100];\n" \
		"uniform float u_strength[100];\n" \
		"uniform float u_const_attenuation[100];\n" \
		"uniform float u_linear_attenuation[100];\n" \
		"uniform float u_quad_attenuation[100];\n" \

		"uniform vec3 u_eye_direction = vec3(0.0, 0.0, 0.0);\n" \

		"uniform int u_enable_light;\n" \
		"uniform int index;\n" \
		
		"layout (location = 0) out vec4 FragColor;\n" \
		"layout (location = 1) out vec4 BrightColor;\n" \
		
		"void main(void)" \
		"{" \
		"   if (u_enable_light == 1) " \
		"   { " \
		"		vec4 scatteredLight = vec4(0.0);" \
		"		vec4 reflectedLight = vec4(0.0);" \

		"		for(int i = 0; i < u_lights; i++)" \
		"		{ " \
		"			vec3 lightDir = vec3(Pos.xyz) - u_light_position[i];\n" \
		"			float len = length(lightDir);\n" \
		"			lightDir = lightDir/len;\n" \
		"			vec3 HalfVector = normalize(u_eye_direction - lightDir);\n" \
		"			float attenuate = 1 / (u_const_attenuation[i] + u_linear_attenuation[i]*len + u_quad_attenuation[i]*len*len);\n" \
		
		"			float diffuse = max(0.0f, -1*dot(Normal, lightDir)) * 0.5;\n" \
		"			float specular = max(0.0f, 1*dot(Normal, HalfVector));\n" \
		"			if (diffuse <= 0.00001)" \
		"				specular = 0.0f;\n" \
		"			else" \
		"				specular = pow(specular, u_shininess);\n" \
		
		"			scatteredLight += u_la[i] + vec4(u_ld[i]*diffuse*attenuate, 0.0);\n" \
		"			reflectedLight += vec4(u_ld[i]*specular*u_strength[i]*attenuate, 0.0);\n" \
		"		}" \
		"       FragColor = min(u_kd*scatteredLight + reflectedLight, vec4(1.0));\n" \
		"   } " \

		"   else " \
		"       FragColor = vec4(1.0);\n" \

		"   if(u_lights ==  1)\n" \
		"       BrightColor = FragColor;\n" \
		"   else\n" \
		"       BrightColor = vec4(0.0, 0.0, 0.0, 1.0);\n" \
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
	glBindAttribLocation(gShaderProgramObject, AMC_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(gShaderProgramObject, AMC_ATTRIBUTE_NORMAL, "vNormal");

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
	mUniform = glGetUniformLocation(gShaderProgramObject, "u_m_matrix");
	vUniform = glGetUniformLocation(gShaderProgramObject, "u_v_matrix");
	pUniform = glGetUniformLocation(gShaderProgramObject, "u_p_matrix");

	kdUniform = glGetUniformLocation(gShaderProgramObject, "u_kd");

	enableLightUniform   = glGetUniformLocation(gShaderProgramObject, "u_enable_light");
	lightsUniform        = glGetUniformLocation(gShaderProgramObject, "u_lights");
	lightPositionUniform = glGetUniformLocation(gShaderProgramObject, "u_light_position");
	laUniform            = glGetUniformLocation(gShaderProgramObject, "u_la");
	ldUniform            = glGetUniformLocation(gShaderProgramObject, "u_ld");
	lightStrengthUniform = glGetUniformLocation(gShaderProgramObject, "u_strength");
	constAttUniform      = glGetUniformLocation(gShaderProgramObject, "u_const_attenuation");
	linearAttUniform     = glGetUniformLocation(gShaderProgramObject, "u_linear_attenuation");
	quadAttUniform       = glGetUniformLocation(gShaderProgramObject, "u_quad_attenuation");

	//// blur shader /////////////////////////////////////////////////////////////////////////////

	// create vertex shader object
	GLuint vertexShaderObjectBlur = glCreateShader(GL_VERTEX_SHADER);

	// vertex shader source code 
	const GLchar* blurVertexShaderSourceCode = (GLchar*)
		"#version 450 core" \
		"\n" \

		"in vec4 vPosition;" \
		"in vec2 vTexcoord;" \

		"out vec2 TexCoord;" \

		"void main(void)" \
		"{" \
		"   TexCoord = vTexcoord;" \
		"   gl_Position = vPosition;" \
		"}";

	// attach source code to vertex shader
	glShaderSource(vertexShaderObjectBlur, 1, (const GLchar**)&blurVertexShaderSourceCode, NULL);

	// compile vertex shader source code
	glCompileShader(vertexShaderObjectBlur);

	// compilation errors 
	iShaderCompileStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;

	glGetShaderiv(vertexShaderObjectBlur, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObjectBlur, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(vertexShaderObjectBlur, GL_INFO_LOG_LENGTH, &written, szInfoLog);

				fprintf(gpFile, "Blur Vertex Shader Compiler Info Log: %s", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(ghWnd);
			}
		}
	}

	// create fragment shader object
	GLuint fragmentShaderObjectBlur = glCreateShader(GL_FRAGMENT_SHADER);

	// fragment shader source code
	const GLchar* blurFragmentShaderSourceCode = (GLchar*)
		"#version 450 core" \
		"\n" \

		"in vec2 TexCoord;\n" \
		"out vec4 FragColor;\n" \

		"uniform sampler2D image;\n" \
		"uniform bool horizontal;\n" \
		"uniform float weight[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);\n" \

		"void main(void)" \
		"{" \
		"	vec2 tex_offset = 1.0 / textureSize(image, 0); // get size of single texel\n" \
		"	vec3 result = texture(image, TexCoord).rgb * weight[0];\n" \
		
		"	if (horizontal)\n" \
		"	{\n" \
		"		for(int i = 0; i < 5; i++)\n" \
		"		{\n" \
		"			result += texture(image, TexCoord + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];\n" \
		"			result += texture(image, TexCoord - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];\n" \
		"		}\n" \
		"	}\n" \
		
		"	else\n" \
		"	{\n" \
		"		for(int i = 0; i < 5; i++)\n" \
		"		{\n" \
		"			result += texture(image, TexCoord + vec2(0.0, tex_offset.y * i)).rgb * weight[i];\n" \
		"			result += texture(image, TexCoord - vec2(0.0, tex_offset.y * i)).rgb * weight[i];\n" \
		"		}\n" \
		"	}\n" \
		
		"	FragColor = vec4(result, 1.0);\n" \
		"}";

	// attach source code to fragment shader
	glShaderSource(fragmentShaderObjectBlur, 1, (const GLchar**)&blurFragmentShaderSourceCode, NULL);

	// compile fragment shader source code
	glCompileShader(fragmentShaderObjectBlur);

	// compile errors
	iShaderCompileStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;

	glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(fragmentShaderObjectBlur, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(fragmentShaderObjectBlur, GL_INFO_LOG_LENGTH, &written, szInfoLog);

				fprintf(gpFile, "Blur Fragment Shader Compiler Info Log: %s", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(ghWnd);
			}
		}
	}

	// create shader program object 
	gBlurShader = glCreateProgram();

	// attach vertex shader to shader program
	glAttachShader(gBlurShader, vertexShaderObjectBlur);

	// attach fragment shader to shader program
	glAttachShader(gBlurShader, fragmentShaderObjectBlur);

	// pre-linking binding to vertex attribute
	glBindAttribLocation(gBlurShader, AMC_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(gBlurShader, AMC_ATTRIBUTE_TEXCOORD0, "vTexcoord");

	// link the shader program
	glLinkProgram(gBlurShader);

	// linking errors
	iProgramLinkStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;

	glGetProgramiv(gBlurShader, GL_LINK_STATUS, &iProgramLinkStatus);
	if (iProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gBlurShader, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(gBlurShader, GL_INFO_LOG_LENGTH, &written, szInfoLog);

				fprintf(gpFile, ("Blur Shader Program Linking Info Log: %s"), szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(ghWnd);
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////

	//// final shader /////////////////////////////////////////////////////////////////////////////

	// create vertex shader object
	GLuint vertexShaderObjectFinal = glCreateShader(GL_VERTEX_SHADER);

	// vertex shader source code 
	const GLchar* finalVertexShaderSourceCode = (GLchar*)
		"#version 450 core" \
		"\n" \

		"in vec4 vPosition;" \
		"in vec2 vTexcoord;" \

		"out vec2 TexCoord;" \

		"void main(void)" \
		"{" \
		"   TexCoord = vTexcoord;" \
		"   gl_Position = vPosition;" \
		"}";

	// attach source code to vertex shader
	glShaderSource(vertexShaderObjectFinal, 1, (const GLchar**)&finalVertexShaderSourceCode, NULL);

	// compile vertex shader source code
	glCompileShader(vertexShaderObjectFinal);

	// compilation errors 
	iShaderCompileStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;

	glGetShaderiv(vertexShaderObjectFinal, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObjectFinal, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(vertexShaderObjectFinal, GL_INFO_LOG_LENGTH, &written, szInfoLog);

				fprintf(gpFile, "Final Vertex Shader Compiler Info Log: %s", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(ghWnd);
			}
		}
	}

	// create fragment shader object
	GLuint fragmentShaderObjectFinal = glCreateShader(GL_FRAGMENT_SHADER);

	// fragment shader source code
	const GLchar* finalFragmentShaderSourceCode = (GLchar*)
		"#version 450 core" \
		"\n" \

		"in vec2 TexCoord;\n" \
		"out vec4 FragColor;\n" \

		"uniform sampler2D scene;\n" \
		"uniform sampler2D bloomBlur;\n" \

		"uniform bool bloom = true;\n" \
		"uniform float exposure = 1.2;\n" \

		"void main(void)" \
		"{" \
		"	const float gamma = 2.2;\n" \
		"	vec3 hdrColor =  texture(scene, TexCoord).rgb;\n" \
		"	vec3 bloomColor =  texture(bloomBlur, TexCoord).rgb;\n" \
		"	if(bloom) hdrColor += bloomColor;\n" \

		"	vec3 result = vec3(1.0) - exp(-hdrColor * exposure);\n" \
		"	result = pow(result, vec3(1.0/gamma));\n" \

		"	FragColor = vec4(result, 1.0);\n" \
		"}";

	// attach source code to fragment shader
	glShaderSource(fragmentShaderObjectFinal, 1, (const GLchar**)&finalFragmentShaderSourceCode, NULL);

	// compile fragment shader source code
	glCompileShader(fragmentShaderObjectFinal);

	// compile errors
	iShaderCompileStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;

	glGetShaderiv(fragmentShaderObjectFinal, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(fragmentShaderObjectFinal, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(fragmentShaderObjectFinal, GL_INFO_LOG_LENGTH, &written, szInfoLog);

				fprintf(gpFile, "Final Fragment Shader Compiler Info Log: %s", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(ghWnd);
			}
		}
	}

	// create shader program object 
	gFinalShader = glCreateProgram();

	// attach vertex shader to shader program
	glAttachShader(gFinalShader, vertexShaderObjectFinal);

	// attach fragment shader to shader program
	glAttachShader(gFinalShader, fragmentShaderObjectFinal);

	// pre-linking binding to vertex attribute
	glBindAttribLocation(gFinalShader, AMC_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(gFinalShader, AMC_ATTRIBUTE_TEXCOORD0, "vTexcoord");

	// link the shader program
	glLinkProgram(gFinalShader);

	// linking errors
	iProgramLinkStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;

	glGetProgramiv(gFinalShader, GL_LINK_STATUS, &iProgramLinkStatus);
	if (iProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gFinalShader, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(gFinalShader, GL_INFO_LOG_LENGTH, &written, szInfoLog);

				fprintf(gpFile, ("Final Shader Program Linking Info Log: %s"), szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(ghWnd);
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////
	
	////// shapes ///////////////////////////////////////////////////////////////////////////////

	getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
	gNumVertices = getNumberOfSphereVertices();
	gNumElements = getNumberOfSphereElements();

	const GLfloat cubeVertices[] = {
		/* Top */
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,

		/* Bottom */
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		/* Front */
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,

		/* Back */
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,

		/* Right */
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,

		/* Left */
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f
	};

	const GLfloat cubeNormals[] = {
		/* Top */
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,

		/* Bottom */
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,

		/* Front */
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,

		/* Back */
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,

		/* Right */
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		/* Left */
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f
	};

	const GLfloat quadVertex[] = {
		-1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 0.0f
	};

	const GLfloat quadTexcoord[] = {
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 1.0f,
		1.0f, 0.0f
	};

	// create vao
	glGenVertexArrays(1, &vaoQuad);
	glBindVertexArray(vaoQuad);

	// vertex positions
	glGenBuffers(1, &vbo_position_quad);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_quad);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertex), quadVertex, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	// vertex positions
	glGenBuffers(1, &vbo_texture_quad);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_texture_quad);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadTexcoord), quadTexcoord, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_TEXCOORD0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXCOORD0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// create vao
	glGenVertexArrays(1, &vaoCube);
	glBindVertexArray(vaoCube);

	// vertex positions
	glGenBuffers(1, &vbo_position_cube);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_cube);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	// vertex positions
	glGenBuffers(1, &vbo_normal_cube);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_normal_cube);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeNormals), cubeNormals, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// create vao
	glGenVertexArrays(1, &vao_sphere);
	glBindVertexArray(vao_sphere);

	// vertex positions
	glGenBuffers(1, &vbo_position_sphere);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_sphere);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	// vertex normals
	glGenBuffers(1, &vbo_normal_sphere);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_normal_sphere);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);

	// vertex normals
	glGenBuffers(1, &vbo_element_sphere);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element_sphere);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//// bloom and HDR /////////////////////////////////////////////////////////////


	glGenFramebuffers(1, &hdrFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

	// create 2 floating point color buffers (1 for normal, 2 for brightness)
	glGenTextures(2, colorBuffers);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WIN_WIDTH, WIN_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
	}

	// create and attach depth buffer
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WIN_WIDTH, WIN_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

	// which color attachments we will use for rendering
	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);
	
	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		fprintf(gpFile, "\nFramebuffer not complete for HDR\n");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// ping pong framebuffers for blurring
	glGenFramebuffers(2, pingpongFBO);
	glGenTextures(2, pingpongColorBuffers);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, pingpongColorBuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WIN_WIDTH, WIN_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorBuffers[i], 0);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			fprintf(gpFile, "\nFramebuffer not complete for pingpong\n");
	}


	//////////////////////////////////////////////////////////////////////

	// clear the depth buffer
	glClearDepth(1.0f);

	// enable depth
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glShadeModel(GL_SMOOTH);

	glEnable(GL_TEXTURE_2D);

	// clear the screen by OpenGL
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	perspectiveProjectionMatrix = mat4::identity();
	initLights();

	for (int i = 0; i < SIZE*SIZE; i++)
	{
		delta[i] = 2.0f * (float)rand() / (float)RAND_MAX;
		delta[i] -= 1.0f;
	}

	SetProcessDpiAwareness(PROCESS_SYSTEM_DPI_AWARE);

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
	static float time = 0.0f;

	void updateLights(void);

	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// use shader program
	glUseProgram(gShaderProgramObject);

	//declaration of matrices
	mat4 translationMatrix;
	mat4 rotationMatrix;
	mat4 modelViewMatrix;
	mat4 modelViewProjectionMatrix;

	// intialize above matrices to identity
	translationMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	modelViewMatrix = mat4::identity();

	// camera
	mat4 viewMatrix = lookat(
		vec3(6.0f*cosf(time/10.0f), 4.0f, 6.0f*sinf(time / 10.0f)),
		vec3(0.0f, 0.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f)
	);

	// send necessary matrices to shader in respective uniforms
	glUniformMatrix4fv(mUniform, 1, false, modelViewMatrix);
	glUniformMatrix4fv(vUniform, 1, false, viewMatrix);
	glUniformMatrix4fv(pUniform, 1, false, perspectiveProjectionMatrix);

	if (bLight == TRUE)
		glUniform1i(enableLightUniform, 1);
	else
		glUniform1i(enableLightUniform, 0);

	// bind with vao (this will avoid many binding to vbo_vertex)
	glBindVertexArray(vao_sphere);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element_sphere);

	for (int i = 0; i < lightCount; i++)
	{
		glUniform3fv(lightPositionUniform, 1, lightPos[i]);
		glUniform3fv(laUniform, 1, lightAmbient[i]);
		glUniform3fv(ldUniform, 1, lightDiffuse[i]);
		glUniform1f(lightStrengthUniform, lightStrength[i]);
		glUniform1f(constAttUniform, lightConstAtt[i]);
		glUniform1f(linearAttUniform, lightLinearAtt[i]);
		glUniform1f(quadAttUniform, lightQuadAtt[i]);
		glUniform1i(lightsUniform, 1);


		glUniform1f(glGetUniformLocation(gShaderProgramObject, "inv"), -1.0f);
		glUniform1i(glGetUniformLocation(gShaderProgramObject, "index"), i);
		
		glUniformMatrix4fv(mUniform, 1, false, translate(lightPos[i])*scale(0.05f, 0.05f, 0.05f));
		glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	}

	glUniform3fv(lightPositionUniform, lightCount, lightPos[0]);
	glUniform3fv(laUniform, lightCount, lightAmbient[0]);
	glUniform3fv(ldUniform, lightCount, lightDiffuse[0]);
	glUniform1fv(lightStrengthUniform, lightCount, lightStrength);
	glUniform1fv(constAttUniform, lightCount, lightConstAtt);
	glUniform1fv(linearAttUniform, lightCount, lightLinearAtt);
	glUniform1fv(quadAttUniform, lightCount, lightQuadAtt);
	glUniform1i(lightsUniform, lightCount);

	angleCube += 0.50f;
	glUniform1f(glGetUniformLocation(gShaderProgramObject, "inv"), 1.0f);

	glBindVertexArray(vaoCube);
	
	// draw necessary scene
	time += 0.01f;

	modelViewMatrix *= translate(0.0f, -2.5f, 0.0f);
	int k = 0;
	for (int i = -SIZE/2; i < SIZE/2; i++)
	{
		for (int j = -SIZE/2; j < SIZE/2; j++)
		{
			glUniformMatrix4fv(mUniform, 1, false, modelViewMatrix*translate(i*2.0f, delta[k++]*cosf(i-j+time), j*2.0f)*scale(0.85f, 0.85f, 0.85f));
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
			glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
			glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
			glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
			glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
			glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
		}
	}

	glBindVertexArray(0);
	glUseProgram(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// blur the bright fragments
	glUseProgram(gBlurShader);
	glBindVertexArray(vaoQuad);
	unsigned int amount = 20;
	bool horizontal = true, first_iter = true;
	for (unsigned int i = 0; i < amount; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
		glUniform1i(glGetUniformLocation(gBlurShader, "horizontal"), horizontal);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, first_iter ? colorBuffers[1] : pingpongColorBuffers[!horizontal]);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		horizontal = !horizontal;
		if (first_iter) first_iter = false;
	}
	glBindVertexArray(0);
	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// final render using HDR
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(gFinalShader);
	glBindVertexArray(vaoQuad);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, pingpongColorBuffers[!horizontal]);

	glUniform1i(glGetUniformLocation(gFinalShader, "bloom"), bloom);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(0);
	glUseProgram(0);

	SwapBuffers(ghDC);
	updateLights();
}

void update(void)
{
	static float t = 0.0f;

	if (t > 20)
	{
		state = (state + 1) % 3;
		t = 0.0f;
	}

	t += 0.05f;
}

void uninitialize(void)
{
	if (vbo_position_sphere)
	{
		glDeleteBuffers(1, &vbo_position_sphere);
		vbo_position_sphere = 0;
	}

	if (vbo_normal_sphere)
	{
		glDeleteBuffers(1, &vbo_normal_sphere);
		vbo_normal_sphere = 0;
	}

	if (vbo_element_sphere)
	{
		glDeleteBuffers(1, &vbo_element_sphere);
		vbo_element_sphere = 0;
	}

	if (vao_sphere)
	{
		glDeleteVertexArrays(1, &vao_sphere);
		vao_sphere = 0;
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

void initLights(void)
{
	vec3 GetRGBFromHSL(FLOAT H, FLOAT S, FLOAT L);

	vec3 pos = vec3(1.0f);
	vec3 col = vec3(1.0f);
	float angle = angleCube * 0.0174532925f;
	for (int i = 0, k = 0; i < lightCount; i++,k++)
	{
		if (k % 10 == 0)
		{
			pos = vec3(getRand(-4.0f, 4.0f), 0.2f, getRand(-4.0f, 4.0f));
			col = GetRGBFromHSL(getRand(0.0f, 360.0f), 1.0f, 0.5f);
		}

		lightPos[i] = pos;
		lightDiffuse[i] = col;

		lightVel[i] = vec3(getRand(-1.0f, 1.0f), getRand(-1.0f, 1.0f), getRand(-1.0f, 1.0f));
		lightAcc[i] = vec3(0.0f, getRand(-0.5f, -1.0f), 0.0f);

		lightAmbient[i] = vec3(0.25f);
		lightStrength[i] = 1.0f;
		lightConstAtt[i] = 1.0f;
		lightLinearAtt[i] = 1.0f;
		lightQuadAtt[i] = 4.5f;
	}
}

void updateLights(void)
{
	vec3 GetRGBFromHSL(FLOAT H, FLOAT S, FLOAT L);

	float angle = angleCube * 0.0174532925f;
	vec3 pos = vec3(1.0f);
	vec3 col = vec3(1.0f);

	for (int i = 0; i < lightCount; i++)
	{
		lightVel[i] += 0.08f*lightAcc[i];
		lightPos[i] += 0.08f*lightVel[i];

		if (lightPos[i][1] < -15.0f) 
		{
			pos = vec3(getRand(-4.0f, 4.0f), getRand(0.0f, 0.5f), getRand(-4.0f, 4.0f));
			col = GetRGBFromHSL(getRand(0.0f, 360.0f), 1.0f, 0.5f);
			//col = vec3(getRand(0.0f, 1.0f), getRand(0.0f, 1.0f), getRand(0.0f, 1.0f));

			int k = 0;
			for (k = 10*(i / 10); k < (10 * (i / 10)) + 10; k++)
			{
				lightPos[k] = pos;
				lightDiffuse[k] = col;
				
				lightVel[k] = vec3(getRand(-0.1f, 0.1f), getRand(1.3f, 1.5f), getRand(-0.1f, 0.1f));
				lightAcc[k] = vec3(getRand(-0.05f, 0.05f), getRand(-0.5f, -0.1f), getRand(-0.05f, 0.05f));
			}

			i = k - 1;
			//lightVel[i] = vec3(0.0f, getRand(1.2f, 1.5f), 0.0f);
			//lightAcc[i] = vec3(0.0f, getRand(-0.1f, -0.2f), 0.0f);

		}
	}
}


// Convert color from HSL format to RGB format
vec3 GetRGBFromHSL(FLOAT H, FLOAT S, FLOAT L)
{
	FLOAT R, G, B;
	fprintf(gpFile, "%f %f %f\n", H, S, L);

	// chroma
	FLOAT C = (1.0f - fabsf(2.0f*L - 1.0f)) * S;
	FLOAT _H = H / 60.0f;
	FLOAT X = C * (1.0f - fabsf(fmodf(_H, 2.0f) - 1.0f));

	INT Hdash = (INT)ceilf(_H);
	switch (Hdash)
	{
	case 1:
		R = C;
		G = X;
		B = 0.0f;
		break;

	case 2:
		R = X;
		G = C;
		B = 0.0f;
		break;

	case 3:
		R = 0;
		G = C;
		B = X;
		break;

	case 4:
		R = 0;
		G = X;
		B = C;
		break;

	case 5:
		R = X;
		G = 0;
		B = C;
		break;

	case 6:
		R = C;
		G = 0;
		B = X;
		break;

	default:
		R = 0.0f;
		G = 0.0f;
		B = 0.0f;
		break;
	}

	FLOAT m = L - (C / 2.0f);
	R += m;
	G += m;
	B += m;

	return vec3(R, G, B);
}

float getRand(float min, float max)
{
	float r = (float)rand() / (float)RAND_MAX;
	return(min + ((max - min) * r));
}
