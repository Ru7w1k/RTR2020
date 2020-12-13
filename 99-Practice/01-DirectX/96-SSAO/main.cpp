// headers
#include <Windows.h>
#include <stdio.h> // for file IO

#include <d3d11.h>        
#include <d3dcompiler.h>  // for shader compilation

#pragma warning(disable:4838)
#include "XNAMath\xnamath.h"

#include "main.h"

// linker commands
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

// macros
#define WIN_WIDTH  800
#define WIN_HEIGHT 600

// global function declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void Log(const char*);

// global variable declarations
FILE *gpFile = NULL;
char gszLogFile[] = "Log.txt";

HWND ghwnd = NULL;

DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };

bool gbActiveWindow = false;
bool gbEscaptKeyIsPressed = false;
bool gbFullScreen = false;

float gClearColor[4];  // RGBA
IDXGISwapChain *gpIDXGISwapChain = NULL;
ID3D11Device *gpID3D11Device = NULL;
ID3D11DeviceContext *gpID3D11DeviceContext = NULL;
ID3D11RenderTargetView *gpID3D11RenderTargetView = NULL;

ID3D11VertexShader *gpID3D11VertexShader = NULL;
ID3D11PixelShader *gpID3D11PixelShader = NULL;

ID3D11Buffer *gpID3D11Buffer_VertexBuffer_Plane_Position = NULL;
ID3D11Buffer *gpID3D11Buffer_VertexBuffer_Plane_Normal = NULL;
ID3D11Buffer *gpID3D11Buffer_VertexBuffer_Cube_Position = NULL;
ID3D11Buffer *gpID3D11Buffer_VertexBuffer_Cube_Normal = NULL;

ID3D11InputLayout *gpID3D11InputLayout = NULL;
ID3D11Buffer *gpID3D11Buffer_ConstantBuffer = NULL;

ID3D11RasterizerState *gpID3D11RasterizerState = NULL;
ID3D11DepthStencilView *gpID3D11DepthStencilView = NULL;

float gAngleCube = 0.0f;

// uniforms
struct CBUFFER
{
	XMMATRIX WorldMatrix;
	XMMATRIX ViewMatrix;
	XMMATRIX ProjectionMatrix;

	XMVECTOR La;
	XMVECTOR Ld;
	XMVECTOR Ls;
	XMVECTOR LightPosition;

	XMVECTOR Ka;
	XMVECTOR Kd;
	XMVECTOR Ks;
	float Material_Shininess;

	unsigned int KeyPressed;

};

float lightAmbient[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
float lightDiffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
float lightSpecular[4] = { 1.0f, 0.0f, 1.0f, 1.0f };
float lightPosition[4] = { 100.0f, 100.0f, -100.0f, 1.0f };

float materialAmbient[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
float materialDiffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
float materialSpecular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
float materialShininess = 128.0f;

XMMATRIX gPerspectiveProjectionMatrix;

// WinMain
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	// function declarations
	HRESULT initialize(void);
	void uninitialize(void);
	void display(void);
	void update(void);

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

	// get the screen size
	int width = GetSystemMetrics(SM_CXSCREEN);
	int height = GetSystemMetrics(SM_CYSCREEN);

	// create window
	hwnd = CreateWindow(szClassName,
		TEXT("DirectX | Screen Space Ambient Occlusion"),
		WS_OVERLAPPEDWINDOW,
		(width / 2) - (WIN_WIDTH/2),
		(height / 2) - (WIN_HEIGHT/2),
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
			update();
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
			NULL,                         // Adapter
			d3dDriverType,                // Driver Type
			NULL,                         // Software Renderer
			createDeviceFlags,            // Flags
			&d3dFeatureLevel_required,    // Feature Level
			numFeatureLevel,              // Num Feature Levels
			D3D11_SDK_VERSION,            // SDK Version
			&dxgiSwapChainDesc,           // Swap Chain Desc
			&gpIDXGISwapChain,            // Swap Chain!
			&gpID3D11Device,              // Device
			&d3dFeatureLevel_acquired,    // Feature Level aquired
			&gpID3D11DeviceContext        // Device Context
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

	// initialize shaders, input layouts, constant buffers etc..

	//// vertex shader ////////////////////////////////////////////////////////////
	const char *vertexShaderSourceCode =
		"cbuffer ConstantBuffer                                                               \n" \
		"{                                                                                    \n" \
		"	float4x4 worldMatrix;                                                             \n" \
		"	float4x4 viewMatrix;                                                              \n" \
		"	float4x4 projectionMatrix;                                                        \n" \
		"                                                                                     \n" \
		"	float4   la;                                                                      \n" \
		"	float4   ld;                                                                      \n" \
		"	float4   ls;                                                                      \n" \
		"	float4   lightPosition;                                                           \n" \
		"                                                                                     \n" \
		"	float4   ka;                                                                      \n" \
		"	float4   kd;                                                                      \n" \
		"	float4   ks;                                                                      \n" \
		"	float    material_shininess;                                                      \n" \
		"                                                                                     \n" \
		"	uint     keyPressed;                                                              \n" \
		"}                                                                                    \n" \
		"                                                                                     \n" \
		"struct vertex_output                                                                 \n" \
		"{                                                                                    \n" \
		"	float4 position        : SV_POSITION;                                             \n" \
		"	float3 tnorm           : NORMAL0;                                                 \n" \
		"	float3 light_direction : NORMAL1;                                                 \n" \
		"	float3 viewer_vector   : NORMAL2;                                                 \n" \
		"};                                                                                   \n" \
		"                                                                                     \n" \
		"vertex_output main(float4 pos: POSITION, float4 normal: NORMAL)                      \n" \
		"{                                                                                    \n" \
		"	vertex_output output;                                                             \n" \
		"                                                                                     \n" \
		"   if (keyPressed == 1)                                                              \n" \
		"   {                                                                                 \n" \
		"       float4 eyeCoordinate = mul(worldMatrix, pos);                                 \n" \
		"       eyeCoordinate = mul(viewMatrix, eyeCoordinate);                               \n" \
		"                                                                                     \n" \
		"       float3 tnorm = mul((float3x3)worldMatrix, (float3)normal);                    \n" \
		"       float3 light_direction = (float3)(lightPosition - eyeCoordinate);             \n" \
		"       float3 viewer_vector = normalize(-eyeCoordinate.xyz);                         \n" \
		"                                                                                     \n" \
		"       output.tnorm = tnorm;                                                         \n" \
		"       output.light_direction = light_direction;                                     \n" \
		"       output.viewer_vector = viewer_vector;                                         \n" \
		"   }                                                                                 \n" \
		"                                                                                     \n" \
		"	float4 position = mul(worldMatrix, pos);                                          \n" \
		"	position = mul(viewMatrix, position);                                             \n" \
		"	position = mul(projectionMatrix, position);                                       \n" \
		"	output.position = position;                                                       \n" \
		"                                                                                     \n" \
		"	return(output);                                                                   \n" \
		"}                                                                                    \n";

	ID3DBlob *pID3DBlob_VertexShaderCode = NULL;
	ID3DBlob *pID3DBlob_Error = NULL;

	hr = D3DCompile(
		vertexShaderSourceCode,                 // source code
		lstrlenA(vertexShaderSourceCode) + 1,   // source code length
		"VS",                                   // vertex shader
		NULL,                                   // #defines
		D3D_COMPILE_STANDARD_FILE_INCLUDE,      // include standard files
		"main",                                 // entry point function's name
		"vs_5_0",                               // vertex shader model 5.0
		0,                                      // compile shader for: debug/optimization/validation etc
		0,                                      // count of shader effects
		&pID3DBlob_VertexShaderCode,            // returns compiled shader code
		&pID3DBlob_Error);                      // returns compilation errors

	if (FAILED(hr))
	{
		if (pID3DBlob_Error != NULL)
		{
			Log("D3DCompile() failed for Vertex Shader: ");
			Log((const char*)pID3DBlob_Error->GetBufferPointer());
			Log("\n");

			pID3DBlob_Error->Release();
			pID3DBlob_Error = NULL;
			return(hr);
		}
	}
	else
	{
		Log("D3DCompile() successful for Vertex Shader()..\n");
	}

	hr = gpID3D11Device->CreateVertexShader(pID3DBlob_VertexShaderCode->GetBufferPointer(),
		pID3DBlob_VertexShaderCode->GetBufferSize(), NULL, &gpID3D11VertexShader);
	if (FAILED(hr))
	{
		Log("ID3D11Device::CreateVertexShader() failed..\n");
		return(hr);
	}
	else
	{
		Log("ID3D11Device::CreateVertexShader() succeeded..\n");
	}

	// set vertex shader in pipeline
	gpID3D11DeviceContext->VSSetShader(gpID3D11VertexShader, NULL, NULL);

	///////////////////////////////////////////////////////////////////////////////
	
	//// pixel shader /////////////////////////////////////////////////////////////
	const char *pixelShaderSourceCode =
		"cbuffer ConstantBuffer                                                               \n" \
		"{                                                                                    \n" \
		"	float4x4 worldMatrix;                                                             \n" \
		"	float4x4 viewMatrix;                                                              \n" \
		"	float4x4 projectionMatrix;                                                        \n" \
		"                                                                                     \n" \
		"	float4   la;                                                                      \n" \
		"	float4   ld;                                                                      \n" \
		"	float4   ls;                                                                      \n" \
		"	float4   lightPosition;                                                           \n" \
		"                                                                                     \n" \
		"	float4   ka;                                                                      \n" \
		"	float4   kd;                                                                      \n" \
		"	float4   ks;                                                                      \n" \
		"	float    material_shininess;                                                      \n" \
		"                                                                                     \n" \
		"	uint     keyPressed;                                                              \n" \
		"}                                                                                    \n" \
		"                                                                                     \n" \
		"struct vertex_output                                                                 \n" \
		"{                                                                                    \n" \
		"	float4 position        : SV_POSITION;                                             \n" \
		"	float3 tnorm           : NORMAL0;                                                 \n" \
		"	float3 light_direction : NORMAL1;                                                 \n" \
		"	float3 viewer_vector   : NORMAL2;                                                 \n" \
		"};                                                                                   \n" \
		"                                                                                     \n" \
		"float4 main(float4 pos: SV_POSITION, vertex_output input): SV_TARGET                 \n" \
		"{                                                                                    \n" \
		"   float4 phong_ads_color = float4(1.0,1.0,1.0,1.0);                                 \n" \
		"                                                                                     \n" \
		"       float3 ntnorm = normalize(input.tnorm);                                       \n" \
		"       float3 nlight_direction = normalize(input.light_direction);                   \n" \
		"       float3 nviewer_vector = normalize(input.viewer_vector);                       \n" \
		"                                                                                     \n" \
		"       float3 reflection_vector = reflect(-nlight_direction, ntnorm);                \n" \
		"       float  tn_dot_ld = max(dot(ntnorm, nlight_direction), 0.0);                   \n" \
		"                                                                                     \n" \
		"       float4 ambient = la * ka;                                                     \n" \
		"       float4 diffuse = ld * kd * tn_dot_ld;                                         \n" \
		"       float4 specular = ls * ks * pow(max(dot(reflection_vector, nviewer_vector), 0.0), material_shininess); \n" \
		"       phong_ads_color = ambient + diffuse + specular;                               \n" \
		"                                                                                     \n" \
		"	float4 color = phong_ads_color;                                                   \n" \
		"	return(color);                                                                    \n" \
		"}                                                                                    \n";

	ID3DBlob *pID3DBlob_PixelShaderCode = NULL;

	hr = D3DCompile(
		pixelShaderSourceCode,                 // source code
		lstrlenA(pixelShaderSourceCode) + 1,   // source code length
		"PS",                                  // pixel shader
		NULL,                                  // #defines
		D3D_COMPILE_STANDARD_FILE_INCLUDE,     // include standard files
		"main",                                // entry point function's name
		"ps_5_0",                              // pixel shader model 5.0
		0,                                     // compile shader for: debug/optimization/validation etc
		0,                                     // count of shader effects
		&pID3DBlob_PixelShaderCode,            // returns compiled shader code
		&pID3DBlob_Error);                     // returns compilation errors

	if (FAILED(hr))
	{
		if (pID3DBlob_Error != NULL)
		{
			Log("D3DCompile() failed for Pixel Shader: ");
			Log((const char*)pID3DBlob_Error->GetBufferPointer());
			Log("\n");

			pID3DBlob_Error->Release();
			pID3DBlob_Error = NULL;
			return(hr);
		}
	}
	else
	{
		Log("D3DCompile() successful for Pixel Shader()..\n");
	}

	hr = gpID3D11Device->CreatePixelShader(pID3DBlob_PixelShaderCode->GetBufferPointer(),
		pID3DBlob_PixelShaderCode->GetBufferSize(), NULL, &gpID3D11PixelShader);
	if (FAILED(hr))
	{
		Log("ID3D11Device::CreatePixelShader() failed..\n");
		return(hr);
	}
	else
	{
		Log("ID3D11Device::CreatePixelShader() succeeded..\n");
	}

	// set pixel shader in pipeline
	gpID3D11DeviceContext->PSSetShader(gpID3D11PixelShader, NULL, NULL);

	///////////////////////////////////////////////////////////////////////////////

	//// create and set input layout //////////////////////////////////////////////
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[2];

	// position data
	inputElementDesc[0].SemanticName = "POSITION";
	inputElementDesc[0].SemanticIndex = 0;
	inputElementDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDesc[0].InputSlot = 0;
	inputElementDesc[0].AlignedByteOffset = 0;
	inputElementDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputElementDesc[0].InstanceDataStepRate = 0;

	// normal data
	inputElementDesc[1].SemanticName = "NORMAL";
	inputElementDesc[1].SemanticIndex = 0;
	inputElementDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDesc[1].InputSlot = 1;
	inputElementDesc[1].AlignedByteOffset = 0;
	inputElementDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputElementDesc[1].InstanceDataStepRate = 0;

	hr = gpID3D11Device->CreateInputLayout(inputElementDesc, ARRAYSIZE(inputElementDesc),
		pID3DBlob_VertexShaderCode->GetBufferPointer(), pID3DBlob_VertexShaderCode->GetBufferSize(),
		&gpID3D11InputLayout);
	if (FAILED(hr))
	{
		Log("ID3D11Device::CreateInputLayout() failed..\n");
		return(hr);
	}
	else
	{
		Log("ID3D11Device::CreateInputLayout() succeeded..\n");
	}

	gpID3D11DeviceContext->IASetInputLayout(gpID3D11InputLayout);

	gpID3D11VertexShader->Release();
	gpID3D11VertexShader = NULL;
	
	gpID3D11PixelShader->Release();
	gpID3D11PixelShader = NULL;

	///////////////////////////////////////////////////////////////////////////////

	//// vertex data //////////////////////////////////////////////////////////////
	
	// Clockwise | Left Hand Rule
	float vertices_Plane[] =
	{
		// triangle 
		 8.0f, 0.0f, -8.0f, // front top
		-8.0f, 0.0f, -8.0f, // front left
		-8.0f, 0.0f,  8.0f, // front right

		 // triangle
		-8.0f, 0.0f,  8.0f, // front right
		 8.0f, 0.0f,  8.0f, // front right
		 8.0f, 0.0f, -8.0f, // front top
	};

	float normals_Plane[] =
	{
		 // triangle of front side
		 0.0f, 1.0f, 0.0f, // R
		 0.0f, 1.0f, 0.0f, // G
		 0.0f, 1.0f, 0.0f, // B

		 // triangle of right side
		 0.0f, 1.0f, 0.0f, // R
		 0.0f, 1.0f, 0.0f, // B
		 0.0f, 1.0f, 0.0f, // G
	};

	float vertices_cube[] =
	{
		// SIDE1 (top)
		// triangle 1
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,
		// triangle 2
		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,

		// SIDE2 (bottom)
		// triangle 1
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		// triangle 2
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, +1.0f,

		// SIDE3 (front)
		// triangle 1
		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		// triangle 2
		-1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		// SIDE4 (back)
		// triangle 1
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,
		// triangle 2
		-1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,

		// SIDE5 (left)
		// triangle 1
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		// triangle 2
		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,

		// SIDE6 (right)
		// triangle 1
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		// triangle 2
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
	};

	float normals_cube[] =
	{
		// SIDE1 (top)
		// triangle 1
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		// triangle 2
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,

		// SIDE2 (bottom)
		// triangle 1
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		// triangle 2
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,

		// SIDE3 (front)
		// triangle 1
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		// triangle 2
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,

		// SIDE4 (back)
		// triangle 1
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		// triangle 2
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,

		// SIDE5 (left)
		// triangle 1
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		// triangle 2
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,

		// SIDE6 (right)
		// triangle 1
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		// triangle 2
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
	};

	///////////////////////////////////////////////////////////////////////////////

	// create vertex buffer for position Plane
	D3D11_BUFFER_DESC bufferDesc_VertexBuffer;
	ZeroMemory(&bufferDesc_VertexBuffer, sizeof(D3D11_BUFFER_DESC));
	bufferDesc_VertexBuffer.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc_VertexBuffer.ByteWidth = sizeof(float)  *ARRAYSIZE(vertices_Plane);
	bufferDesc_VertexBuffer.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc_VertexBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = gpID3D11Device->CreateBuffer(&bufferDesc_VertexBuffer, NULL, &gpID3D11Buffer_VertexBuffer_Plane_Position);
	if (FAILED(hr))
	{
		Log("ID3D11Device::CreateBuffer() failed for vertex buffer position..\n");
		return(hr);
	}
	else
	{
		Log("ID3D11Device::CreateBuffer() succeeded for vertex buffer position..\n");
	}

	// copy vertices_Plane into above buffer
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	ZeroMemory(&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext->Map(gpID3D11Buffer_VertexBuffer_Plane_Position, 0,
		D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	memcpy(mappedSubresource.pData, vertices_Plane, sizeof(vertices_Plane));
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_VertexBuffer_Plane_Position, NULL);

	///////////////////////////////////////////////////////////////////////////////

	// create vertex buffer for color Plane
	ZeroMemory(&bufferDesc_VertexBuffer, sizeof(D3D11_BUFFER_DESC));
	bufferDesc_VertexBuffer.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc_VertexBuffer.ByteWidth = sizeof(float)  *ARRAYSIZE(normals_Plane);
	bufferDesc_VertexBuffer.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc_VertexBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = gpID3D11Device->CreateBuffer(&bufferDesc_VertexBuffer, NULL, &gpID3D11Buffer_VertexBuffer_Plane_Normal);
	if (FAILED(hr))
	{
		Log("ID3D11Device::CreateBuffer() failed for vertex buffer color..\n");
		return(hr);
	}
	else
	{
		Log("ID3D11Device::CreateBuffer() succeeded for vertex buffer color..\n");
	}

	// copy normals_Plane into above buffer
	ZeroMemory(&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext->Map(gpID3D11Buffer_VertexBuffer_Plane_Normal, 0,
		D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	memcpy(mappedSubresource.pData, normals_Plane, sizeof(normals_Plane));
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_VertexBuffer_Plane_Normal, NULL);

	///////////////////////////////////////////////////////////////////////////////

	// create vertex buffer for position cube
	ZeroMemory(&bufferDesc_VertexBuffer, sizeof(D3D11_BUFFER_DESC));
	bufferDesc_VertexBuffer.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc_VertexBuffer.ByteWidth = sizeof(float)  *ARRAYSIZE(vertices_cube);
	bufferDesc_VertexBuffer.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc_VertexBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = gpID3D11Device->CreateBuffer(&bufferDesc_VertexBuffer, NULL, &gpID3D11Buffer_VertexBuffer_Cube_Position);
	if (FAILED(hr))
	{
		Log("ID3D11Device::CreateBuffer() failed for vertex buffer position..\n");
		return(hr);
	}
	else
	{
		Log("ID3D11Device::CreateBuffer() succeeded for vertex buffer position..\n");
	}

	// copy vertices_cube into above buffer
	ZeroMemory(&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext->Map(gpID3D11Buffer_VertexBuffer_Cube_Position, 0,
		D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	memcpy(mappedSubresource.pData, vertices_cube, sizeof(vertices_cube));
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_VertexBuffer_Cube_Position, NULL);

	///////////////////////////////////////////////////////////////////////////////

	// create vertex buffer for color cube
	ZeroMemory(&bufferDesc_VertexBuffer, sizeof(D3D11_BUFFER_DESC));
	bufferDesc_VertexBuffer.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc_VertexBuffer.ByteWidth = sizeof(float)  *ARRAYSIZE(normals_cube);
	bufferDesc_VertexBuffer.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc_VertexBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = gpID3D11Device->CreateBuffer(&bufferDesc_VertexBuffer, NULL, &gpID3D11Buffer_VertexBuffer_Cube_Normal);
	if (FAILED(hr))
	{
		Log("ID3D11Device::CreateBuffer() failed for vertex buffer color..\n");
		return(hr);
	}
	else
	{
		Log("ID3D11Device::CreateBuffer() succeeded for vertex buffer color..\n");
	}

	// copy normals_cube into above buffer
	ZeroMemory(&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext->Map(gpID3D11Buffer_VertexBuffer_Cube_Normal, 0,
		D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	memcpy(mappedSubresource.pData, normals_cube, sizeof(normals_cube));
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_VertexBuffer_Cube_Normal, NULL);

	///////////////////////////////////////////////////////////////////////////////

	//// constant data ////////////////////////////////////////////////////////////
	// define and set constant buffer
	D3D11_BUFFER_DESC bufferDesc_ConstantBuffer;
	ZeroMemory(&bufferDesc_ConstantBuffer, sizeof(D3D11_BUFFER_DESC));
	bufferDesc_ConstantBuffer.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc_ConstantBuffer.ByteWidth = sizeof(CBUFFER);
	bufferDesc_ConstantBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	hr = gpID3D11Device->CreateBuffer(&bufferDesc_ConstantBuffer, NULL, &gpID3D11Buffer_ConstantBuffer);
	if (FAILED(hr))
	{
		Log("ID3D11Device::CreateBuffer() failed for constant buffer..\n");
		return(hr);
	}
	else
	{
		Log("ID3D11Device::CreateBuffer() succeeded for constant buffer..\n");
	}

	gpID3D11DeviceContext->VSSetConstantBuffers(0, 1, &gpID3D11Buffer_ConstantBuffer);
	gpID3D11DeviceContext->PSSetConstantBuffers(0, 1, &gpID3D11Buffer_ConstantBuffer);

	///////////////////////////////////////////////////////////////////////////////

	//// rasterization state //////////////////////////////////////////////////////

	// in D3D, backface culling is by default ON
	// means the backface of geometry will not be visible
	// this causes our 2D triangles backface to vanish in rotation
	// so set the culling OFF
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	rasterizerDesc.AntialiasedLineEnable = FALSE;
	rasterizerDesc.CullMode = D3D11_CULL_NONE; // this disables culling
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;

	hr = gpID3D11Device->CreateRasterizerState(&rasterizerDesc, &gpID3D11RasterizerState);
	if (FAILED(hr))
	{
		Log("ID3D11Device::CreateRasterizerState() failed..\n");
		return(hr);
	}
	else
	{
		Log("ID3D11Device::CreateRasterizerState() succeeded..\n");
	}
	
	gpID3D11DeviceContext->RSSetState(gpID3D11RasterizerState);

	///////////////////////////////////////////////////////////////////////////////

	// d3d clear color
	gClearColor[0] = 0.0f;
	gClearColor[1] = 0.0f;
	gClearColor[2] = 0.0f;
	gClearColor[3] = 1.0f;

	// set projection matrix
	gPerspectiveProjectionMatrix = XMMatrixIdentity();

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
	if (gpID3D11DepthStencilView)
	{
		gpID3D11DepthStencilView->Release();
		gpID3D11DepthStencilView = NULL;
	}
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

	// create depth stencil buffer (or zbuffer)
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));
	textureDesc.Width = (UINT)width;
	textureDesc.Height = (UINT)height;
	textureDesc.ArraySize = 1;
	textureDesc.MipLevels = 1;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Format = DXGI_FORMAT_D32_FLOAT;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	ID3D11Texture2D *pID3D11Texture2D_DepthBuffer;
	gpID3D11Device->CreateTexture2D(&textureDesc, NULL, &pID3D11Texture2D_DepthBuffer);

	// create depth stencil view from above depth stencil buffer
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

	depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	hr = gpID3D11Device->CreateDepthStencilView(pID3D11Texture2D_DepthBuffer, &depthStencilViewDesc, &gpID3D11DepthStencilView);
	if (FAILED(hr))
	{
		Log("ID3D11::CreateDepthStencilView() failed..\n");
		return(hr);
	}
	else
	{
		Log("ID3D11::CreateDepthStencilView() succeeded..\n");
	}

	pID3D11Texture2D_DepthBuffer->Release();
	pID3D11Texture2D_DepthBuffer = NULL;	

	// set render target view as render target
	gpID3D11DeviceContext->OMSetRenderTargets(1, &gpID3D11RenderTargetView, gpID3D11DepthStencilView);

	// set viewport
	D3D11_VIEWPORT d3dViewport;
	d3dViewport.TopLeftX = 0.0f;
	d3dViewport.TopLeftY = 0.0f;
	d3dViewport.Width  = (FLOAT)width;
	d3dViewport.Height = (FLOAT)height;
	d3dViewport.MinDepth = 0.0f;
	d3dViewport.MaxDepth = 1.0f;

	gpID3D11DeviceContext->RSSetViewports(1, &d3dViewport);

	// set perspective matrix
	gPerspectiveProjectionMatrix = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(45.0f),
		(FLOAT)width / (FLOAT)height,
		0.1f,
		100.0f
	);

	return(hr);
}

void display(void)
{
	// code
	// clear render target view to a chosen color
	gpID3D11DeviceContext->ClearRenderTargetView(gpID3D11RenderTargetView, gClearColor);
	gpID3D11DeviceContext->ClearDepthStencilView(gpID3D11DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	//// Plane ////////////////////////////////////////////////////////////////////

	// select which vertex buffer to display
	// position buffer
	UINT stride = sizeof(float)  *3;
	UINT offset = 0;
	gpID3D11DeviceContext->IASetVertexBuffers(0, 1, &gpID3D11Buffer_VertexBuffer_Plane_Position, &stride, &offset);

	// color buffer
	stride = sizeof(float)  *3;
	offset = 0;
	gpID3D11DeviceContext->IASetVertexBuffers(1, 1, &gpID3D11Buffer_VertexBuffer_Plane_Normal, &stride, &offset);

	// select geometry primitive
	gpID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// translation is concerned with world matrix transformation
	XMMATRIX worldMatrix = XMMatrixIdentity();
	XMMATRIX viewMatrix = XMMatrixIdentity();
	XMMATRIX translationMatrix = XMMatrixIdentity();
	XMMATRIX rotationMatrix = XMMatrixIdentity();

	// translation
	translationMatrix = XMMatrixTranslation(0.0f, -1.0f, 0.0f);

	// this order of multiplication is important!
	worldMatrix = rotationMatrix * translationMatrix;

	// load the data into the constant buffer
	CBUFFER constantBuffer;
	ZeroMemory(&constantBuffer, sizeof(CBUFFER));
	constantBuffer.WorldMatrix = worldMatrix;
	constantBuffer.ViewMatrix = viewMatrix;
	constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;
	constantBuffer.KeyPressed = 1;

	constantBuffer.La = XMVectorSet(lightAmbient[0], lightAmbient[1], lightAmbient[2], lightAmbient[3]);
	constantBuffer.Ld = XMVectorSet(lightDiffuse[0], lightDiffuse[1], lightDiffuse[2], lightDiffuse[3]);
	constantBuffer.Ls = XMVectorSet(lightSpecular[0], lightSpecular[1], lightSpecular[2], lightSpecular[3]);
	constantBuffer.LightPosition = XMVectorSet(lightPosition[0], lightPosition[1], lightPosition[2], lightPosition[3]);

	constantBuffer.Ka = XMVectorSet(materialAmbient[0], materialAmbient[1], materialAmbient[2], materialAmbient[3]);
	constantBuffer.Kd = XMVectorSet(materialDiffuse[0], materialDiffuse[1], materialDiffuse[2], materialDiffuse[3]);
	constantBuffer.Ks = XMVectorSet(materialSpecular[0], materialSpecular[1], materialSpecular[2], materialSpecular[3]);
	constantBuffer.Material_Shininess = materialShininess;

	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer, 0, NULL, &constantBuffer, 0, 0);

	// draw vertex buffer to render target
	gpID3D11DeviceContext->Draw(6, 0);

	///////////////////////////////////////////////////////////////////////////////

	//// cube /////////////////////////////////////////////////////////////////////

	// select which vertex buffer to display
	// position buffer
	stride = sizeof(float)  *3;
	offset = 0;
	gpID3D11DeviceContext->IASetVertexBuffers(0, 1, &gpID3D11Buffer_VertexBuffer_Cube_Position, &stride, &offset);

	// color buffer
	stride = sizeof(float)  *3;
	offset = 0;
	gpID3D11DeviceContext->IASetVertexBuffers(1, 1, &gpID3D11Buffer_VertexBuffer_Cube_Normal, &stride, &offset);

	// select geometry primitive
	gpID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// translation is concerned with world matrix transformation
	worldMatrix = XMMatrixIdentity();
	viewMatrix = XMMatrixIdentity();

	// translation
	translationMatrix = XMMatrixTranslation(1.0f, 0.0f, 6.0f);
	rotationMatrix = XMMatrixRotationY(XMConvertToRadians(-gAngleCube));

	// this order of multiplication is important!
	worldMatrix = rotationMatrix * translationMatrix;

	// load the data into the constant buffer
	constantBuffer.WorldMatrix = worldMatrix;
	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer, 0, NULL, &constantBuffer, 0, 0);

	// draw vertex buffer to render target
	gpID3D11DeviceContext->Draw(6, 0);
	gpID3D11DeviceContext->Draw(6, 6);
	gpID3D11DeviceContext->Draw(6, 12);
	gpID3D11DeviceContext->Draw(6, 18);
	gpID3D11DeviceContext->Draw(6, 24);
	gpID3D11DeviceContext->Draw(6, 30);

	///////////////////////////////////////////////////////////////////////////////

	// switch between front and back buffer
	gpIDXGISwapChain->Present(1, 0);
}

void update(void)
{
	gAngleCube += 1.0f;
	if (gAngleCube >= 360.0f)
		gAngleCube = 0.0f;
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

	if (gpID3D11Buffer_VertexBuffer_Plane_Normal)
	{
		gpID3D11Buffer_VertexBuffer_Plane_Normal->Release();
		gpID3D11Buffer_VertexBuffer_Plane_Normal = NULL;
	}

	if (gpID3D11Buffer_VertexBuffer_Plane_Position)
	{
		gpID3D11Buffer_VertexBuffer_Plane_Position->Release();
		gpID3D11Buffer_VertexBuffer_Plane_Position = NULL;
	}

	if (gpID3D11Buffer_VertexBuffer_Cube_Normal)
	{
		gpID3D11Buffer_VertexBuffer_Cube_Normal->Release();
		gpID3D11Buffer_VertexBuffer_Cube_Normal = NULL;
	}

	if (gpID3D11Buffer_VertexBuffer_Cube_Position)
	{
		gpID3D11Buffer_VertexBuffer_Cube_Position->Release();
		gpID3D11Buffer_VertexBuffer_Cube_Position = NULL;
	}

	if (gpID3D11PixelShader)
	{
		gpID3D11PixelShader->Release();
		gpID3D11PixelShader = NULL;
	}

	if (gpID3D11VertexShader)
	{
		gpID3D11VertexShader->Release();
		gpID3D11VertexShader = NULL;
	}

	if (gpID3D11RasterizerState)
	{
		gpID3D11RasterizerState->Release();
		gpID3D11RasterizerState = NULL;
	}

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


void Log(const char *str)
{
	fopen_s(&gpFile, gszLogFile, "a+");
	fprintf_s(gpFile, str);
	fclose(gpFile);
}
