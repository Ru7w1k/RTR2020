// headers
#include <stdio.h>
#include <windows.h>

#include <gl/glew.h>

#include <CL/opencl.h>

#include "OGLTemplate.h"
#include "vmath.h"

#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "opencl.lib")

// macros
#define WIN_WIDTH 800
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
HDC ghdc = NULL;
HGLRC ghrc = NULL;

bool gbFullscreen = false;
bool gbActiveWindow = false;

HWND ghwnd = NULL;
FILE *gpFile = NULL;

DWORD dwStyle;
WINDOWPLACEMENT wpPrev = {sizeof(WINDOWPLACEMENT)};

const int meshWidth = 2048;
const int meshHeight = 2048;

#define MY_ARRAY_SIZE (meshWidth * meshHeight * 4)
float pos[meshWidth][meshHeight][4];

GLuint gShaderProgramObject;

GLuint vao;
GLuint vbo;
GLuint vbo_gpu;
GLuint mvpMatrixUniform;

mat4 perspectiveProjectionMatrix;
float animationTime = 0.0f;
bool bOnGPU;

// OpenCL variables
cl_int oclResult;                 // return value of OpenCL calls
cl_mem oclGraphicsResource;       // interop resource
cl_device_id oclDeviceID;         // compute device id
cl_context oclContext;            // compute context
cl_command_queue oclCommandQueue; // compute command queue
cl_program oclProgram;            // compute program
cl_kernel oclKernel;              // compute kernel

// OpenCL kernel variables
char *oclSourceCode = NULL;
size_t sizeKernelCodeLength;

// WinMain()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpszCmdLine, int iCmdShow) {
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
  if (fopen_s(&gpFile, "RMCLog.txt", "w") != 0) {
    MessageBox(NULL, TEXT("Cannot open RMCLog.txt file.."), TEXT("Error"),
               MB_OK | MB_ICONERROR);
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
  hwnd = CreateWindowEx(
      WS_EX_APPWINDOW, szAppName, TEXT("OpenCL + OpenGL | Sinewave"),
      WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
      (width / 2) - 400, (height / 2) - 300, WIN_WIDTH, WIN_HEIGHT, NULL, NULL,
      hInstance, NULL);

  ghwnd = hwnd;

  initialize();

  ShowWindow(hwnd, iCmdShow);
  SetForegroundWindow(hwnd);
  SetFocus(hwnd);

  // Game Loop!
  while (bDone == false) {
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
      if (msg.message == WM_QUIT)
        bDone = true;
      else {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }
    } else {
      if (gbActiveWindow == true) {
        // call update() here for OpenGL rendering
        update();
        // call display() here for OpenGL rendering
        display();
      }
    }
  }

  return ((int)msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {
  // function declaration
  void display(void);
  void resize(int, int);
  void uninitialize();
  void ToggleFullscreen(void);

  // code
  switch (iMsg) {

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
    switch (wParam) {
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

  case WM_CHAR:
    switch (wParam) {
    case 'c':
    case 'C':
      bOnGPU = false;
      break;

    case 'g':
    case 'G':
      bOnGPU = true;
      break;
    }
    break;

  case WM_ERASEBKGND:
    return (0);

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

  return (DefWindowProc(hwnd, iMsg, wParam, lParam));
}

void ToggleFullscreen(void) {
  // local variables
  MONITORINFO mi = {sizeof(MONITORINFO)};

  // code
  if (gbFullscreen == false) {
    dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
    if (dwStyle & WS_OVERLAPPEDWINDOW) {
      if (GetWindowPlacement(ghwnd, &wpPrev) &&
          GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi)) {
        SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
        SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top,
                     mi.rcMonitor.right - mi.rcMonitor.left,
                     mi.rcMonitor.bottom - mi.rcMonitor.top,
                     SWP_NOZORDER | SWP_FRAMECHANGED);
      }
    }
    ShowCursor(FALSE);
    gbFullscreen = true;
  } else {
    SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
    SetWindowPlacement(ghwnd, &wpPrev);
    SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0,
                 SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE |
                     SWP_NOOWNERZORDER);
    ShowCursor(TRUE);
    gbFullscreen = false;
  }
}

void initialize(void) {
  // function declarations
  void resize(int, int);
  void uninitialize(void);
  char *loadOclProgramSource(const char *, const char *, size_t *);

  // variable declarations
  PIXELFORMATDESCRIPTOR pfd;
  int iPixelFormatIndex;

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
  pfd.cDepthBits = 32;

  iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
  if (iPixelFormatIndex == 0) {
    fprintf(gpFile, "ChoosePixelFormat() failed..\n");
    DestroyWindow(ghwnd);
  }

  if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE) {
    fprintf(gpFile, "SetPixelFormat() failed..\n");
    DestroyWindow(ghwnd);
  }

  ghrc = wglCreateContext(ghdc);
  if (ghrc == NULL) {
    fprintf(gpFile, "wglCreateContext() failed..\n");
    DestroyWindow(ghwnd);
  }

  if (wglMakeCurrent(ghdc, ghrc) == FALSE) {
    fprintf(gpFile, "wglMakeCurrent() failed..\n");
    DestroyWindow(ghwnd);
  }

  // glew initialization for programmable pipeline
  GLenum glew_error = glewInit();
  if (glew_error != GLEW_OK) {
    fprintf(gpFile, "glewInit() failed..\n");
    DestroyWindow(ghwnd);
  }

  // fetch OpenGL related details
  fprintf(gpFile, "OpenGL Vendor:   %s\n", glGetString(GL_VENDOR));
  fprintf(gpFile, "OpenGL Renderer: %s\n", glGetString(GL_RENDERER));
  fprintf(gpFile, "OpenGL Version:  %s\n", glGetString(GL_VERSION));
  fprintf(gpFile, "GLSL Version:    %s\n",
          glGetString(GL_SHADING_LANGUAGE_VERSION));

  // fetch OpenGL enabled extensions
  GLint numExtensions;
  glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);

  fprintf(gpFile, "==== OpenGL Extensions ====\n");
  for (int i = 0; i < numExtensions; i++) {
    fprintf(gpFile, "  %s\n", glGetStringi(GL_EXTENSIONS, i));
  }
  fprintf(gpFile, "===========================\n\n");

  //// OpenCL initialization ///////////////////////////////////////////////////

  // 1.
  cl_platform_id platformID;
  cl_device_id *deviceIDs;
  cl_uint devCount;

  // 2.
  oclResult = clGetPlatformIDs(1, &platformID, NULL);
  if (oclResult != CL_SUCCESS) {
    fprintf(gpFile,
            "OpenCL Error - clGetPlatformIDs() Failed : %d. Exitting Now ...\n",
            oclResult);
    uninitialize();
    exit(EXIT_FAILURE);
  }

  // 3.
  oclResult =
      clGetDeviceIDs(platformID, CL_DEVICE_TYPE_GPU, 0, NULL, &devCount);
  if (oclResult != CL_SUCCESS) {

    fprintf(gpFile,
            "OpenCL Error - clGetDeviceIDs() Failed : %d. Exitting Now ...\n",
            oclResult);
    uninitialize();
    exit(EXIT_FAILURE);

  } else if (devCount == 0) {
    fprintf(gpFile, "No OpenCL device found. Exitting Now ...\n", oclResult);
    uninitialize();
    exit(EXIT_FAILURE);

  } else {
    deviceIDs = (cl_device_id *)malloc(devCount * sizeof(cl_device_id));
    oclResult = clGetDeviceIDs(platformID, CL_DEVICE_TYPE_GPU, devCount,
                               deviceIDs, NULL);
    if (oclResult != CL_SUCCESS) {
      fprintf(gpFile,
              "OpenCL Error - clGetDeviceIDs() Failed : %d. Exitting Now ...\n",
              oclResult);
      uninitialize();
      exit(EXIT_FAILURE);
    }

    oclDeviceID = deviceIDs[0];
    free(deviceIDs);
  }

  // 4.
  cl_context_properties contextProperties[] = {
      CL_GL_CONTEXT_KHR,
      (cl_context_properties)wglGetCurrentContext(),
      CL_WGL_HDC_KHR,
      (cl_context_properties)wglGetCurrentDC(),
      CL_CONTEXT_PLATFORM,
      (cl_context_properties)platformID,
      0};

  // 5.
  oclContext = clCreateContext(contextProperties, 1, &oclDeviceID, NULL, NULL,
                               &oclResult);
  if (oclResult != CL_SUCCESS) {

    fprintf(gpFile,
            "OpenCL Error - clCreateContext() Failed : %d. Exitting Now ...\n",
            oclResult);
    uninitialize();
    exit(EXIT_FAILURE);
  }

  // 6.
  oclCommandQueue = clCreateCommandQueueWithProperties(oclContext, oclDeviceID,
                                                       0, &oclResult);
  if (oclResult != CL_SUCCESS) {

    fprintf(gpFile,
            "OpenCL Error - clCreateCommandQueueWithProperties() Failed : %d. "
            "Exitting Now ...\n",
            oclResult);
    uninitialize();
    exit(EXIT_FAILURE);
  }

  // 7.
  oclSourceCode =
      loadOclProgramSource("sinewave.cl", "", &sizeKernelCodeLength);

  cl_int status = 0;
  oclProgram =
      clCreateProgramWithSource(oclContext, 1, (const char **)&oclSourceCode,
                                &sizeKernelCodeLength, &oclResult);
  if (oclResult != CL_SUCCESS) {
    fprintf(gpFile,
            "OpenCL Error - clCreateProgramWithSource() Failed : %d. Exitting "
            "Now ...\n",
            oclResult);
    uninitialize();
    exit(0);
  }

  // 8.
  oclResult =
      clBuildProgram(oclProgram, 0, NULL, "-cl-fast-relaxed-math", NULL, NULL);
  if (oclResult != CL_SUCCESS) {
    fprintf(gpFile,
            "OpenCL Error - clBuildProgram() Failed : %d. Exitting Now ...\n",
            oclResult);

    size_t len;
    char buffer[2048];
    clGetProgramBuildInfo(oclProgram, oclDeviceID, CL_PROGRAM_BUILD_LOG,
                          sizeof(buffer), buffer, &len);
    fprintf(gpFile, "OpenCL Program Build Log : %s\n", buffer);

    uninitialize();
    exit(EXIT_FAILURE);
  }

  // 9.
  oclKernel = clCreateKernel(oclProgram, "sinewave", &oclResult);
  if (oclResult != CL_SUCCESS) {
    fprintf(gpFile,
            "OpenCL Error - clCreateKernel() Failed : %d. Exitting Now ...\n",
            oclResult);
    uninitialize();
    exit(EXIT_FAILURE);
  }

  //////////////////////////////////////////////////////////////////////////////

  //// vertex shader
  // create shader
  GLuint gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

  // provide source code to shader
  const GLchar *vertexShaderSourceCode =
      "#version 450 core \n"

      "in vec4 vPosition; \n"
      "uniform mat4 u_mvpMatrix; \n"

      "void main (void) \n"
      "{ \n"
      "	gl_Position = u_mvpMatrix * vPosition; \n"
      "	gl_PointSize = 2.5f; \n"
      "} \n";

  glShaderSource(gVertexShaderObject, 1,
                 (const GLchar **)&vertexShaderSourceCode, NULL);

  // compile shader
  glCompileShader(gVertexShaderObject);

  // compilation errors
  GLint iShaderCompileStatus = 0;
  GLint iInfoLogLength = 0;
  GLchar *szInfoLog = NULL;

  glGetShaderiv(gVertexShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);
  if (iShaderCompileStatus == GL_FALSE) {
    glGetShaderiv(gVertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
    if (iInfoLogLength > 0) {
      szInfoLog = (GLchar *)malloc(iInfoLogLength);
      if (szInfoLog != NULL) {
        GLsizei written;
        glGetShaderInfoLog(gVertexShaderObject, GL_INFO_LOG_LENGTH, &written,
                           szInfoLog);

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
      "#version 450 core \n"

      "out vec4 FragColor; \n"

      "void main (void) \n"
      "{ \n"
      "	FragColor = vec4(0.0, 1.0, 0.0, 0.2); \n"
      "} \n";

  glShaderSource(gFragmentShaderObject, 1,
                 (const GLchar **)&fragmentShaderSourceCode, NULL);

  // compile shader
  glCompileShader(gFragmentShaderObject);

  // compile errors
  iShaderCompileStatus = 0;
  iInfoLogLength = 0;
  szInfoLog = NULL;

  glGetShaderiv(gFragmentShaderObject, GL_COMPILE_STATUS,
                &iShaderCompileStatus);
  if (iShaderCompileStatus == GL_FALSE) {
    glGetShaderiv(gFragmentShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
    if (iInfoLogLength > 0) {
      szInfoLog = (GLchar *)malloc(iInfoLogLength);
      if (szInfoLog != NULL) {
        GLsizei written;
        glGetShaderInfoLog(gFragmentShaderObject, GL_INFO_LOG_LENGTH, &written,
                           szInfoLog);

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
  glBindAttribLocation(gShaderProgramObject, RMC_ATTRIBUTE_POSITION,
                       "vPosition");

  // link shader
  glLinkProgram(gShaderProgramObject);

  // linking errors
  GLint iProgramLinkStatus = 0;
  iInfoLogLength = 0;
  szInfoLog = NULL;

  glGetProgramiv(gShaderProgramObject, GL_LINK_STATUS, &iProgramLinkStatus);
  if (iProgramLinkStatus == GL_FALSE) {
    glGetProgramiv(gShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
    if (iInfoLogLength > 0) {
      szInfoLog = (GLchar *)malloc(iInfoLogLength);
      if (szInfoLog != NULL) {
        GLsizei written;
        glGetProgramInfoLog(gShaderProgramObject, GL_INFO_LOG_LENGTH, &written,
                            szInfoLog);

        fprintf(gpFile, ("Shader Program Linking Info Log: \n%s\n"), szInfoLog);
        free(szInfoLog);
        DestroyWindow(ghwnd);
      }
    }
  }

  // post-linking retrieving uniform locations
  mvpMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_mvpMatrix");

  // initialize pos array
  for (int i = 0; i < meshWidth; i++) {
    for (int j = 0; j < meshHeight; j++) {
      for (int k = 0; k < 4; k++) {
        pos[i][j][k] = 0.0f;
      }
    }
  }

  // create vao
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, MY_ARRAY_SIZE * sizeof(float), NULL,
               GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // vbo for opencl + opengl
  glGenBuffers(1, &vbo_gpu);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_gpu);
  glBufferData(GL_ARRAY_BUFFER, MY_ARRAY_SIZE * sizeof(float), NULL,
               GL_DYNAMIC_DRAW);

  oclGraphicsResource =
      clCreateFromGLBuffer(oclContext, CL_MEM_WRITE_ONLY, vbo_gpu, &oclResult);
  if (oclResult != CL_SUCCESS) {
    fprintf(
        gpFile,
        "OpenCL Error - clCreateFromGLBuffer() Failed : %d. Exitting Now ...\n",
        oclResult);
    uninitialize();
    exit(EXIT_FAILURE);
  }

  glBindVertexArray(0);

  // set clear color
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  // set clear depth
  glClearDepth(1.0f);

  // depth test
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  // blend
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  perspectiveProjectionMatrix = mat4::identity();

  // warm-up resize call
  resize(WIN_WIDTH, WIN_HEIGHT);
}

void resize(int width, int height) {
  // code
  if (height == 0)
    height = 1;

  glViewport(0, 0, (GLsizei)width, (GLsizei)height);

  perspectiveProjectionMatrix =
      vmath::perspective(45.0f, (float)width / (float)height, 0.1f, 100.0f);
}

void display(void) {
  // function declarations
  void launchCPUKernel(float);
  void uninitialize(void);

  // code
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // start using OpenGL program object
  glUseProgram(gShaderProgramObject);

  // declaration of matrices
  mat4 translateMatrix;
  mat4 modelViewMatrix;
  mat4 modelViewProjectionMatrix;

  size_t globalWorkSize[2];

  // intialize above matrices to identity
  translateMatrix = mat4::identity();
  modelViewMatrix = mat4::identity();
  modelViewProjectionMatrix = mat4::identity();

  // transformations
  translateMatrix = translate(0.0f, 0.0f, -2.0f);
  modelViewMatrix = translateMatrix;

  // do necessary matrix multiplication
  modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

  // send necessary matrices to shader in respective uniforms
  glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

  // bind with vao (this will avoid many binding to vbo_vertex)
  glBindVertexArray(vao);

  if (bOnGPU) {

    oclResult = clSetKernelArg(oclKernel, 0, sizeof(cl_mem),
                               (void *)&oclGraphicsResource);
    if (oclResult != CL_SUCCESS) {
      fprintf(gpFile,
              "OpenCL Error - clCreateFromGLBuffer() Failed : %d. Exitting Now "
              "...\n",
              oclResult);
      uninitialize();
      exit(EXIT_FAILURE);
    }

    oclResult =
        clSetKernelArg(oclKernel, 1, sizeof(unsigned int), (void *)&meshWidth);
    if (oclResult != CL_SUCCESS) {
      fprintf(gpFile,
              "OpenCL Error - clCreateFromGLBuffer() Failed : %d. Exitting Now "
              "...\n",
              oclResult);
      uninitialize();
      exit(EXIT_FAILURE);
    }

    oclResult =
        clSetKernelArg(oclKernel, 2, sizeof(unsigned int), (void *)&meshHeight);
    if (oclResult != CL_SUCCESS) {
      fprintf(gpFile,
              "OpenCL Error - clCreateFromGLBuffer() Failed : %d. Exitting Now "
              "...\n",
              oclResult);
      uninitialize();
      exit(EXIT_FAILURE);
    }

    oclResult =
        clSetKernelArg(oclKernel, 3, sizeof(float), (void *)&animationTime);
    if (oclResult != CL_SUCCESS) {
      fprintf(gpFile,
              "OpenCL Error - clCreateFromGLBuffer() Failed : %d. Exitting Now "
              "...\n",
              oclResult);
      uninitialize();
      exit(EXIT_FAILURE);
    }

    oclResult = clEnqueueAcquireGLObjects(oclCommandQueue, 1,
                                          &oclGraphicsResource, 0, NULL, NULL);
    if (oclResult != CL_SUCCESS) {
      fprintf(gpFile,
              "OpenCL Error - clCreateFromGLBuffer() Failed : %d. Exitting Now "
              "...\n",
              oclResult);
      uninitialize();
      exit(EXIT_FAILURE);
    }

    globalWorkSize[0] = meshWidth;
    globalWorkSize[1] = meshHeight;
    oclResult = clEnqueueNDRangeKernel(oclCommandQueue, oclKernel, 2, NULL,
                                       globalWorkSize, 0, 0, NULL, NULL);
    if (oclResult != CL_SUCCESS) {
      fprintf(gpFile,
              "OpenCL Error - clCreateFromGLBuffer() Failed : %d. Exitting Now "
              "...\n",
              oclResult);
      uninitialize();
      exit(EXIT_FAILURE);
    }

    oclResult = clEnqueueReleaseGLObjects(oclCommandQueue, 1,
                                          &oclGraphicsResource, 0, NULL, NULL);
    if (oclResult != CL_SUCCESS) {
      fprintf(gpFile,
              "OpenCL Error - clCreateFromGLBuffer() Failed : %d. Exitting Now "
              "...\n",
              oclResult);
      uninitialize();
      exit(EXIT_FAILURE);
    }

    clFinish(oclCommandQueue);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_gpu);

  } else {
    launchCPUKernel(animationTime);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, MY_ARRAY_SIZE * sizeof(float), pos,
                 GL_DYNAMIC_DRAW);
  }

  glVertexAttribPointer(RMC_ATTRIBUTE_POSITION, 4, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(RMC_ATTRIBUTE_POSITION);

  // draw necessary scene
  glDrawArrays(GL_POINTS, 0, meshWidth * meshHeight);

  // unbind vao
  glBindVertexArray(0);

  // stop using OpenGL program object
  glUseProgram(0);

  SwapBuffers(ghdc);
  animationTime += 0.01f;
}

void update(void) {
  // code
}

void uninitialize(void) {
  // code
  if (gbFullscreen == true) {
    dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
    SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
    SetWindowPlacement(ghwnd, &wpPrev);
    SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0,
                 SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE |
                     SWP_NOOWNERZORDER);

    ShowCursor(TRUE);
  }

  if (vao) {
    glDeleteVertexArrays(1, &vao);
    vao = 0;
  }

  if (vbo) {
    glDeleteBuffers(1, &vbo);
    vbo = 0;
  }

  if (vbo_gpu) {
    glDeleteBuffers(1, &vbo_gpu);
    vbo_gpu = 0;
  }

  // OpenCL cleanup
  if (oclGraphicsResource) {
    clReleaseMemObject(oclGraphicsResource);
    oclGraphicsResource = NULL;
  }

  if (oclSourceCode) {
    free((void *)oclSourceCode);
    oclSourceCode = NULL;
  }

  if (oclKernel) {
    clReleaseKernel(oclKernel);
    oclKernel = NULL;
  }

  if (oclProgram) {
    clReleaseProgram(oclProgram);
    oclProgram = NULL;
  }

  if (oclCommandQueue) {
    clReleaseCommandQueue(oclCommandQueue);
    oclCommandQueue = NULL;
  }

  if (oclContext) {
    clReleaseContext(oclContext);
    oclContext = NULL;
  }

  // destroy shader programs
  if (gShaderProgramObject) {
    GLsizei shaderCount;
    GLsizei i;

    glUseProgram(gShaderProgramObject);
    glGetProgramiv(gShaderProgramObject, GL_ATTACHED_SHADERS, &shaderCount);

    GLuint *pShaders = (GLuint *)malloc(shaderCount * sizeof(GLuint));
    if (pShaders) {
      glGetAttachedShaders(gShaderProgramObject, shaderCount, &shaderCount,
                           pShaders);

      for (i = 0; i < shaderCount; i++) {
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

  if (wglGetCurrentContext() == ghrc) {
    wglMakeCurrent(NULL, NULL);
  }

  if (ghrc) {
    wglDeleteContext(ghrc);
    ghrc = NULL;
  }

  if (ghdc) {
    ReleaseDC(ghwnd, ghdc);
    ghdc = NULL;
  }

  if (gpFile) {
    fprintf(gpFile, "==== Application Terminated ====\n");
    fclose(gpFile);
    gpFile = NULL;
  }
}

void launchCPUKernel(float time) {
  for (int i = 0; i < meshWidth; i++) {
    for (int j = 0; j < meshHeight; j++) {
      for (int k = 0; k < 4; k++) {
        float u = i / (float)meshWidth;
        float v = j / (float)meshHeight;

        u = (u * 2.0) - 1.0;
        v = (v * 2.0) - 1.0;

        float freq = 4.0f;
        float w = sinf(freq * u + time) * cosf(freq * v + time) * 0.5f;

        if (k == 0)
          pos[i][j][k] = u;
        if (k == 1)
          pos[i][j][k] = w;
        if (k == 2)
          pos[i][j][k] = v;
        if (k == 3)
          pos[i][j][k] = 1.0f;
      }
    }
  }
}

char *loadOclProgramSource(const char *filename, const char *preamble,
                           size_t *sizeFinalLength) {
  // locals
  FILE *pFile = NULL;
  size_t sizeSourceLength;

  pFile = fopen(filename, "rb"); // binary read
  if (pFile == NULL)
    return (NULL);

  size_t sizePreambleLength = (size_t)strlen(preamble);

  // get the length of the source code
  fseek(pFile, 0, SEEK_END);
  sizeSourceLength = ftell(pFile);
  fseek(pFile, 0, SEEK_SET); // reset to beginning

  // allocate a buffer for the source code string and read it in
  char *sourceString =
      (char *)malloc(sizeSourceLength + sizePreambleLength + 1);
  memcpy(sourceString, preamble, sizePreambleLength);
  if (fread((sourceString) + sizePreambleLength, sizeSourceLength, 1, pFile) !=
      1) {
    fclose(pFile);
    free(sourceString);
    return (0);
  }

  // close the file and return the total length of the combined (preamble +
  // source) string
  fclose(pFile);
  if (sizeFinalLength != 0) {
    *sizeFinalLength = sizeSourceLength + sizePreambleLength;
  }
  sourceString[sizeSourceLength + sizePreambleLength] = '\0';

  return (sourceString);
}
