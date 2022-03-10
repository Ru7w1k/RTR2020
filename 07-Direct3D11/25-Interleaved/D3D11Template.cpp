/*
 * Direct3D - Interleaved
 */

#include <Windows.h>
#include <stdio.h>  // for file I/O

#include <d3d11.h>
#include <d3dcompiler.h> // for shader compilation

#pragma warning(disable:4838)
#include "XNAMath\xnamath.h"
#include "WICTextureLoader.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "DirectXTK.lib")

#define WIN_WIDTH  800
#define WIN_HEIGHT 600

using namespace DirectX;

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
ID3D11PixelShader *gpID3D11PixelShader = NULL;

ID3D11Buffer *gpID3D11Buffer_VertexBuffer = NULL;

ID3D11InputLayout *gpID3D11InputLayout = NULL;
ID3D11Buffer *gpID3D11Buffer_ConstantBuffer = NULL;

ID3D11RasterizerState *gpID3D11RasterizerState = NULL;
ID3D11DepthStencilView *gpID3D11DepthStencilView = NULL;

ID3D11ShaderResourceView *gpID3D11ShaderResourceView_Texture = NULL;
ID3D11SamplerState *gpID3D11SamplerState_Texture = NULL;

float gAngleCube = 0.0f;
bool bLight = false;
bool bTexture = false;

float lightAmbient[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
float lightDiffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
float lightSpecular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
float lightPosition[4] = { 100.0f, 100.0f, -100.0f, 1.0f };

float materialAmbient[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
float materialDiffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
float materialSpecular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
float materialShininess = 128.0f;

struct CBUFFER
{
	XMMATRIX WorldMatrix;
	XMMATRIX ViewMatrix;
	XMMATRIX ProjectionMatrix;

	XMVECTOR La;
	XMVECTOR Ld;
	XMVECTOR Ls;
	XMVECTOR Light_Position;

	XMVECTOR Ka;
	XMVECTOR Kd;
	XMVECTOR Ks;
	float Material_Shininess;

	unsigned int LightEnable;
	unsigned int TextureEnable;
};

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
		TEXT("D3D11 | Interleaved"),
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
			update();
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

	case WM_CHAR:
		switch (wParam)
		{
		case 'L':
		case 'l':
			bLight = !bLight;
			break;

		case 'T':
		case 't':
			bTexture = !bTexture;
			break;
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
	HRESULT LoadD3DTexture(const wchar_t *, ID3D11ShaderResourceView **);
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
		"	float4x4 worldMatrix;" \
		"	float4x4 viewMatrix;" \
		"	float4x4 projectionMatrix;" \

		"	float4   la;" \
		"	float4   ld;" \
		"	float4   ls;" \
		"	float4   light_position;" \

		"	float4   ka;" \
		"	float4   kd;" \
		"	float4   ks;" \
		"	float    material_shininess;" \

		"	uint	 lightEnabled;" \
		"	uint	 textureEnabled;" \
		"}" \

		"struct vertex_output" \
		"{" \
		"	float4 position        : SV_POSITION;" \
		"	float4 color           : COLOR;" \
		"	float2 texcoord        : TEXCOORD;" \
		"	float3 tnorm		   : NORMAL0;" \
		"	float3 light_direction : NORMAL1;" \
		"	float3 viewer_vector   : NORMAL2;" \
		"};" \

		"vertex_output main(float4 pos: POSITION, float4 color: COLOR, float4 normal: NORMAL, float2 texcoord: TEXCOORD)" \
		"{" \
		"	vertex_output output;" \
		"	if (lightEnabled == 1)" \
		"	{ " \
		"		float4 eyeCoordinate = mul(worldMatrix, pos);" \
		"		eyeCoordinate = mul(viewMatrix, eyeCoordinate);" \

		"		float3 tnorm = mul((float3x3) worldMatrix, (float3)normal);" \
		"		float3 light_direction = (float3)(light_position - eyeCoordinate);" \
		"		float3 viewer_vector = normalize(-eyeCoordinate.xyz);" \

		"		output.tnorm = tnorm;" \
		"		output.light_direction = light_direction;" \
		"		output.viewer_vector = viewer_vector;" \
		"	} " \

		"	float4 position = mul(worldMatrix, pos);" \
		"	position = mul(viewMatrix, position);" \
		"	position = mul(projectionMatrix, position);" \
		"	output.position = position;" \
		"	output.color = color;" \
		"	output.texcoord = texcoord;" \
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

	//// PIXEL SHADER ///////////////////////////////////////////////////////////////////////
	const char *pixelShaderSourceCode =
		"cbuffer ConstantBuffer" \
		"{" \
		"	float4x4 worldMatrix;" \
		"	float4x4 viewMatrix;" \
		"	float4x4 projectionMatrix;" \

		"	float4   la;" \
		"	float4   ld;" \
		"	float4   ls;" \
		"	float4   light_position;" \

		"	float4   ka;" \
		"	float4   kd;" \
		"	float4   ks;" \
		"	float    material_shininess;" \

		"	uint	 lightEnabled;" \
		"	uint	 textureEnabled;" \
		"}" \

		"struct vertex_output" \
		"{" \
		"	float4 position        : SV_POSITION;" \
		"	float4 color           : COLOR;" \
		"	float2 texcoord        : TEXCOORD;" \
		"	float3 tnorm		   : NORMAL0;" \
		"	float3 light_direction : NORMAL1;" \
		"	float3 viewer_vector   : NORMAL2;" \
		"};" \

		"Texture2D myTexture2D;" \
		"SamplerState mySamplerState;" \

		"float4 main(float4 pos: SV_POSITION, vertex_output input) : SV_TARGET" \
		"{" \
		"	float4 phong_ads_color = float4(1.0,1.0,1.0,1.0);" \
		"	float4 texture_color = float4(1.0,1.0,1.0,1.0);" \

		"	if (lightEnabled == 1)" \
		"	{ " \
		"		float3 ntnorm = normalize(input.tnorm);" \
		"		float3 nlight_direction = normalize(input.light_direction);" \
		"		float3 nviewer_vector = normalize(input.viewer_vector);" \

		"		float3 reflection_vector = reflect(-nlight_direction, ntnorm);" \
		"		float  tn_dot_ld = max(dot(ntnorm, nlight_direction), 0.0);" \

		"		float4 ambient = la * ka;" \
		"		float4 diffuse = ld * kd * tn_dot_ld;" \
		"		float4 specular = ls * ks * pow(max(dot(reflection_vector, nviewer_vector), 0.0), material_shininess);" \
		"		phong_ads_color = ambient + diffuse + specular;" \
		"	} " \

		"	if (textureEnabled == 1)" \
		"	{ " \
		"		texture_color = myTexture2D.Sample(mySamplerState, input.texcoord); " \
		"	} " \

		"	float4 color = input.color * phong_ads_color * texture_color;" \
		"	return(color);" \
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
	D3D11_INPUT_ELEMENT_DESC inputElementsDesc[4];
	inputElementsDesc[0].SemanticName = "POSITION";
	inputElementsDesc[0].SemanticIndex = 0;
	inputElementsDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementsDesc[0].InputSlot = 0;
	inputElementsDesc[0].AlignedByteOffset = 0;
	inputElementsDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputElementsDesc[0].InstanceDataStepRate = 0;

	inputElementsDesc[1].SemanticName = "COLOR";
	inputElementsDesc[1].SemanticIndex = 0;
	inputElementsDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementsDesc[1].InputSlot = 1;
	inputElementsDesc[1].AlignedByteOffset = 0;
	inputElementsDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputElementsDesc[1].InstanceDataStepRate = 0;

	inputElementsDesc[2].SemanticName = "NORMAL";
	inputElementsDesc[2].SemanticIndex = 0;
	inputElementsDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementsDesc[2].InputSlot = 2;
	inputElementsDesc[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	inputElementsDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputElementsDesc[2].InstanceDataStepRate = 0;

	inputElementsDesc[3].SemanticName = "TEXCOORD";
	inputElementsDesc[3].SemanticIndex = 0;
	inputElementsDesc[3].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementsDesc[3].InputSlot = 3;
	inputElementsDesc[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	inputElementsDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputElementsDesc[3].InstanceDataStepRate = 0;

	hr = gpID3D11Device->CreateInputLayout(inputElementsDesc, _ARRAYSIZE(inputElementsDesc),
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

	pID3DBlob_VertexShaderCode->Release();
	pID3DBlob_VertexShaderCode = NULL;

	pID3DBlob_PixelShaderCode->Release();
	pID3DBlob_PixelShaderCode = NULL;

	/////////////////////////////////////////////////////////////////////////////////////

	//// VERTEX DATA ////////////////////////////////////////////////////////////////////

	// Clockwise | Left Hand Rule
	float vertices[] =
	{
		// SIDE1 (top)			 // SIDE1 (top)		   // SIDE1 (top)		  // SIDE1 (top)
		// triangle 1			 // triangle 1		   // triangle 1		  // triangle 1
		-1.0f,  1.0f,  1.0f,	 1.0f, 0.0f, 0.0f,	   0.0f, 1.0f, 0.0f,	  0.0f, 0.0f,
		 1.0f,  1.0f,  1.0f,	 1.0f, 0.0f, 0.0f,	   0.0f, 1.0f, 0.0f,	  0.0f, 1.0f,
		-1.0f,  1.0f, -1.0f,	 1.0f, 0.0f, 0.0f,	   0.0f, 1.0f, 0.0f,	  1.0f, 0.0f,
		// triangle 2			 // triangle 2		   // triangle 2		  // triangle 2
		-1.0f,  1.0f, -1.0f,	 1.0f, 0.0f, 0.0f,	   0.0f, 1.0f, 0.0f,	  1.0f, 0.0f,
		1.0f,  1.0f,  1.0f,	     1.0f, 0.0f, 0.0f,	   0.0f, 1.0f, 0.0f,	  0.0f, 1.0f,
		1.0f,  1.0f, -1.0f,	     1.0f, 0.0f, 0.0f,	   0.0f, 1.0f, 0.0f,	  1.0f, 1.0f,

		// SIDE2 (bottom)		 // SIDE2 (bottom)	   // SIDE2 (bottom)	  // SIDE2 (bottom)
		// triangle 1			 // triangle 1		   // triangle 1		  // triangle 1
		 1.0f, -1.0f, -1.0f,	 0.0f, 1.0f, 0.0f,	   0.0f, -1.0f, 0.0f,	  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f,	 0.0f, 1.0f, 0.0f,	   0.0f, -1.0f, 0.0f,	  0.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,	 0.0f, 1.0f, 0.0f,	   0.0f, -1.0f, 0.0f,	  1.0f, 0.0f,
		// triangle 2			 // triangle 2		   // triangle 2		  // triangle 2
		-1.0f, -1.0f, -1.0f,	 0.0f, 1.0f, 0.0f,	   0.0f, -1.0f, 0.0f,	  1.0f, 0.0f,
		 1.0f, -1.0f,  1.0f,	 0.0f, 1.0f, 0.0f,	   0.0f, -1.0f, 0.0f,	  0.0f, 1.0f,
		-1.0f, -1.0f, +1.0f,	 0.0f, 1.0f, 0.0f,	   0.0f, -1.0f, 0.0f,	  1.0f, 1.0f,

		// SIDE3 (front)		 // SIDE3 (front)	   // SIDE3 (front)	      // SIDE3 (front)
		// triangle 1			 // triangle 1		   // triangle 1		  // triangle 1
		-1.0f,  1.0f, -1.0f,	 0.0f, 0.0f, 1.0f,	   0.0f, 0.0f, -1.0f,	  0.0f, 0.0f,
		 1.0f,  1.0f, -1.0f,	 0.0f, 0.0f, 1.0f,	   0.0f, 0.0f, -1.0f,	  0.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,	 0.0f, 0.0f, 1.0f,	   0.0f, 0.0f, -1.0f,	  1.0f, 0.0f,
		// triangle 2			 // triangle 2		   // triangle 2		  // triangle 2
		-1.0f, -1.0f, -1.0f,	 0.0f, 0.0f, 1.0f,	   0.0f, 0.0f, -1.0f,	  1.0f, 0.0f,
		1.0f,  1.0f, -1.0f,		 0.0f, 0.0f, 1.0f,	   0.0f, 0.0f, -1.0f,	  0.0f, 1.0f,
		1.0f, -1.0f, -1.0f,		 0.0f, 0.0f, 1.0f,	   0.0f, 0.0f, -1.0f,	  1.0f, 1.0f,

		// SIDE4 (back)			 // SIDE4 (back)	   	// SIDE4 (back)		  // SIDE4 (back)
		// triangle 1			 // triangle 1		   // triangle 1		  // triangle 1
		 1.0f, -1.0f,  1.0f,	 0.0f, 1.0f, 1.0f,	   0.0f, 0.0f, 1.0f,	  0.0f, 0.0f,
		 1.0f,  1.0f,  1.0f,	 0.0f, 1.0f, 1.0f,	   0.0f, 0.0f, 1.0f,	  0.0f, 1.0f,
		-1.0f, -1.0f,  1.0f,	 0.0f, 1.0f, 1.0f,	   0.0f, 0.0f, 1.0f,	  1.0f, 0.0f,
		// triangle 2			 // triangle 2		   // triangle 2		  // triangle 2
		-1.0f, -1.0f,  1.0f,	 0.0f, 1.0f, 1.0f,	   0.0f, 0.0f, 1.0f,	  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f,	 0.0f, 1.0f, 1.0f,	   0.0f, 0.0f, 1.0f,	  0.0f, 1.0f,
		-1.0f,  1.0f,  1.0f,	 0.0f, 1.0f, 1.0f,	   0.0f, 0.0f, 1.0f,	  1.0f, 1.0f,

		// SIDE5 (left)			 // SIDE5 (left)	   	// SIDE5 (left)		  // SIDE5 (left)
		// triangle 1			 // triangle 1		   // triangle 1		  // triangle 1
		-1.0f,  1.0f,  1.0f,	 1.0f, 0.0f, 1.0f,	   -1.0f, 0.0f, 0.0f,	  0.0f, 0.0f,
		-1.0f,  1.0f, -1.0f,	 1.0f, 0.0f, 1.0f,	   -1.0f, 0.0f, 0.0f,	  0.0f, 1.0f,
		-1.0f, -1.0f,  1.0f,	 1.0f, 0.0f, 1.0f,	   -1.0f, 0.0f, 0.0f,	  1.0f, 0.0f,
		// triangle 2			 // triangle 2		   // triangle 2		  // triangle 2
		-1.0f, -1.0f,  1.0f,	 1.0f, 0.0f, 1.0f,	   -1.0f, 0.0f, 0.0f,	  1.0f, 0.0f,
		-1.0f,  1.0f, -1.0f,	 1.0f, 0.0f, 1.0f,	   -1.0f, 0.0f, 0.0f,	  0.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,	 1.0f, 0.0f, 1.0f,	   -1.0f, 0.0f, 0.0f,	  1.0f, 1.0f,

		// SIDE6 (right)		 // SIDE6 (right)	   // SIDE6 (right)	      // SIDE6 (right)
		// triangle 1			 // triangle 1		   // triangle 1		  // triangle 1
		1.0f, -1.0f, -1.0f,		 1.0f, 1.0f, 0.0f,	   1.0f, 0.0f, 0.0f,	  0.0f, 0.0f,
		1.0f,  1.0f, -1.0f,		 1.0f, 1.0f, 0.0f,	   1.0f, 0.0f, 0.0f,	  0.0f, 1.0f,
		1.0f, -1.0f,  1.0f,		 1.0f, 1.0f, 0.0f,	   1.0f, 0.0f, 0.0f,	  1.0f, 0.0f,
		// triangle 2			 // triangle 2		   // triangle 2		  // triangle 2
		1.0f, -1.0f,  1.0f,	     1.0f, 1.0f, 0.0f,	   1.0f, 0.0f, 0.0f,	  1.0f, 0.0f,
		1.0f,  1.0f, -1.0f,	     1.0f, 1.0f, 0.0f,	   1.0f, 0.0f, 0.0f,	  0.0f, 1.0f,
		1.0f,  1.0f,  1.0f,	     1.0f, 1.0f, 0.0f,	   1.0f, 0.0f, 0.0f,	  1.0f, 1.0f,
	};

	/////////////////////////////////////////////////////////////////////////////////////

	// create vertex buffer for triangle
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
	gpID3D11DeviceContext->PSSetConstantBuffers(0, 1, &gpID3D11Buffer_ConstantBuffer);

	/////////////////////////////////////////////////////////////////////////////////////

	///// RASTERIZATION STATE //////////////////////////////////////////////////////////////////////

	// In D3D, backface culling is by default 'ON'
	// Means the backface of geometry will not be visible.
	// this causes our 2D triangles backface to vanish on rotation.
	// so set culling 'OFF'
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory((void *)&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	rasterizerDesc.AntialiasedLineEnable = FALSE;
	rasterizerDesc.CullMode = D3D11_CULL_NONE; // this disables culling
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.ScissorEnable = FALSE;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;

	hr = gpID3D11Device->CreateRasterizerState(&rasterizerDesc, &gpID3D11RasterizerState);
	if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateRasterizerState() failed for Constant Buffer.\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateRasterizerState() succeeded for Constant Buffer.\n");
		fclose(gpFile);
	}

	gpID3D11DeviceContext->RSSetState(gpID3D11RasterizerState);

	/////////////////////////////////////////////////////////////////////////////////////

	////// TEXTURE //////////////////////////////////////////////////////////////////////

	// create texture resource and texture view
	hr = LoadD3DTexture(L"marble.bmp",
		&gpID3D11ShaderResourceView_Texture);
	if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "LoadD3DTexture() failed for Constant Buffer.\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "LoadD3DTexture() succeeded for Constant Buffer.\n");
		fclose(gpFile);
	}

	// create sampler state
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	hr = gpID3D11Device->CreateSamplerState(&samplerDesc, &gpID3D11SamplerState_Texture);
	if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateSamplerState() failed for Constant Buffer.\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateSamplerState() succeeded for Constant Buffer.\n");
		fclose(gpFile);
	}

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

HRESULT LoadD3DTexture(const wchar_t *textureFileName, ID3D11ShaderResourceView **ppID3D11ShaderResourceView)
{
	// code
	HRESULT hr;

	// create texture
	hr = DirectX::CreateWICTextureFromFile(gpID3D11Device, textureFileName, NULL, ppID3D11ShaderResourceView);
	if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "DirectX::CreateWICTextureFromFile() failed.\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "DirectX::CreateWICTextureFromFile() succeeded.\n");
		fclose(gpFile);
	}
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

	// create depth stencil buffer ( or zbuffer )
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));
	textureDesc.Width = (UINT)width;
	textureDesc.Height = (UINT)height;
	textureDesc.ArraySize = 1;
	textureDesc.MipLevels = 1;
	textureDesc.SampleDesc.Count = 1; // this can be upto 4
	textureDesc.SampleDesc.Quality = 0; // if above is 4, then it is 1
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
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateDepthStencilView() failed. \n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateDepthStencilView() succeeded. \n");
		fclose(gpFile);
	}

	pID3D11Texture2D_DepthBuffer->Release();
	pID3D11Texture2D_DepthBuffer = NULL;

	// set render target view as render target
	gpID3D11DeviceContext->OMSetRenderTargets(1, &gpID3D11RenderTargetView, gpID3D11DepthStencilView);

	// set viewport
	D3D11_VIEWPORT d3dViewPort;
	d3dViewPort.TopLeftX = 0.0f;
	d3dViewPort.TopLeftY = 0.0f;
	d3dViewPort.Width = (FLOAT)width;
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

	gpID3D11DeviceContext->ClearDepthStencilView(gpID3D11DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	////// TRIANGLE ////////////////////////////////////////////////////////////////////////

	// select which vertex buffer to display
	UINT stride = sizeof(float) * 11;
	UINT offset = sizeof(float) * 0;
	gpID3D11DeviceContext->IASetVertexBuffers(0, 1, &gpID3D11Buffer_VertexBuffer,
		&stride, &offset);

	// colors 
	stride = sizeof(float) * 11;
	offset = sizeof(float) * 3;
	gpID3D11DeviceContext->IASetVertexBuffers(1, 1, &gpID3D11Buffer_VertexBuffer,
		&stride, &offset);

	// normal 
	stride = sizeof(float) * 11;
	offset = sizeof(float) * 6;
	gpID3D11DeviceContext->IASetVertexBuffers(2, 1, &gpID3D11Buffer_VertexBuffer,
		&stride, &offset);

	// texture 
	stride = sizeof(float) * 11;
	offset = sizeof(float) * 9;
	gpID3D11DeviceContext->IASetVertexBuffers(3, 1, &gpID3D11Buffer_VertexBuffer,
		&stride, &offset);

	// bind texture and sampler as pixel shader resource
	gpID3D11DeviceContext->PSSetShaderResources(0, 1, &gpID3D11ShaderResourceView_Texture);
	gpID3D11DeviceContext->PSSetSamplers(0, 1, &gpID3D11SamplerState_Texture);

	// select geometry primitive
	gpID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// translation is concerned with world matrix transformation
	XMMATRIX worldMatrix = XMMatrixIdentity();
	XMMATRIX viewMatrix = XMMatrixIdentity();
	XMMATRIX translationMatrix = XMMatrixIdentity();
	XMMATRIX rotationMatrix = XMMatrixIdentity();

	// translations
	translationMatrix = XMMatrixTranslation(0.0f, 0.0f, 6.0f);
	XMMATRIX r1 = XMMatrixRotationX(XMConvertToRadians(-gAngleCube)); // +ve: right to left | -ve: left to right rotation
	XMMATRIX r2 = XMMatrixRotationY(XMConvertToRadians(-gAngleCube)); // +ve: right to left | -ve: left to right rotation
	XMMATRIX r3 = XMMatrixRotationZ(XMConvertToRadians(-gAngleCube)); // +ve: right to left | -ve: left to right rotation
	rotationMatrix = r1 * r2 * r3;

	XMMATRIX scaleMatrix = XMMatrixScaling(0.75f, 0.75f, 0.75f);

	// THIS MULTIPLICATION ORDER IS IMPORTANT!
	worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;

	// final WorldViewProjection matrix
	XMMATRIX wvpMatrix = worldMatrix * viewMatrix * gPerspectiveProjectionMatrix;

	// load the data into the constant buffer
	CBUFFER constantBuffer;
	ZeroMemory(&constantBuffer, sizeof(CBUFFER));
	constantBuffer.WorldMatrix = worldMatrix;
	constantBuffer.ViewMatrix = viewMatrix;
	constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;
	constantBuffer.TextureEnable = bTexture ? 1 : 0;
	if (bLight == true)
	{
		constantBuffer.LightEnable = 1;

		constantBuffer.La = XMVectorSet(lightAmbient[0], lightAmbient[1], lightAmbient[2], lightAmbient[3]);
		constantBuffer.Ld = XMVectorSet(lightDiffuse[0], lightDiffuse[1], lightDiffuse[2], lightDiffuse[3]);
		constantBuffer.Ls = XMVectorSet(lightSpecular[0], lightSpecular[1], lightSpecular[2], lightSpecular[3]);
		constantBuffer.Light_Position = XMVectorSet(lightPosition[0], lightPosition[1], lightPosition[2], lightPosition[3]);

		constantBuffer.Ka = XMVectorSet(materialAmbient[0], materialAmbient[1], materialAmbient[2], materialAmbient[3]);
		constantBuffer.Kd = XMVectorSet(materialDiffuse[0], materialDiffuse[1], materialDiffuse[2], materialDiffuse[3]);
		constantBuffer.Ks = XMVectorSet(materialSpecular[0], materialSpecular[1], materialSpecular[2], materialSpecular[3]);
		constantBuffer.Material_Shininess = 50.0f;
	}
	else
	{
		constantBuffer.LightEnable = 0;
	}

	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer, 0,
		NULL, &constantBuffer, 0, 0);

	// draw vertex buffer to render target
	gpID3D11DeviceContext->Draw(6, 0);
	gpID3D11DeviceContext->Draw(6, 6);
	gpID3D11DeviceContext->Draw(6, 12);
	gpID3D11DeviceContext->Draw(6, 18);
	gpID3D11DeviceContext->Draw(6, 24);
	gpID3D11DeviceContext->Draw(6, 30);

	///////////////////////////////////////////////////////////////////////////////////////////

	// switch between front and back buffers
	gpIDXGISwapChain->Present(0, 0);
}

void update(void)
{
	gAngleCube += 0.01f;

	if (gAngleCube >= 360.0f)
		gAngleCube = 0.0f;

}

void uninitialize(void)
{
	// code

	if (gpID3D11SamplerState_Texture)
	{
		gpID3D11SamplerState_Texture->Release();
		gpID3D11SamplerState_Texture = NULL;
	}

	if (gpID3D11ShaderResourceView_Texture)
	{
		gpID3D11ShaderResourceView_Texture->Release();
		gpID3D11ShaderResourceView_Texture = NULL;
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
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "uninitalize() succeeded. \n");
		fprintf_s(gpFile, "Log file closed..\n");
		fclose(gpFile);
	}
}
