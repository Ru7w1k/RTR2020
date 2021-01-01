// headers
#include <Windows.h>
#include <ShellScalingApi.h>
#include <stdio.h> // for file IO

#include <d3d11.h>        
#include <d3dcompiler.h>  // for shader compilation

#pragma warning(disable:4838)
#include "XNAMath\xnamath.h"
#include "WICTextureLoader.h"

#include "main.h"
#include "Model.h"

// linker commands
#pragma comment(lib, "shcore.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "DirectXTK.lib")
#pragma comment(lib, "assimp-vc141-mtd.lib")

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

ID3D11Buffer *gpID3D11Buffer_VertexBuffer_Position = NULL;
ID3D11Buffer *gpID3D11Buffer_VertexBuffer_Normal = NULL;
ID3D11Buffer *gpID3D11Buffer_IndexBuffer = NULL;

ID3D11InputLayout *gpID3D11InputLayout = NULL;
ID3D11Buffer *gpID3D11Buffer_ConstantBuffer = NULL;

ID3D11RasterizerState *gpID3D11RasterizerState = NULL;
ID3D11DepthStencilView *gpID3D11DepthStencilView = NULL;

ID3D11ShaderResourceView *gpID3D11ShaderResourceView = NULL;
ID3D11SamplerState *gpID3D11SamplerState = NULL;

unsigned int gNumElements;

// uniforms
struct CBUFFER
{
	XMMATRIX WorldMatrix;
	XMMATRIX ViewMatrix;
	XMMATRIX ProjectionMatrix;
	XMMATRIX BoneMatrix[100];

	XMVECTOR La[4];
	XMVECTOR Ld[4];
	XMVECTOR Ls[4];
	XMVECTOR LightPosition[4];

	XMVECTOR Ka;
	XMVECTOR Kd;
	XMVECTOR Ks;
	float Material_Shininess;

	unsigned int KeyPressed;
	
};

XMMATRIX gPerspectiveProjectionMatrix;
bool bLight = false;

float lightAmbient[4][4] = {
	{ 0.0f, 0.0f, 0.0f, 1.0f },
	{ 0.0f, 0.0f, 0.0f, 1.0f },
	{ 0.0f, 0.0f, 0.0f, 1.0f },
	{ 0.0f, 0.0f, 0.0f, 1.0f }};

float lightDiffuse[4][4] = {
	{ 1.0f, 1.0f, 0.0f, 1.0f },
	{ 1.0f, 0.0f, 0.0f, 1.0f },
	{ 0.0f, 1.0f, 0.0f, 1.0f },
	{ 0.0f, 0.0f, 1.0f, 1.0f } };

float lightSpecular[4][4] = {
	{ 1.0f, 1.0f, 0.0f, 1.0f },
	{ 1.0f, 0.0f, 0.0f, 1.0f },
	{ 0.0f, 1.0f, 0.0f, 1.0f },
	{ 0.0f, 0.0f, 1.0f, 1.0f }};

float lightPosition[4][4] = {
	{ 0.0f, 0.0f, 0.0f, 1.0f },
	{ 0.0f, 0.0f, 0.0f, 1.0f },
	{ 0.0f, 0.0f, 0.0f, 1.0f },
	{ 0.0f, 0.0f, 0.0f, 1.0f }};

float materialAmbient[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
float materialDiffuse[4] = { 0.8f, 0.8f, 0.8f, 0.8f };
float materialSpecular[4] = { 0.8f, 0.8f, 0.8f, 0.8f };
float materialShininess = 128.0f;

// model related global data
uint boneCount = 0;
Animation animation;
Bone skeleton;
XMMATRIX globalInverseTransform;

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

	// set DPI awareness for scaling
	SetProcessDpiAwareness(PROCESS_SYSTEM_DPI_AWARE);

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
		TEXT("DirectX | Model Loading"),
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

	case WM_CHAR:
		switch (wParam)
		{
			case 'L':
			case 'l':
				bLight = !bLight;
				break;
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
	HRESULT LoadD3DTexture(const wchar_t*, ID3D11ShaderResourceView **);
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
		"	float4x4 boneMatrix[100];                                                         \n" \
		"                                                                                     \n" \
		"	float4   la[4];                                                                   \n" \
		"	float4   ld[4];                                                                   \n" \
		"	float4   ls[4];                                                                   \n" \
		"	float4   lightPosition[4];                                                        \n" \
		"                                                                                     \n" \
		"	float4   ka;                                                                      \n" \
		"	float4   kd;                                                                      \n" \
		"	float4   ks;                                                                      \n" \
		"	float    material_shininess;                                                      \n" \
		"                                                                                     \n" \
		"	uint     keyPressed;                                                              \n" \
		"}                                                                                    \n" \
		"                                                                                     \n" \
		"struct vertex_input                                                                  \n" \
		"{                                                                                    \n" \
		"	float4 pos         : POSITION;                                                    \n" \
		"	float4 normal      : NORMAL;                                                      \n" \
		"	float2 texcoord    : TEXCOORD;                                                    \n" \
		"	int4   boneIds     : BONEIDS;                                                     \n" \
		"	float4 boneWeights : BONEWEIGHTS;                                                 \n" \
		"	uint instid        : SV_InstanceID;                                               \n" \
		"};                                                                                   \n" \
		"                                                                                     \n" \
		"struct vertex_output                                                                 \n" \
		"{                                                                                    \n" \
		"	float4 position           : SV_POSITION;                                          \n" \
		"	float3 tnorm              : NORMAL0;                                              \n" \
		"	float3 light_direction[4] : NORMAL1;                                              \n" \
		"	float3 viewer_vector      : NORMAL5;                                              \n" \
		"	float2 texcoord           : TEXCOORD0;                                            \n" \
		"};                                                                                   \n" \
		"                                                                                     \n" \
		"vertex_output main(vertex_input input)                                               \n" \
		"{                                                                                    \n" \
		"	vertex_output output;                                                             \n" \
		"   float4x4 boneTransform;                                                           \n" \
		"                                                                                     \n" \
		"   boneTransform = input.boneWeights.x * boneMatrix[input.boneIds.x];                \n" \
		"   boneTransform += input.boneWeights.y * boneMatrix[input.boneIds.y];               \n" \
		"   boneTransform += input.boneWeights.z * boneMatrix[input.boneIds.z];               \n" \
		"   boneTransform += input.boneWeights.w * boneMatrix[input.boneIds.w];               \n" \
		"                                                                                     \n" \
		"   float4 pos = mul(boneTransform, float4(input.pos.xyz, 1.0));                      \n" \
		"   float4 normal = mul(boneTransform, float4(input.normal.xyz, 0.0));                \n" \
		"   pos.x += 2.5*input.instid;                                                        \n" \
		"   pos.z += 2.5*input.instid;                                                        \n" \
		"                                                                                     \n" \
		"   if (keyPressed == 1)                                                              \n" \
		"   {                                                                                 \n" \
		"       float4 eyeCoordinate = mul(worldMatrix, pos);                                 \n" \
		"       eyeCoordinate = mul(viewMatrix, eyeCoordinate);                               \n" \
		"                                                                                     \n" \
		"       float3 tnorm = mul((float3x3)worldMatrix, (float3)normal);                    \n" \
		"       float3 viewer_vector = normalize(-eyeCoordinate.xyz);                         \n" \
		"                                                                                     \n" \
		"       for(int i = 0; i < 4; i++)                                                    \n" \
		"       {                                                                             \n" \
		"           float3 light_direction = (float3)(lightPosition[i] - eyeCoordinate);      \n" \
		"           output.light_direction[i] = light_direction;                              \n" \
		"       }                                                                             \n" \
		"                                                                                     \n" \
		"       output.tnorm = tnorm;                                                         \n" \
		"       output.viewer_vector = viewer_vector;                                         \n" \
		"   }                                                                                 \n" \
		"                                                                                     \n" \
		"	float4 position = mul(worldMatrix, pos);                                          \n" \
		"	position = mul(viewMatrix, position);                                             \n" \
		"	position = mul(projectionMatrix, position);                                       \n" \
		"	output.position = position;                                                       \n" \
		"	output.texcoord = input.texcoord;                                                 \n" \
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
		"	float4x4 boneMatrix[100];                                                         \n" \
		"                                                                                     \n" \
		"	float4   la[4];                                                                   \n" \
		"	float4   ld[4];                                                                   \n" \
		"	float4   ls[4];                                                                   \n" \
		"	float4   lightPosition[4];                                                        \n" \
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
		"	float3 light_direction[4] : NORMAL1;                                                 \n" \
		"	float3 viewer_vector   : NORMAL5;                                                 \n" \
		"	float2 texcoord        : TEXCOORD0;                                               \n" \
		"};                                                                                   \n" \
		"                                                                                     \n" \
		"Texture2D myTexture2D;                                                               \n" \
		"SamplerState mySamplerState;                                                         \n" \
		"                                                                                     \n" \
		"float4 main(float4 pos: SV_POSITION, vertex_output input): SV_TARGET                 \n" \
		"{                                                                                    \n" \
		"   float4 phong_ads_color = float4(1.0,1.0,1.0,1.0);                                 \n" \
		"                                                                                     \n" \
		"	if (keyPressed == 1)                                                              \n" \
		"   {                                                                                 \n" \
		"       float3 ntnorm = normalize(input.tnorm);                                       \n" \
		"       float3 nviewer_vector = normalize(input.viewer_vector);                       \n" \
		"                                                                                     \n" \
		"                                                                                     \n" \
		"       for(int i = 0; i < 4; i++)                                                    \n" \
		"       {                                                                             \n" \
		"           float3 nlight_direction = normalize(input.light_direction[i]);            \n" \
		"           float3 reflection_vector = reflect(-nlight_direction, ntnorm);            \n" \
		"           float  tn_dot_ld = max(dot(ntnorm, nlight_direction), 0.0);               \n" \
		"                                                                                     \n" \
		"           float4 ambient = la[i] * ka;                                              \n" \
		"           float4 diffuse = ld[i] * kd * tn_dot_ld;                                  \n" \
		"           float4 specular = ls[i] * ks * pow(max(dot(reflection_vector, nviewer_vector), 0.0), material_shininess); \n" \
		"           phong_ads_color += ambient + diffuse + specular;                          \n" \
		"       }                                                                             \n" \
		"   }                                                                                 \n" \
		"                                                                                     \n" \
		"	float4 color = myTexture2D.Sample(mySamplerState, input.texcoord);                \n" \
		"   color *= phong_ads_color;                                                         \n" \
		"                                                                                     \n" \
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
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[5];

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
	inputElementDesc[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	inputElementDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputElementDesc[1].InstanceDataStepRate = 0;

	// texture data
	inputElementDesc[2].SemanticName = "TEXCOORD";
	inputElementDesc[2].SemanticIndex = 0;
	inputElementDesc[2].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDesc[2].InputSlot = 2;
	inputElementDesc[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	inputElementDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputElementDesc[2].InstanceDataStepRate = 0;

	// bone ids
	inputElementDesc[3].SemanticName = "BONEIDS";
	inputElementDesc[3].SemanticIndex = 0;
	inputElementDesc[3].Format = DXGI_FORMAT_R32G32B32A32_UINT;
	inputElementDesc[3].InputSlot = 3;
	inputElementDesc[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	inputElementDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputElementDesc[3].InstanceDataStepRate = 0;

	// bone weights
	inputElementDesc[4].SemanticName = "BONEWEIGHTS";
	inputElementDesc[4].SemanticIndex = 0;
	inputElementDesc[4].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDesc[4].InputSlot = 4;
	inputElementDesc[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	inputElementDesc[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputElementDesc[4].InstanceDataStepRate = 0;

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

	//// vertex data from model ///////////////////////////////////////////////////

	Assimp::Importer importer;
	//const char* filePath = "model/stormtrooper.dae";
	const char* filePath = "model/man.dae";
	const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_MakeLeftHanded | aiProcess_FlipWindingOrder);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		Log("Assimp Error: ");
		Log(importer.GetErrorString());
		Log("\n");
	}

	Log("assimp: file parsing done..\n");
	aiMesh *mesh = scene->mMeshes[0];

	vector<Vertex> vertices = {};
	vector<uint> indices = {};

	// inverse the global transformation matrix
	XMVECTOR det;
	globalInverseTransform = assimpToXMMATRIX(scene->mRootNode->mTransformation);
	globalInverseTransform = XMMatrixInverse(&det, globalInverseTransform);

	Log("assimp: loading model..\n");
	loadModel(scene, mesh, &vertices, &indices, &skeleton, &boneCount);
	Log("assimp: model loading finished..\n");
	Log("assimp: loading animation..\n");
	loadAnimation(scene, animation);
	Log("assimp: animation loading finished..\n");

	gNumElements = indices.size();

	///////////////////////////////////////////////////////////////////////////////

	// create vertex buffer 
	D3D11_BUFFER_DESC bufferDesc_VertexBuffer;
	ZeroMemory(&bufferDesc_VertexBuffer, sizeof(D3D11_BUFFER_DESC));
	bufferDesc_VertexBuffer.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc_VertexBuffer.ByteWidth = sizeof(Vertex) * vertices.size();
	bufferDesc_VertexBuffer.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc_VertexBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = gpID3D11Device->CreateBuffer(&bufferDesc_VertexBuffer, NULL, &gpID3D11Buffer_VertexBuffer_Position);
	if (FAILED(hr))
	{
		Log("ID3D11Device::CreateBuffer() failed for vertex buffer..\n");
		return(hr);
	}
	else
	{
		Log("ID3D11Device::CreateBuffer() succeeded for vertex buffer..\n");
	}

	// copy vertex data into above buffer
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	ZeroMemory(&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext->Map(gpID3D11Buffer_VertexBuffer_Position, 0,
		D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	memcpy(mappedSubresource.pData, &vertices[0], sizeof(Vertex) * vertices.size());
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_VertexBuffer_Position, NULL);

	///////////////////////////////////////////////////////////////////////////////
	
	// create index buffer
	ZeroMemory(&bufferDesc_VertexBuffer, sizeof(D3D11_BUFFER_DESC));
	bufferDesc_VertexBuffer.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc_VertexBuffer.ByteWidth = indices.size() * sizeof(uint);
	bufferDesc_VertexBuffer.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc_VertexBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = gpID3D11Device->CreateBuffer(&bufferDesc_VertexBuffer, NULL, &gpID3D11Buffer_IndexBuffer);
	if (FAILED(hr))
	{
		Log("ID3D11Device::CreateBuffer() failed for index buffer..\n");
		return(hr);
	}
	else
	{
		Log("ID3D11Device::CreateBuffer() succeeded for index buffer..\n");
	}

	// copy index data into above buffer
	ZeroMemory(&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext->Map(gpID3D11Buffer_IndexBuffer, 0,
		D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	memcpy(mappedSubresource.pData, &indices[0], indices.size() * sizeof(uint));
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_IndexBuffer, NULL);

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

	//// diffuse texture //////////////////////////////////////////////////////////

	// create texture resource and texture view
	hr = LoadD3DTexture(L"model//diffuse.png", &gpID3D11ShaderResourceView);
	if (FAILED(hr))
	{
		Log("LoadD3DTexture() failed..\n");
		return(hr);
	}
	else
	{
		Log("LoadD3DTexture() succeeded..\n");
	}

	// create sampler state
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	hr = gpID3D11Device->CreateSamplerState(&samplerDesc, &gpID3D11SamplerState);
	if (FAILED(hr))
	{
		Log("ID3D11Device::CreateSamplerState() failed..\n");
	}
	else 
	{
		Log("ID3D11Device::CreateSamplerState() succeeded..\n");
	}

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
	gClearColor[0] = 0.2f;
	gClearColor[1] = 0.2f;
	gClearColor[2] = 0.2f;
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

	//// sphere ////////////////////////////////////////////////////////////////////

	// select which vertex buffer to display
	// position buffer
	UINT stride = sizeof(Vertex);
	UINT offset = offsetof(Vertex, position);
	gpID3D11DeviceContext->IASetVertexBuffers(0, 1, &gpID3D11Buffer_VertexBuffer_Position, &stride, &offset);

	// normal buffer
	stride = sizeof(Vertex);
	offset = offsetof(Vertex, normal);
	gpID3D11DeviceContext->IASetVertexBuffers(1, 1, &gpID3D11Buffer_VertexBuffer_Position, &stride, &offset);

	// texcoord buffer
	stride = sizeof(Vertex);
	offset = offsetof(Vertex, uv);
	gpID3D11DeviceContext->IASetVertexBuffers(2, 1, &gpID3D11Buffer_VertexBuffer_Position, &stride, &offset);

	// bone ids buffer
	stride = sizeof(Vertex);
	offset = offsetof(Vertex, boneIDs);
	gpID3D11DeviceContext->IASetVertexBuffers(3, 1, &gpID3D11Buffer_VertexBuffer_Position, &stride, &offset);

	// texcoord buffer
	stride = sizeof(Vertex);
	offset = offsetof(Vertex, boneWeights);
	gpID3D11DeviceContext->IASetVertexBuffers(4, 1, &gpID3D11Buffer_VertexBuffer_Position, &stride, &offset);

	// index buffer
	gpID3D11DeviceContext->IASetIndexBuffer(gpID3D11Buffer_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// select geometry primitive
	gpID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// bind texture and sampler as pixel shader resource
	gpID3D11DeviceContext->PSSetShaderResources(0, 1, &gpID3D11ShaderResourceView);
	gpID3D11DeviceContext->PSSetSamplers(0, 1, &gpID3D11SamplerState);

	// translation is concerned with world matrix transformation
	XMMATRIX identity = XMMatrixIdentity();
	XMMATRIX worldMatrix = XMMatrixIdentity();
	XMMATRIX viewMatrix = XMMatrixIdentity();
	XMMATRIX translationMatrix = XMMatrixIdentity();
	XMMATRIX rotationMatrix = XMMatrixIdentity();
	XMMATRIX rotationMatrix1 = XMMatrixIdentity();
	XMMATRIX scaleMatrix = XMMatrixIdentity();

	// camera
	viewMatrix = XMMatrixLookAtLH(
		XMVectorSet(0.0f, 0.0f, -20.0f, 0.0f),
		XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
		XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
	);

	// translation
	static float angle = 180.0f;
	translationMatrix = XMMatrixTranslation(0.0f, -4.0f, 0.0f);
	rotationMatrix = XMMatrixRotationY(XMConvertToRadians(angle));
	rotationMatrix1 = XMMatrixRotationZ(XMConvertToRadians(180.0f));
	//scaleMatrix = XMMatrixScaling(0.4f, 0.4f, 0.4f);
	angle += 0.1f;

	// this order of multiplication is important!
	worldMatrix = scaleMatrix * rotationMatrix1 * rotationMatrix * translationMatrix;

	// get the current pos
	vector<XMMATRIX> currentPose = {};
	XMMATRIX m = XMMatrixRotationY(XMConvertToRadians(45.0f));
	currentPose.resize(boneCount, m);
	static float elapsedTime = 0.00001f;
	getPose(animation, skeleton, elapsedTime, currentPose, identity, globalInverseTransform);
	elapsedTime += 0.01f;

	// load the data into the constant buffer
	CBUFFER constantBuffer;
	ZeroMemory(&constantBuffer, sizeof(CBUFFER));
	constantBuffer.WorldMatrix = worldMatrix;
	constantBuffer.ViewMatrix = viewMatrix;
	constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;
	
	for (int i = 0; i < boneCount; i++)
		constantBuffer.BoneMatrix[i] = currentPose[i];

	if (bLight)
	{
		constantBuffer.KeyPressed = 1;

		for (int i = 0; i < 4; i++)
		{
			constantBuffer.La[i] = XMVectorSet(lightAmbient[i][0], lightAmbient[i][1], lightAmbient[i][2], lightAmbient[i][3]);
			constantBuffer.Ld[i] = XMVectorSet(lightDiffuse[i][0], lightDiffuse[i][1], lightDiffuse[i][2], lightDiffuse[i][3]);
			constantBuffer.Ls[i] = XMVectorSet(lightSpecular[i][0], lightSpecular[i][1], lightSpecular[i][2], lightSpecular[i][3]);
			constantBuffer.LightPosition[i] = XMVectorSet(lightPosition[i][0], lightPosition[i][1], lightPosition[i][2], lightPosition[i][3]);
		}

		constantBuffer.Ka = XMVectorSet(materialAmbient[0], materialAmbient[1], materialAmbient[2], materialAmbient[3]);
		constantBuffer.Kd = XMVectorSet(materialDiffuse[0], materialDiffuse[1], materialDiffuse[2], materialDiffuse[3]);
		constantBuffer.Ks = XMVectorSet(materialSpecular[0], materialSpecular[1], materialSpecular[2], materialSpecular[3]);
		constantBuffer.Material_Shininess = materialShininess;
	}
	else
	{
		constantBuffer.KeyPressed = 0;
	}

	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer, 0, NULL, &constantBuffer, 0, 0);

	// draw vertex buffer to render target
	gpID3D11DeviceContext->DrawIndexedInstanced(gNumElements, 1, 0, 0, 0);

	///////////////////////////////////////////////////////////////////////////////

	// switch between front and back buffer
	gpIDXGISwapChain->Present(1, 0);
}

void update(void)
{
	static float angle = 0.0f;
	if (angle < 360.0f) angle += 1.0f;
	else angle = 0.0f;

	for (int i = 0; i < 4; i++)
	{
		lightPosition[i][0] = 100.0f * cosf(XMConvertToRadians(angle + (90.0f * i)));
		lightPosition[i][1] = 0.0f;
		lightPosition[i][2] = 100.0f * sinf(XMConvertToRadians(angle + (90.0f * i)));
	}
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

	if (gpID3D11Buffer_VertexBuffer_Normal)
	{
		gpID3D11Buffer_VertexBuffer_Normal->Release();
		gpID3D11Buffer_VertexBuffer_Normal = NULL;
	}

	if (gpID3D11Buffer_VertexBuffer_Position)
	{
		gpID3D11Buffer_VertexBuffer_Position->Release();
		gpID3D11Buffer_VertexBuffer_Position = NULL;
	}

	if (gpID3D11Buffer_IndexBuffer)
	{
		gpID3D11Buffer_IndexBuffer->Release();
		gpID3D11Buffer_IndexBuffer = NULL;
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

HRESULT LoadD3DTexture(const wchar_t *textureFileName, ID3D11ShaderResourceView **ppID3D11ShaderResourceView)
{
	// code
	HRESULT hr;

	// create texture
	hr = DirectX::CreateWICTextureFromFile(gpID3D11Device, textureFileName, NULL, ppID3D11ShaderResourceView);
	if (FAILED(hr))
	{
		Log("DirectX::CreateWICTextureFromFile() failed..\n");
		return(hr);
	}
	else
	{
		Log("DirectX::CreateWICTextureFromFile() succeeded..\n");
	}
	return hr;
}

void Log(const char *str)
{
	fopen_s(&gpFile, gszLogFile, "a+");
	fprintf_s(gpFile, str);
	fclose(gpFile);
}



