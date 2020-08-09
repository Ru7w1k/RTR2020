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
	AMC_ATTRIBUTE_POSITION_MASS = 0,
	AMC_ATTRIBUTE_VELOCITY,
	AMC_ATTRIBUTE_CONNECTION_1,
	AMC_ATTRIBUTE_CONNECTION_2
};

enum BUFFER_TYPE_t
{
	POSITION_A,
	POSITION_B,
	VELOCITY_A,
	VELOCITY_B,
	CONNECTION_1
};

enum
{
	POINTS_X          = 50,
	POINTS_Y          = 50,
	POINTS_TOTAL      = POINTS_X * POINTS_Y,
	CONNECTIONS_TOTAL = (POINTS_X - 1) * POINTS_Y + (POINTS_Y - 1) * POINTS_X
};

// Global Variables
FILE  *gpFile = NULL;
bool  gbActiveWindow = false;
bool  gbIsFullScreen = false;
bool  bWind = false;
HDC   ghDC = NULL;
HGLRC ghRC = NULL;
HWND  ghWnd = NULL;
DWORD dwStyle;
int gWidth;
int gHeight;

WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };

GLuint gUpdateShaderProgram;
GLuint gRenderShaderProgram;

GLuint vao[2];			// vertex array object
GLuint vbo[6];			// vertex buffer object
GLuint indexBuffer;
GLuint pos_tbo[2];

GLuint texPositionUniform;	

mat4   perspectiveProjectionMatrix;
int iterations_per_frame = 1;

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
		TEXT("OpenGL | Transform Feedback"),
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

	case WM_CHAR:
		switch (wParam)
		{
		case 'W':
		case 'w':
			bWind = !bWind;
			break;

		case '+':
			if (iterations_per_frame < 2000)
				iterations_per_frame += 50;
			break;

		case '-':
			if (iterations_per_frame > 0)
				iterations_per_frame--;
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
			ToggleFullScreen();
			break;
		}
		break;

		// returned from here, to block DefWindowProc
		// We have our own painter
	case WM_ERASEBKGND:
		return(0);

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

	//// Programable Pipeline ////////////////////////////////////////////

	result = glewInit();
	if (result != GLEW_OK) {
		fprintf(gpFile, "GLEW initialization failed..\n");
		uninitialize();
		DestroyWindow(ghWnd);
	}

	//////// UPDATE SHADER ////////////////////////////////////////////////////////////////////////////////

	// create vertex shader object
	vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	// vertex shader source code 
	const GLchar* vertexShaderSourceCode = (GLchar*)
		"#version 450 core" \
		"\n" \

		// this input vector contains the vertex posisiton in xyz,
		// and the mass of the vertex in w
		"in vec4 position_mass;" \

		// this is the current velocity of the vertex
		"in vec3 velocity;" \

		// this is our connection vector
		"in ivec4 connection_1;" \
		"in ivec4 connection_2;" \

		// this is TBO that will be bound to the same buffer as the position_mass input attribute
		"uniform samplerBuffer tex_position;" \

		// the output of the vertex shader are the same as inputs
		"out vec4 tf_position_mass;" \
		"out vec3 tf_velocity;" \

		// a uniform hold the timestep. the application can update this
		"uniform float t = 0.000005;" \
		
		// the global spring constant
		"uniform float k = 2000.0;" \

		// gravity 
		"uniform vec3 gravity = vec3(0.0, -0.08, 0.0);" \

		// global damping constant
		"uniform float c = 0.1;" \

		// spring resting length
		"uniform float rest_length = 0.88;" \

		"void main()" \
		"{" \
		"	vec3 p = position_mass.xyz;    /* p can be our position */" \
		"	float m = position_mass.w;     /* m is the mass of our vertex */ " \
		"	vec3 u = velocity;             /* u is the initial velocity */" \
		"	vec3 F = vec3(0.0, -10.00, 0.0) * m - c * u;  /* F is the force on the mass */" \
		"	bool fixed_node = true;        /* becomes false when force is applied */" \

		"   F += gravity; " \

		"	for (int i = 0; i < 4; i++)" \
		"	{" \
		"		if (connection_1[i] != -1)" \
		"		{" \
					// q is the position of the other vertex
		"			vec3 q = texelFetch(tex_position, connection_1[i]).xyz;" \
		"			vec3 d = q - p;" \
		"			float x = length(d);" \
		"			F += -k * (rest_length - x) * normalize(d);" \
		"			fixed_node = false;" \
		"		}" \

		"		if (connection_2[i] != -1)" \
		"		{" \
					// q is the position of the other vertex
		"			vec3 q = texelFetch(tex_position, connection_2[i]).xyz;" \
		"			vec3 d = q - p;" \
		"			float x = length(d);" \
		"			F += -k * ((1.414 * rest_length) - x) * normalize(d);" \
		"			fixed_node = false;" \
		"		}" \
		"	}" \

			// if this is a fixed node, reset the force to zero 
		"	if (fixed_node)" \
		"	{" \
		"		F = vec3(0.0);" \
		"	}" \

			// acceleration due to force
		"	vec3 a = F / m;" \

			// displacement
		"	vec3 s = u * t + 0.5 * a * t * t;" \

			// final velocity
		"	vec3 v = u + a * t;" \

			// constrain the absolute value of the displacement per step
		"	s = clamp(s, vec3(-25.0), vec3(25.0));" \

			// write the outputs
		"	tf_position_mass = vec4(p + s, m);" \
		"	tf_velocity = v;" \
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

	// create shader program object 
	gUpdateShaderProgram = glCreateProgram();

	// attach vertex shader to shader program
	glAttachShader(gUpdateShaderProgram, vertexShaderObject);

	// pre-linking binding to vertex attribute
	glBindAttribLocation(gUpdateShaderProgram, AMC_ATTRIBUTE_POSITION_MASS, "position_mass");
	glBindAttribLocation(gUpdateShaderProgram, AMC_ATTRIBUTE_VELOCITY, "velocity");
	glBindAttribLocation(gUpdateShaderProgram, AMC_ATTRIBUTE_CONNECTION_1, "connection_1");
	glBindAttribLocation(gUpdateShaderProgram, AMC_ATTRIBUTE_CONNECTION_2, "connection_2");

	// transform feedback things
	static const char* tf_varyings[] =
	{
		"tf_position_mass",
		"tf_velocity"
	};

	glTransformFeedbackVaryings(gUpdateShaderProgram, 2, tf_varyings, GL_SEPARATE_ATTRIBS);

	// link the shader program
	glLinkProgram(gUpdateShaderProgram);

	// linking errors
	GLint iProgramLinkStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;

	glGetProgramiv(gUpdateShaderProgram, GL_LINK_STATUS, &iProgramLinkStatus);
	if (iProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gUpdateShaderProgram, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(gUpdateShaderProgram, GL_INFO_LOG_LENGTH, &written, szInfoLog);

				fprintf(gpFile, ("Update Shader Program Linking Info Log: %s"), szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(ghWnd);
			}
		}
	}

	// post-linking retrieving uniform locations
	texPositionUniform = glGetUniformLocation(gUpdateShaderProgram, "tex_position");

	////////// RENDER SHADER //////////////////////////////////////////////////////////////////////////////////////

	// create vertex shader object
	vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	// vertex shader source code 
	vertexShaderSourceCode = (GLchar*)
		"#version 450 core" \
		"\n" \

		"in vec3 position;" \

		"void main()" \
		"{" \
		"	gl_Position = vec4(position * 0.01, 1.0);" \
		"}";

	// attach source code to vertex shader
	glShaderSource(vertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);

	// compile vertex shader source code
	glCompileShader(vertexShaderObject);

	// compilation errors 
	iShaderCompileStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;

	glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(vertexShaderObject, GL_INFO_LOG_LENGTH, &written, szInfoLog);

				fprintf(gpFile, "Render Vertex Shader Compiler Info Log: %s", szInfoLog);
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

		"void main (void)" \
		"{" \
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

				fprintf(gpFile, "Render Fragment Shader Compiler Info Log: %s", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(ghWnd);
			}
		}
	}

	// create shader program object 
	gRenderShaderProgram = glCreateProgram();

	// attach vertex shader to shader program
	glAttachShader(gRenderShaderProgram, vertexShaderObject);

	// attach fragment shader to shader program
	glAttachShader(gRenderShaderProgram, fragmentShaderObject);

	// pre-linking binding to vertex attribute
	glBindAttribLocation(gRenderShaderProgram, AMC_ATTRIBUTE_POSITION_MASS, "position");

	// link the shader program
	glLinkProgram(gRenderShaderProgram);

	// linking errors
	iProgramLinkStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;

	glGetProgramiv(gRenderShaderProgram, GL_LINK_STATUS, &iProgramLinkStatus);
	if (iProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gRenderShaderProgram, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(gRenderShaderProgram, GL_INFO_LOG_LENGTH, &written, szInfoLog);

				fprintf(gpFile, ("Render Shader Program Linking Info Log: %s"), szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(ghWnd);
			}
		}
	}

	///////////// INITIAL MESH SETUP ////////////////////////////////////////////////

	int i, j;

	vec4 *initial_positions = new vec4[POINTS_TOTAL];
	vec3 *initial_velocities = new vec3[POINTS_TOTAL];
	ivec4 *connection_vectors_1 = new ivec4[POINTS_TOTAL];
	ivec4 *connection_vectors_2 = new ivec4[POINTS_TOTAL];

	int n = 0;

	for (j = 0; j < POINTS_Y; j++)
	{
		float fj = (float)j / (float)POINTS_Y;
		for (i = 0; i < POINTS_X; i++)
		{
			float fi = (float)i / (float)POINTS_X;

			initial_positions[n] = vec4((fi - 0.5f) * (float)POINTS_X,
				                        20.0f,
				                        (fj - 0.5f) * (float)POINTS_Y,
				                        1.0);

			initial_velocities[n] = vec3(0.0f);
			connection_vectors_1[n] = ivec4(-1);
			connection_vectors_2[n] = ivec4(-1);

			// straight connections (up down left right)
			if (j != (POINTS_Y - 1) || i % 7 != 0)
			{
				if (i != 0)
					connection_vectors_1[n][0] = n - 1;

				if (j != 0)
					connection_vectors_1[n][1] = n - POINTS_X;

				if (i != (POINTS_X - 1))
					connection_vectors_1[n][2] = n + 1;

				if (j != (POINTS_Y - 1))
					connection_vectors_1[n][3] = n + POINTS_X;

				// diagonal connections 
				if (i > 0 && j > 0) /* lower left */
					connection_vectors_2[n][0] = n - 1 - POINTS_X;

				if (i < (POINTS_X - 1) && j < (POINTS_Y - 1)) /* upper right */
					connection_vectors_2[n][1] = n + 1 + POINTS_X;

				if (i < (POINTS_X - 1) && j > 0) /* lower right */
					connection_vectors_2[n][2] = n + 1 - POINTS_X;

				if (i > 0 && j < (POINTS_Y - 1)) /* upper left */
					connection_vectors_2[n][3] = n - 1 + POINTS_X;
			}

			n++;

		}
	}

	// create vaos and vbos
	glGenVertexArrays(2, vao);
	glGenBuffers(6, vbo);

	for (i = 0; i < 2; i++)
	{
		glBindVertexArray(vao[i]);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[POSITION_A + i]);
		glBufferData(GL_ARRAY_BUFFER, POINTS_TOTAL * sizeof(vec4), initial_positions, GL_DYNAMIC_COPY);
		glVertexAttribPointer(AMC_ATTRIBUTE_POSITION_MASS, 4, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION_MASS);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[VELOCITY_A + i]);
		glBufferData(GL_ARRAY_BUFFER, POINTS_TOTAL * sizeof(vec3), initial_velocities, GL_DYNAMIC_COPY);
		glVertexAttribPointer(AMC_ATTRIBUTE_VELOCITY, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(AMC_ATTRIBUTE_VELOCITY);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[CONNECTION_1]);
		glBufferData(GL_ARRAY_BUFFER, POINTS_TOTAL * sizeof(ivec4), connection_vectors_1, GL_DYNAMIC_COPY);
		glVertexAttribIPointer(AMC_ATTRIBUTE_CONNECTION_1, 4, GL_INT, 0, NULL);
		glEnableVertexAttribArray(AMC_ATTRIBUTE_CONNECTION_1);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[CONNECTION_1 + 1]);
		glBufferData(GL_ARRAY_BUFFER, POINTS_TOTAL * sizeof(ivec4), connection_vectors_2, GL_DYNAMIC_COPY);
		glVertexAttribIPointer(AMC_ATTRIBUTE_CONNECTION_2, 4, GL_INT, 0, NULL);
		glEnableVertexAttribArray(AMC_ATTRIBUTE_CONNECTION_2);
	}

	delete[] initial_positions;
	delete[] initial_velocities;
	delete[] connection_vectors_1;

	glGenTextures(2, pos_tbo);
	
	glBindTexture(GL_TEXTURE_BUFFER, pos_tbo[0]);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, vbo[POSITION_A]);

	glBindTexture(GL_TEXTURE_BUFFER, pos_tbo[1]);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, vbo[POSITION_B]);

	int lines = (POINTS_X - 1) * POINTS_Y + (POINTS_Y - 1) * POINTS_X;

	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, lines * 2 * sizeof(int), NULL, GL_STATIC_DRAW);

	int* e = (int*)glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, lines * 2 * sizeof(int), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

	for (j = 0; j < POINTS_Y; j++)
	{
		for (i = 0; i < POINTS_X - 1; i++)
		{
			*e++ = i + j * POINTS_X;
			*e++ = 1 + i + j * POINTS_X;
		}
	}

	for (i = 0; i < POINTS_X; i++)
	{
		for (j = 0; j < POINTS_Y - 1; j++)
		{
			*e++ = i + j * POINTS_X;
			*e++ = POINTS_X + i + j * POINTS_X;
		}
	}

	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

	//////////////////////////////////////////////////////////////////////

	// clear the depth buffer
	glClearDepth(1.0f);

	// enable depth
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// face culling
	// glEnable(GL_CULL_FACE);
	// glCullFace(GL_FRONT);

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

	gWidth = width;
	gHeight = height;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	//perspectiveProjectionMatrix = perspective(50.0, (float)width / (float)height, 1.0f, 100.0f);
}


void display(void)
{
	int i;
	static int iteration_index = 0;
	static float t = 0.0f;

	glUseProgram(gUpdateShaderProgram);

	if (bWind)
	{
		glUniform3fv(glGetUniformLocation(gUpdateShaderProgram, "gravity"),
			1, vec3(0.0f, -10.08f, 10.25f));
	} 
	else
	{
		glUniform3fv(glGetUniformLocation(gUpdateShaderProgram, "gravity"),
			1, vec3(0.0f, -10.08f, 0.0f));
	}


	glEnable(GL_RASTERIZER_DISCARD);

	for (i = iterations_per_frame; i != 0; --i)
	{
		glBindVertexArray(vao[iteration_index & 1]);
		glBindTexture(GL_TEXTURE_BUFFER, pos_tbo[iteration_index & 1]);
		iteration_index++;

		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, vbo[POSITION_A + (iteration_index & 1)]);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, vbo[VELOCITY_A + (iteration_index & 1)]);

		glBeginTransformFeedback(GL_POINTS);
		glDrawArrays(GL_POINTS, 0, POINTS_TOTAL);
		glEndTransformFeedback();
	}

	glDisable(GL_RASTERIZER_DISCARD);

	glViewport(0, 0, (GLsizei)gWidth, (GLsizei)gHeight);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(gRenderShaderProgram);
	
	/*glPointSize(4.0f);
	glDrawArrays(GL_POINTS, 0, POINTS_TOTAL);*/

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glDrawElements(GL_LINES, CONNECTIONS_TOTAL * 2, GL_UNSIGNED_INT, NULL);

	SwapBuffers(ghDC);
	t += 0.001f;
}

void update(void)
{
	

}

void uninitialize(void)
{
	
	if (vbo)
	{
		glDeleteBuffers(2, vbo);
	}

	if (vao)
	{
		glDeleteVertexArrays(5, vao);
	}

	if (gUpdateShaderProgram)
	{
		GLsizei shaderCount;
		GLsizei shaderNumber;

		glUseProgram(gUpdateShaderProgram);
		glGetProgramiv(gUpdateShaderProgram, GL_ATTACHED_SHADERS, &shaderCount);

		GLuint *pShaders = (GLuint *)malloc(sizeof(GLuint) * shaderCount);
		if (pShaders)
		{
			glGetAttachedShaders(gUpdateShaderProgram, shaderCount, &shaderCount, pShaders);

			for (shaderNumber = 0; shaderNumber < shaderCount; shaderNumber++)
			{
				// detach shader
				glDetachShader(gUpdateShaderProgram, pShaders[shaderNumber]);

				// delete shader
				glDeleteShader(pShaders[shaderNumber]);
				pShaders[shaderNumber] = 0;
			}
			free(pShaders);
		}

		glDeleteProgram(gUpdateShaderProgram);
		gUpdateShaderProgram = 0;
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

