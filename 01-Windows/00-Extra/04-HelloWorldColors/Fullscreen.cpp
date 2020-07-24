// headers
#include <windows.h>

// global function declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

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

	// create window
	hwnd = CreateWindow(szAppName,
		TEXT("Hello World - Rutwik Choughule"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		NULL);

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
	// variable declarations
	HDC hDC;
	PAINTSTRUCT ps;
	RECT rc;
	TCHAR str[] = TEXT("Hello World !!!");

	static int R, G, B;

	// code
	switch (iMsg)
	{
	case WM_CREATE:
		R = 255;
		G = 255;
		B = 255;
		break;

	case WM_CHAR:
		switch (wParam)
		{
		case 'R':
		case 'r':
			R = 255;
			G = 0;
			B = 0;
			break;

		case 'G':
		case 'g':
			R = 0;
			G = 255;
			B = 0;
			break;

		case 'B':
		case 'b':
			R = 0;
			G = 0;
			B = 255;
			break;

		case 'C':
		case 'c':
			R = 0;
			G = 255;
			B = 255;
			break;

		case 'M':
		case 'm':
			R = 255;
			G = 0;
			B = 255;
			break;

		case 'Y':
		case 'y':
			R = 255;
			G = 255;
			B = 0;
			break;

		case 'W':
		case 'w':
			R = 255;
			G = 255;
			B = 255;
			break;

		case 'K':
		case 'k':
			R = 0;
			G = 0;
			B = 0;
			break;

		}
		InvalidateRect(hwnd, NULL, TRUE);

		break;


	case WM_PAINT:
		GetClientRect(hwnd, &rc);
		hDC = BeginPaint(hwnd, &ps);
		FillRect(hDC, &rc, CreateSolidBrush(RGB(0, 255, 0)));
		SetBkColor(hDC, RGB(0, 0, 0));
		SetBkMode(hDC, TRANSPARENT);
		SetTextColor(hDC, RGB(R, G, B));
		DrawText(hDC, str, -1, &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		EndPaint(hwnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}

