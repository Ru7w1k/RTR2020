// headers
#include <Windows.h>
#include <stdio.h> // for file IO
#include <d3d11.h>

#include "main.h"

// linker commands
#pragma comment(lib, "d3d11.lib")

// macros
#define WIN_WIDTH  800
#define WIN_HEIGHT 600

// global function declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void Log(const char*);

// global variable declarations
FILE* gpFile = NULL;
char gszLogFile[] = "Log.txt";

HWND ghwnd = NULL;

DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };

bool gbActiveWindow = false;
bool gbEscaptKeyIsPressed = false;
bool gbFullScreen = false;

float gClearColor[4];  // RGBA
IDXGISwapChain* gpIDXGISwapChain = NULL;
ID3D11Device* gpID3D11Device = NULL;
ID3D11DeviceContext* gpID3D11DeviceContext = NULL;
ID3D11RenderTargetView* gpID3D11RenderTargetView = NULL;

// WinMain
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	// function declarations
	HRESULT initialize(void);
	void uninitialize(void);
	void display(void);

	// variables
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szClassName[] = TEXT("Direct3D11");
	bool bDone = false;

	// code
	// create file for logging
	if (fopen_s(&gpFile, gszLogFile, "w") != 0)
	{
		MessageBox(NULL, TEXT("Cannot open log file!"), TEXT("Error"), MB_OK | MB_ICONSTOP | MB_TOPMOST);
		exit(0);
	}
	else
	{
		fprintf(gpFile, "Log.txt file created..\n");
		fclose(gpFile);
	}


	// initialization of WNDCLASSEX
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(RMC_ICON));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.lpszClassName = szClassName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(RMC_ICON));

	// register class
	RegisterClassEx(&wndclass);

	// create window
	hwnd = CreateWindow(szClassName,
		TEXT("DirectX | Blue Screen"),
		WS_OVERLAPPEDWINDOW,
		100,
		100,
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ghwnd = hwnd;

	ShowWindow(hwnd, iCmdShow);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	// initialize D3D
	HRESULT hr;
	hr = initialize();
	if (FAILED(hr))
	{
		Log("Initialize() failed! Exiting now...\n");
		DestroyWindow(hwnd);
		hwnd = NULL;
	}
	else
	{
		Log("Initialize() successful!\n");
	}

	// game loop
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
			// render
			display();

			if (gbActiveWindow == true && gbEscaptKeyIsPressed == true)
			{
				bDone = true;
			}
		}
	}

	// clean-up
	uninitialize();

	return((int)msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	// function declarations
	HRESULT resize(int, int);
	void ToggleFullScreen(void);
	void uninitialize();

	// variable declarations
	HRESULT hr;

	// code
	switch (iMsg)
	{
	case WM_ACTIVATE:
		if (HIWORD(wParam) == 0) // if 0, window is active
			gbActiveWindow = true;
		else                     // otherwise, window is not active
			gbActiveWindow = false;
		break;

	// returned from here, to block DefWindowProc
	// we have our own painter
	case WM_ERASEBKGND:
		return(0);

	case WM_SIZE:
		if (gpID3D11DeviceContext)
		{
			hr = resize(LOWORD(lParam), HIWORD(lParam));
			if (FAILED(hr))
			{
				Log("resize() failed!\n");
				return(hr);
			}
			else
			{
				Log("resize() succeeded!\n");
			}
		}
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			if (gbEscaptKeyIsPressed == false)
				gbEscaptKeyIsPressed = true;
			break;
		
		case 0x46:
			ToggleFullScreen();
			break;

		default:
			break;
		}
		break;

	case WM_CLOSE:
		uninitialize();
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		break;
	}

	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}

void ToggleFullScreen(void)
{
	// local variables
	MONITORINFO mi = { sizeof(MONITORINFO) };

	// code
	if (gbFullScreen == false)
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
		gbFullScreen = true;
	}
	else
	{
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP,
			0, 0, 0, 0,
			SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER);
		ShowCursor(TRUE);
		gbFullScreen = false;
	}
}

HRESULT initialize(void)
{
	// function declarations
	void uninitialize(void);
	HRESULT resize(int, int);

	// variable declarations
	HRESULT hr;
	D3D_DRIVER_TYPE d3dDriverType;
	D3D_DRIVER_TYPE d3dDriverTypes[] = { D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP, D3D_DRIVER_TYPE_REFERENCE };
	D3D_FEATURE_LEVEL d3dFeatureLevel_required = D3D_FEATURE_LEVEL_11_0;
	D3D_FEATURE_LEVEL d3dFeatureLevel_acquired = D3D_FEATURE_LEVEL_10_0; // default, lowest level

	UINT createDeviceFlags = 0;
	UINT numDriverTypes = 0;
	UINT numFeatureLevel = 1; // based upon d3dFeatureLevel_required

	// code
	// calculate the size of array
	numDriverTypes = sizeof(d3dDriverTypes) / sizeof(d3dDriverTypes[0]);

	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	ZeroMemory((void*)&dxgiSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	dxgiSwapChainDesc.BufferCount = 1;
	dxgiSwapChainDesc.BufferDesc.Width= WIN_WIDTH;
	dxgiSwapChainDesc.BufferDesc.Height= WIN_HEIGHT;
	dxgiSwapChainDesc.BufferDesc.Format= DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.OutputWindow = ghwnd;
	dxgiSwapChainDesc.SampleDesc.Count = 1;
	dxgiSwapChainDesc.SampleDesc.Quality = 0;
	dxgiSwapChainDesc.Windowed = TRUE;

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		d3dDriverType = d3dDriverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(
			NULL,                             // Adapter
			d3dDriverType,                    // Driver Type
			NULL,                             // Software Renderer
			createDeviceFlags,                // Flags
			&d3dFeatureLevel_required,        // Feature Level
			numFeatureLevel,                  // Num Feature Levels
			D3D11_SDK_VERSION,                // SDK Version
			&dxgiSwapChainDesc,               // Swap Chain Desc
			&gpIDXGISwapChain,                // Swap Chain!
			&gpID3D11Device,                  // Device
			&d3dFeatureLevel_acquired,        // Feature Level aquired
			&gpID3D11DeviceContext            // Device Context
		);

		if (SUCCEEDED(hr))
			break;
	}

	if (FAILED(hr))
	{
		Log("D3D11CreateDeviceAndSwapChain() failed..\n");
		return(hr);
	}
	else
	{
		Log("D3D11CreateDeviceAndSwapChain() succeeded..\n");
		Log("The chosen driver is of: ");
		if (d3dDriverType == D3D_DRIVER_TYPE_HARDWARE)
			Log("Hardware Type. \n");
		else if (d3dDriverType == D3D_DRIVER_TYPE_WARP)
			Log("Warp Type. \n");
		else if (d3dDriverType == D3D_DRIVER_TYPE_REFERENCE)
			Log("Reference Type. \n");
		else
			Log("Unknown Type. \n");

		Log("The supported highest feature level is: ");
		if (d3dFeatureLevel_acquired == D3D_FEATURE_LEVEL_11_0)
			Log("11.0 \n");
		else if (d3dFeatureLevel_acquired == D3D_FEATURE_LEVEL_10_1)
			Log("10.1 \n");
		else if (d3dFeatureLevel_acquired == D3D_FEATURE_LEVEL_10_0)
			Log("10.0 \n");
		else
			Log("Unknown \n");
	}

	// d3d clear color
	gClearColor[0] = 0.0f;
	gClearColor[1] = 0.0f;
	gClearColor[2] = 1.0f;
	gClearColor[3] = 1.0f;

	// call resize for the first time
	hr = resize(WIN_WIDTH, WIN_HEIGHT);
	if (FAILED(hr))
	{
		Log("resize() failed..\n");
		return(hr);
	}
	else
	{
		Log("resize() succeeded..\n");
	}

	return(S_OK);
}

HRESULT resize(int width, int height)
{
	// code
	HRESULT hr = S_OK;

	// free any size-dependent resource
	if (gpID3D11RenderTargetView)
	{
		gpID3D11RenderTargetView->Release();
		gpID3D11RenderTargetView = NULL;
	}

	// resize swap chain buffers accordingly
	gpIDXGISwapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

	// again get back buffers from swap chain
	ID3D11Texture2D* pID3D11Texture2D_BackBuffer;
	gpIDXGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pID3D11Texture2D_BackBuffer);

	// again get render target view from d3d11 device using above back buffer
	hr = gpID3D11Device->CreateRenderTargetView(pID3D11Texture2D_BackBuffer, NULL, &gpID3D11RenderTargetView);
	if (FAILED(hr))
	{
		Log("ID3D11Device::CreateRenderTargerView() failed..\n");
		return(hr);
	}
	else
	{
		Log("ID3D11Device::CreateRenderTargerView() succeeded..\n");
	}

	pID3D11Texture2D_BackBuffer->Release();
	pID3D11Texture2D_BackBuffer = NULL;

	// set render target view as render target
	gpID3D11DeviceContext->OMSetRenderTargets(1, &gpID3D11RenderTargetView, NULL);

	// set viewport
	D3D11_VIEWPORT d3dViewport;
	d3dViewport.TopLeftX = 0.0f;
	d3dViewport.TopLeftY = 0.0f;
	d3dViewport.Width  = (FLOAT)width;
	d3dViewport.Height = (FLOAT)height;
	d3dViewport.MinDepth = 0.0f;
	d3dViewport.MaxDepth = 1.0f;

	gpID3D11DeviceContext->RSSetViewports(1, &d3dViewport);

	return(hr);
}

void display(void)
{
	// code
	// clear render target view to a chosen color
	gpID3D11DeviceContext->ClearRenderTargetView(gpID3D11RenderTargetView, gClearColor);

	// switch between front and back buffer
	gpIDXGISwapChain->Present(0, 0);
}

void uninitialize(void)
{
	// code
	if (gpID3D11RenderTargetView)
	{
		gpID3D11RenderTargetView->Release();
		gpID3D11RenderTargetView = NULL;
	}

	if (gpIDXGISwapChain)
	{
		gpIDXGISwapChain->Release();
		gpIDXGISwapChain = NULL;
	}


	if (gpID3D11DeviceContext)
	{
		gpID3D11DeviceContext->Release();
		gpID3D11DeviceContext = NULL;
	}


	if (gpID3D11Device)
	{
		gpID3D11Device->Release();
		gpID3D11Device = NULL;
	}

	if (gpFile)
	{
		Log("Uninitialize() succeeded..\n");
		Log("Log file closed..\n");
	}
}


void Log(const char* str)
{
	fopen_s(&gpFile, gszLogFile, "a+");
	fprintf_s(gpFile, str);
	fclose(gpFile);
}
