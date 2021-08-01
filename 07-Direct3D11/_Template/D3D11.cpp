// headers
#include <stdio.h>
#include <windows.h>

#include "D3D11.h"

// macros
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

// global function declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// global variables
bool gbFullscreen = false;
bool gbActiveWindow = false;

HWND ghwnd = NULL;
FILE *gpFile = NULL;

DWORD dwStyle;
WINDOWPLACEMENT wpPrev = {sizeof(WINDOWPLACEMENT)};

// log macro
#define LogD(...)                                                              \
  fopen_s(&gpFile, "RMCLog.txt", "w");                                         \
  fprintf(gpFile, __VA_ARGS__);                                                \
  fclose(gpFile);

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
  LogD("==== Application Started ====\n");

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
  hwnd = CreateWindowEx(WS_EX_APPWINDOW, szAppName, TEXT("D3D11 | Blue Screen"),
                        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN |
                            WS_CLIPSIBLINGS | WS_VISIBLE,
                        (width / 2) - 400, (height / 2) - 300, WIN_WIDTH,
                        WIN_HEIGHT, NULL, NULL, hInstance, NULL);

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
        update();
        display();
      }
    }
  }

  return ((int)msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {
  // function declaration
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

  // warm-up resize call
  resize(WIN_WIDTH, WIN_HEIGHT);
}

void resize(int width, int height) {
  // code
}

void display(void) {
  // code
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

  LogD("==== Application Terminated ====\n");
}
