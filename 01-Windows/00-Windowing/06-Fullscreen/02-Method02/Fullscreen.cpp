// headers
#include <windows.h>

// global function declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// global variables
DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
bool gbFullscreen = false;
HWND ghwnd = NULL;

int gWidth  = 800;
int gHeight = 600;


// WinMain()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	// variable declarations
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("MyApp");

	// code
	// initialization of WNDCLASSEX
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	// register above class
	RegisterClassEx(&wndclass);

	// get the screen size
	int width = GetSystemMetrics(SM_CXSCREEN);
	int height = GetSystemMetrics(SM_CYSCREEN);

	// create window
	hwnd = CreateWindow(szAppName,
		TEXT("Fullscreen: Method 02"),
		WS_OVERLAPPEDWINDOW,
		(width / 2) - (gWidth / 2),
		(height / 2) - (gHeight / 2),
		gWidth,
		gHeight,
		NULL,
		NULL,
		hInstance,
		NULL);

	ghwnd = hwnd;

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	// message loop
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return((int)msg.wParam);
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	// local function declaration
	void ToggleFullscreen(void);

	// code
	switch (iMsg)
	{

	case WM_KEYDOWN:
		switch (wParam)
		{
		case 0x46:
		case 0x66:
			ToggleFullscreen();
			break;

		default:
			break;
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	}
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}


void ToggleFullscreen(void)
{
	// local variables
	DEVMODE dm;
	LONG lResult;
	MONITORINFO mi = { sizeof(MONITORINFO) };

	// code
	if (gbFullscreen == false)
	{
		dm.dmSize = sizeof(DEVMODE);
		EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm);

		GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi);

		dm.dmSpecVersion = DM_SPECVERSION;
		dm.dmPelsWidth = gWidth; // mi.rcMonitor.right - mi.rcMonitor.left;
		dm.dmPelsHeight = gHeight; // mi.rcMonitor.bottom - mi.rcMonitor.top;
		dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_SPECVERSION;

		lResult = ChangeDisplaySettings(&dm, CDS_FULLSCREEN);

		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			if (GetWindowPlacement(ghwnd, &wpPrev) &&
				GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd, HWND_TOP,
					0,
					0,
					gWidth,
					gHeight,
					SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
		gbFullscreen = true;
	}
	else
	{
		ChangeDisplaySettings(NULL, 0);
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP,
			0, 0, 0, 0,
			SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER);
		ShowCursor(TRUE);
		gbFullscreen = false;
	}
}

