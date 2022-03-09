/*
 * Direct3D - Geometry Shader
 */

#include <Windows.h>
#include <stdio.h>  // for file I/O

#include <d3d11.h>
#include <d3dcompiler.h> // for shader compilation

#pragma warning(disable:4838)
#include "XNAMath\xnamath.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

#define WIN_WIDTH  800
#define WIN_HEIGHT 600

// global function declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// global variable declarations
FILE *gpFile = NULL;
char gszLogFileName[] = "Log.txt";

HWND ghwnd = NULL;

DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };

bool gbActiveWindow = false;
bool gbEscapeKeyIsPressed = false;
bool gbFullscreen = false;

float gClearColor[4]; // RGBA
IDXGISwapChain *gpIDXGISwapChain = NULL;
ID3D11Device *gpID3D11Device = NULL;
ID3D11DeviceContext *gpID3D11DeviceContext = NULL;
ID3D11RenderTargetView *gpID3D11RenderTargetView = NULL;

ID3D11VertexShader *gpID3D11VertexShader = NULL;
ID3D11GeometryShader *gpID3D11GeometryShader = NULL;
ID3D11PixelShader *gpID3D11PixelShader = NULL;
ID3D11Buffer *gpID3D11Buffer_VertexBuffer = NULL;
ID3D11InputLayout *gpID3D11InputLayout = NULL;
ID3D11Buffer *gpID3D11Buffer_ConstantBuffer = NULL;

struct CBUFFER 
{
	XMMATRIX WorldViewProjectionMatrix;
};

XMMATRIX gPerspectiveProjectionMatrix;

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
	if (fopen_s(&gpFile, gszLogFileName, "w") != 0)
	{
		MessageBox(NULL, TEXT("Cannot Create log file!"), TEXT("Error"), MB_OK | MB_ICONSTOP | MB_TOPMOST);
		exit(0);
	}
	else
	{
		fprintf(gpFile, "Log.txt file created...\n");
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
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.lpszClassName = szClassName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	// register class
	RegisterClassEx(&wndclass);

	// create window
	hwnd = CreateWindow(szClassName,
		TEXT("D3D11 | Geometry Shader"),
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
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "initialize() Failed. Exiting Now...\n");
		fclose(gpFile);
		DestroyWindow(hwnd);
		hwnd = NULL;
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "initialize() succeeded.\n");
		fclose(gpFile);
	}

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
			// render
			display();

			if (gbActiveWindow == true) 
			{
				if (gbEscapeKeyIsPressed == true)
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
		else // if non-zero, window is not active
			gbActiveWindow = false;
		break;

	// returned from here, to block DefWindowProc
	// We have our own painter
	case WM_ERASEBKGND:
			return(0);

	case WM_SIZE:
		if (gpID3D11DeviceContext)
		{
			hr = resize(LOWORD(lParam), HIWORD(lParam));
			if (FAILED(hr))
			{
				fopen_s(&gpFile, gszLogFileName, "a+");
				fprintf_s(gpFile, "resize() Failed.\n");
				fclose(gpFile);
				return(hr);
			}
			else
			{
				fopen_s(&gpFile, gszLogFileName, "a+");
				fprintf_s(gpFile, "resize() Succeeded.\n");
				fclose(gpFile);
			}
		}
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			if (gbEscapeKeyIsPressed == false)
				gbEscapeKeyIsPressed = true;
			break;

		case 0x46:
			if (gbFullscreen == false)
			{
				ToggleFullScreen();
				gbFullscreen = true;
			}
			else
			{
				ToggleFullScreen();
				gbFullscreen = false;
			}
			break;

		default:
			break;
		}
		break;

	case WM_LBUTTONDOWN:
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

void ToggleFullScreen()
{
	MONITORINFO MI;

	if (gbFullscreen == false)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			MI = { sizeof(MONITORINFO) };
			if (GetWindowPlacement(ghwnd, &wpPrev)
				&& GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &MI))
			{
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd,
					HWND_TOP,
					MI.rcMonitor.left,
					MI.rcMonitor.top,
					MI.rcMonitor.right - MI.rcMonitor.left,
					MI.rcMonitor.bottom - MI.rcMonitor.top,
					SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);		
	}
	else
	{
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
	D3D_FEATURE_LEVEL d3dFeatureLevel_acquired = D3D_FEATURE_LEVEL_10_0; // default, lowest

	UINT createDeviceFlags = 0;
	UINT numDriverTypes = 0;
	UINT numFeatureLevels = 1; // based upon d3dFeatureLevel_required

	// code
	numDriverTypes = sizeof(d3dDriverTypes) / sizeof(d3dDriverTypes[0]); // calculating size of array

	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	ZeroMemory((void *)&dxgiSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	dxgiSwapChainDesc.BufferCount = 1;
	dxgiSwapChainDesc.BufferDesc.Width = WIN_WIDTH;
	dxgiSwapChainDesc.BufferDesc.Height = WIN_HEIGHT;
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
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
			NULL,							// Adapter
			d3dDriverType,					// Driver Type
			NULL,							// Software Renderer
			createDeviceFlags,				// Flags
			&d3dFeatureLevel_required,		// Feature Level
			numFeatureLevels,				// Num Feature Levels
			D3D11_SDK_VERSION,				// SDK version
			&dxgiSwapChainDesc,				// Swap Chain Desc
			&gpIDXGISwapChain,				// Swap Chain
			&gpID3D11Device,				// Device
			&d3dFeatureLevel_acquired,		// Feature Level
			&gpID3D11DeviceContext			// Device Context
		);

		if (SUCCEEDED(hr))
			break;
	}
	if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "D3D11CreateDeviceAndSwapChain() failed..\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "D3D11CreateDeviceAndSwapChain() succeeded..\n");
		fprintf_s(gpFile, "The chosen driver is of: ");
		if (d3dDriverType == D3D_DRIVER_TYPE_HARDWARE)
		{
			fprintf_s(gpFile, "Hardware Type. \n");
		}
		else if (d3dDriverType == D3D_DRIVER_TYPE_WARP)
		{
			fprintf_s(gpFile, "Warp Type. \n");
		}
		else if (d3dDriverType == D3D_DRIVER_TYPE_REFERENCE)
		{
			fprintf_s(gpFile, "Reference Type. \n");
		}
		else
		{
			fprintf_s(gpFile, "Unknown Type. \n");
		}

		fprintf_s(gpFile, "The supported Highest Feature Level is: ");
		if (d3dFeatureLevel_acquired == D3D_FEATURE_LEVEL_11_0)
		{
			fprintf_s(gpFile, "11.0\n");
		}
		else if (d3dFeatureLevel_acquired == D3D_FEATURE_LEVEL_10_1)
		{
			fprintf_s(gpFile, "10.1\n");
		}
		else if (d3dFeatureLevel_acquired == D3D_FEATURE_LEVEL_10_0)
		{
			fprintf_s(gpFile, "10.0\n");
		}
		else
		{
			fprintf_s(gpFile, "Unknown.\n");
		}
		fclose(gpFile);
	}

	// initialize shaders, input layouts, constant buffers etc.

	//// VERTEX SHADER ///////////////////////////////////////////////////////////////////////
	const char *vertexShaderSourceCode =
		"cbuffer ConstantBuffer" \
		"{" \
		"	float4x4 worldViewProjectionMatrix;" \
		"}" \

		"struct vertex_output" \
		"{" \
		"	float4 position: POSITION;" \
		"};" \

		"vertex_output main(float4 pos: POSITION)" \
		"{" \
		"	vertex_output output;" \
		"	output.position = mul(worldViewProjectionMatrix, pos);" \
		"	return(output);" \
		"}";

	ID3DBlob *pID3DBlob_VertexShaderCode = NULL;
	ID3DBlob *pID3DBlob_Error = NULL;

	hr = D3DCompile(vertexShaderSourceCode,			// source code
		lstrlenA(vertexShaderSourceCode) + 1,		// source code length
		"VS",										// vertex shader
		NULL,										// #defines 
		D3D_COMPILE_STANDARD_FILE_INCLUDE,			// include standard files
		"main",										// entry point function's name
		"vs_5_0",									// vertex shader model 5.0
		0,											// compile shader for: debug/optimization/validation etc
		0,											// count of shader effects
		&pID3DBlob_VertexShaderCode,				// returns compiled shader code
		&pID3DBlob_Error);							// returns compilation errors

	if (FAILED(hr))
	{
		if (pID3DBlob_Error != NULL)
		{
			fopen_s(&gpFile, gszLogFileName, "a+");
			fprintf_s(gpFile, "D3DCompile() failed for Vertex Shader: %s.\n",
				(char *)pID3DBlob_Error->GetBufferPointer());
			fclose(gpFile);
			pID3DBlob_Error->Release();
			pID3DBlob_Error = NULL;
			return(hr);
		}
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "D3DCompile() succeeded for Vertex Shader.\n");
		fclose(gpFile);
	}

	hr = gpID3D11Device->CreateVertexShader(pID3DBlob_VertexShaderCode->GetBufferPointer(),
		pID3DBlob_VertexShaderCode->GetBufferSize(), NULL, &gpID3D11VertexShader);
	if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateVertexShader() failed.\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateVertexShader() succeeded.\n");
		fclose(gpFile);
	}

	// set vertex shader in pipeline
	gpID3D11DeviceContext->VSSetShader(gpID3D11VertexShader, NULL, NULL);

	//////////////////////////////////////////////////////////////////////////////////////////

	//// GEOMETRY SHADER ///////////////////////////////////////////////////////////////////////
	const char *geometryShaderSourceCode =
		"cbuffer ConstantBuffer" \
		"{" \
		"	float4x4 worldViewProjectionMatrix;" \
		"}" \

		"struct vertex_output" \
		"{" \
		"	float4 position: POSITION;" \
		"};" \

		"struct geometry_output" \
		"{" \
		"	float4 position: SV_POSITION;" \
		"};" \

		"[maxvertexcount(9)]" \
		"void main(triangle vertex_output input[3], inout TriangleStream<geometry_output> triangleStream)" \
		"{" \
		"	geometry_output output;" \
		
		"	for (int vertex = 0; vertex < 3; vertex++)" \
		"	{" \
		"		output.position = mul(worldViewProjectionMatrix, (input[vertex].position + float4(0.0, 1.0, 0.0, 0.0)));" \
		"		triangleStream.Append(output);" \

		"		output.position = mul(worldViewProjectionMatrix, (input[vertex].position + float4(1.0, -1.0, 0.0, 0.0)));" \
		"		triangleStream.Append(output);" \

		"		output.position = mul(worldViewProjectionMatrix, (input[vertex].position + float4(-1.0, -1.0, 0.0, 0.0)));" \
		"		triangleStream.Append(output);" \
		
		"		triangleStream.RestartStrip();" \
		"	}" \
		"}";

	ID3DBlob *pID3DBlob_GeometryShaderCode = NULL;
	pID3DBlob_Error = NULL;

	hr = D3DCompile(geometryShaderSourceCode,		// source code
		lstrlenA(geometryShaderSourceCode) + 1,		// source code length
		"GS",										// vertex shader
		NULL,										// #defines 
		D3D_COMPILE_STANDARD_FILE_INCLUDE,			// include standard files
		"main",										// entry point function's name
		"gs_5_0",									// vertex shader model 5.0
		0,											// compile shader for: debug/optimization/validation etc
		0,											// count of shader effects
		&pID3DBlob_GeometryShaderCode,				// returns compiled shader code
		&pID3DBlob_Error);							// returns compilation errors

	if (FAILED(hr))
	{
		if (pID3DBlob_Error != NULL)
		{
			fopen_s(&gpFile, gszLogFileName, "a+");
			fprintf_s(gpFile, "D3DCompile() failed for Geometry Shader: %s.\n",
				(char *)pID3DBlob_Error->GetBufferPointer());
			fclose(gpFile);
			pID3DBlob_Error->Release();
			pID3DBlob_Error = NULL;
			return(hr);
		}
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "D3DCompile() succeeded for Geometry Shader.\n");
		fclose(gpFile);
	}

	hr = gpID3D11Device->CreateGeometryShader(pID3DBlob_GeometryShaderCode->GetBufferPointer(),
		pID3DBlob_GeometryShaderCode->GetBufferSize(), NULL, &gpID3D11GeometryShader);
	if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateGeometryShader() failed.\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateGeometryShader() succeeded.\n");
		fclose(gpFile);
	}

	// set vertex shader in pipeline
	gpID3D11DeviceContext->GSSetShader(gpID3D11GeometryShader, NULL, NULL);

	//////////////////////////////////////////////////////////////////////////////////////////

	//// PIXEL SHADER ///////////////////////////////////////////////////////////////////////
	const char *pixelShaderSourceCode =
		"float4 main(void) : SV_TARGET" \
		"{" \
		"	return(float4(1.0f, 1.0f, 1.0f, 1.0f));" \
		"}";

	ID3DBlob *pID3DBlob_PixelShaderCode = NULL;

	hr = D3DCompile(pixelShaderSourceCode,			// source code
		lstrlenA(pixelShaderSourceCode) + 1,		// source code length
		"PS",										// pixel shader
		NULL,										// #defines 
		D3D_COMPILE_STANDARD_FILE_INCLUDE,			// include standard files
		"main",										// entry point function's name
		"ps_5_0",									// pixel shader model 5.0
		0,											// compile shader for: debug/optimization/validation etc
		0,											// count of shader effects
		&pID3DBlob_PixelShaderCode,					// returns compiled shader code
		&pID3DBlob_Error);							// returns compilation errors

	if (FAILED(hr))
	{
		if (pID3DBlob_Error != NULL)
		{
			fopen_s(&gpFile, gszLogFileName, "a+");
			fprintf_s(gpFile, "D3DCompile() failed for Pixel Shader: %s.\n",
				(char *)pID3DBlob_Error->GetBufferPointer());
			fclose(gpFile);
			pID3DBlob_Error->Release();
			pID3DBlob_Error = NULL;
			return(hr);
		}
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "D3DCompile() succeeded for Pixel Shader.\n");
		fclose(gpFile);
	}

	hr = gpID3D11Device->CreatePixelShader(pID3DBlob_PixelShaderCode->GetBufferPointer(),
		pID3DBlob_PixelShaderCode->GetBufferSize(), NULL, &gpID3D11PixelShader);
	if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreatePixelShader() failed.\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreatePixelShader() succeeded.\n");
		fclose(gpFile);
	}

	// set pixel shader in pipeline
	gpID3D11DeviceContext->PSSetShader(gpID3D11PixelShader, NULL, NULL);

	////////////////////////////////////////////////////////////////////////////////////

	// create and set input layout
	D3D11_INPUT_ELEMENT_DESC inputElementsDesc;
	ZeroMemory((void *)&inputElementsDesc, sizeof(inputElementsDesc));
	inputElementsDesc.SemanticName = "POSITION";
	inputElementsDesc.SemanticIndex = 0;
	inputElementsDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementsDesc.InputSlot = 0;
	inputElementsDesc.AlignedByteOffset = 0;
	inputElementsDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputElementsDesc.InstanceDataStepRate = 0;

	hr = gpID3D11Device->CreateInputLayout(&inputElementsDesc, 1,
		pID3DBlob_VertexShaderCode->GetBufferPointer(), pID3DBlob_VertexShaderCode->GetBufferSize(),
		&gpID3D11InputLayout);
	if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateInputLayout() failed.\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateInputLayout() succeeded.\n");
		fclose(gpFile);
	}

	gpID3D11DeviceContext->IASetInputLayout(gpID3D11InputLayout);

	gpID3D11VertexShader->Release();
	gpID3D11VertexShader = NULL;

	gpID3D11PixelShader->Release();
	gpID3D11PixelShader = NULL;

	/////////////////////////////////////////////////////////////////////////////////////

	//// VERTEX DATA ////////////////////////////////////////////////////////////////////

	// Clockwise | Left Hand Rule
	float vertices[] =
	{
		 0.0f,  1.0f, 0.0f,	// apex
		 1.0f, -1.0f, 0.0f,	// right
		-1.0f, -1.0f, 0.0f	// left
	};

	// create vertex buffer
	D3D11_BUFFER_DESC bufferDesc_VertexBuffer;
	ZeroMemory(&bufferDesc_VertexBuffer, sizeof(D3D11_BUFFER_DESC));
	bufferDesc_VertexBuffer.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc_VertexBuffer.ByteWidth = sizeof(float) * ARRAYSIZE(vertices);
	bufferDesc_VertexBuffer.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc_VertexBuffer.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;

	hr = gpID3D11Device->CreateBuffer(&bufferDesc_VertexBuffer, NULL, &gpID3D11Buffer_VertexBuffer);
	if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateBuffer() failed for Vertex Buffer.\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateBuffer() succeeded for Vertex Buffer.\n");
		fclose(gpFile);
	}

	// copy vertices into above buffer
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	ZeroMemory(&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext->Map(gpID3D11Buffer_VertexBuffer, 0,
		D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	memcpy(mappedSubresource.pData, vertices, sizeof(vertices));
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_VertexBuffer, NULL);

	/////////////////////////////////////////////////////////////////////////////////////

	//// CONSTANT DATA ////////////////////////////////////////////////////////////////////

	// define and set constant buffer
	D3D11_BUFFER_DESC bufferDesc_ConstantBuffer;
	ZeroMemory(&bufferDesc_ConstantBuffer, sizeof(D3D11_BUFFER_DESC));
	bufferDesc_ConstantBuffer.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc_ConstantBuffer.ByteWidth = sizeof(CBUFFER);
	bufferDesc_ConstantBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	hr = gpID3D11Device->CreateBuffer(&bufferDesc_ConstantBuffer, NULL, &gpID3D11Buffer_ConstantBuffer);
	if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateBuffer() failed for Constant Buffer.\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateBuffer() succeeded for Constant Buffer.\n");
		fclose(gpFile);
	}

	gpID3D11DeviceContext->VSSetConstantBuffers(0, 1, &gpID3D11Buffer_ConstantBuffer);
	gpID3D11DeviceContext->GSSetConstantBuffers(0, 1, &gpID3D11Buffer_ConstantBuffer);

	/////////////////////////////////////////////////////////////////////////////////////

	// d3d clear color (blue)
	gClearColor[0] = 0.0f;
	gClearColor[1] = 0.0f;
	gClearColor[2] = 0.0f;
	gClearColor[3] = 1.0f;

	// set projection matrix
	gPerspectiveProjectionMatrix = XMMatrixIdentity();

	// call resize for first time
	hr = resize(WIN_WIDTH, WIN_HEIGHT);
	if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "resize() failed.\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "resize() succeeded.\n");
		fclose(gpFile);
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
	ID3D11Texture2D *pID3D11Texture2D_BackBuffer;
	gpIDXGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pID3D11Texture2D_BackBuffer);

	// afain get render target view from d3d11 device using above back buffer
	hr = gpID3D11Device->CreateRenderTargetView(pID3D11Texture2D_BackBuffer, NULL, &gpID3D11RenderTargetView);
	if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateRenderTargetView() failed. \n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateRenderTargetView() succeeded. \n");
		fclose(gpFile);
	}

	pID3D11Texture2D_BackBuffer->Release();
	pID3D11Texture2D_BackBuffer = NULL;

	// set render target view as render target
	gpID3D11DeviceContext->OMSetRenderTargets(1, &gpID3D11RenderTargetView, NULL);

	// set viewport
	D3D11_VIEWPORT d3dViewPort;
	d3dViewPort.TopLeftX = 0.0f;
	d3dViewPort.TopLeftY = 0.0f;
	d3dViewPort.Width  = (FLOAT)width;
	d3dViewPort.Height = (FLOAT)height;
	d3dViewPort.MinDepth = 0.0f;
	d3dViewPort.MaxDepth = 1.0f;
	gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort);

	// set perspective matrix
	gPerspectiveProjectionMatrix = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(45.0f),
		(float)width / (float)height,
		0.1f,
		100.0f
	);

	return(hr);
}

void display(void)
{
	// code
	// clear render targer view to a chosen color
	gpID3D11DeviceContext->ClearRenderTargetView(gpID3D11RenderTargetView, gClearColor);

	// select which vertex buffer to display
	UINT stride = sizeof(float) * 3;
	UINT offset = 0;
	gpID3D11DeviceContext->IASetVertexBuffers(0, 1, &gpID3D11Buffer_VertexBuffer,
		&stride, &offset);

	// select geometry primitive
	gpID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// translation is concerned with world matrix transformation
	XMMATRIX worldMatrix = XMMatrixIdentity();
	XMMATRIX viewMatrix = XMMatrixIdentity();

	// translations
	worldMatrix = XMMatrixTranslation(0.0f, 0.0f, 3.0f);

	// final WorldViewProjection matrix
	XMMATRIX wvpMatrix = worldMatrix * viewMatrix * gPerspectiveProjectionMatrix;

	// load the data into the constant buffer
	CBUFFER constantBuffer;
	ZeroMemory(&constantBuffer, sizeof(CBUFFER));
	constantBuffer.WorldViewProjectionMatrix = wvpMatrix;
	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer, 0,
		NULL, &constantBuffer, 0, 0);

	// draw vertex buffer to render target
	gpID3D11DeviceContext->Draw(3, 0);

	// switch between front and back buffers
	gpIDXGISwapChain->Present(0, 0);
}
			
void uninitialize(void)
{
	// code
	if (gpID3D11Buffer_ConstantBuffer)
	{
		gpID3D11Buffer_ConstantBuffer->Release();
		gpID3D11Buffer_ConstantBuffer = NULL;
	}

	if (gpID3D11InputLayout)
	{
		gpID3D11InputLayout->Release();
		gpID3D11InputLayout = NULL;
	}

	if (gpID3D11Buffer_VertexBuffer)
	{
		gpID3D11Buffer_VertexBuffer->Release();
		gpID3D11Buffer_VertexBuffer = NULL;
	}

	if (gpID3D11PixelShader)
	{
		gpID3D11PixelShader->Release();
		gpID3D11PixelShader = NULL;
	}

	if (gpID3D11GeometryShader)
	{
		gpID3D11GeometryShader->Release();
		gpID3D11GeometryShader = NULL;
	}

	if (gpID3D11VertexShader)
	{			
		gpID3D11VertexShader->Release();
		gpID3D11VertexShader = NULL;
	}

	if (gpID3D11RenderTargetView)
	{
		gpID3D11RenderTargetView->Release();
		gpID3D11RenderTargetView = NULL;
	}

	if (gpIDXGISwapChain)
	{
		gpIDXGISwapChain->Release();
		gpIDXGISwapChain= NULL;
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
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "uninitalize() succeeded. \n");
		fprintf_s(gpFile, "Log file closed..\n");
		fclose(gpFile);
	}
}
