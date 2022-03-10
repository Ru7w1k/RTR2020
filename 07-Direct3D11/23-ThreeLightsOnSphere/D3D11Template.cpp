/*
 * Direct3D - Three lights on Sphere
 */

#include <Windows.h>
#include <stdio.h>  // for file I/O

#include <d3d11.h>
#include <d3dcompiler.h> // for shader compilation

#pragma warning(disable:4838)
#include "XNAMath\xnamath.h"

#include "Sphere.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "Sphere.lib")

#define WIN_WIDTH  800
#define WIN_HEIGHT 600

// global function declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// types
typedef struct _Light {
	float lightAmbient[4];
	float lightDiffuse[4];
	float lightSpecular[4];
	float lightPosition[4];
} Light;

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

ID3D11VertexShader *gpID3D11VertexShader_PerVertex = NULL;
ID3D11PixelShader *gpID3D11PixelShader_PerVertex = NULL;

ID3D11VertexShader *gpID3D11VertexShader_PerPixel = NULL;
ID3D11PixelShader *gpID3D11PixelShader_PerPixel = NULL;

ID3D11Buffer *gpID3D11Buffer_VertexBuffer_Position = NULL;
ID3D11Buffer *gpID3D11Buffer_VertexBuffer_Normal = NULL;
ID3D11Buffer *gpID3D11Buffer_IndexBuffer = NULL;

ID3D11InputLayout *gpID3D11InputLayout = NULL;
ID3D11Buffer *gpID3D11Buffer_ConstantBuffer = NULL;

ID3D11RasterizerState *gpID3D11RasterizerState = NULL;
ID3D11DepthStencilView *gpID3D11DepthStencilView = NULL;

float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];
unsigned int gNumElements;
unsigned int gNumVertices;

bool gbPerPixel = false;

struct CBUFFER
{
	XMMATRIX WorldMatrix;
	XMMATRIX ViewMatrix;
	XMMATRIX ProjectionMatrix;

	XMVECTOR La_Red;
	XMVECTOR Ld_Red;
	XMVECTOR Ls_Red;
	XMVECTOR Light_Position_Red;

	XMVECTOR La_Green;
	XMVECTOR Ld_Green;
	XMVECTOR Ls_Green;
	XMVECTOR Light_Position_Green;

	XMVECTOR La_Blue;
	XMVECTOR Ld_Blue;
	XMVECTOR Ls_Blue;
	XMVECTOR Light_Position_Blue;
	
	XMVECTOR Ka;
	XMVECTOR Kd;
	XMVECTOR Ks;
	float Material_Shininess;
	
	unsigned int KeyPressed;
};

bool gbLight = false;

Light lights[3];

float materialAmbient[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
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
		TEXT("D3D11 | Three Lights on Sphere"),
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
			gbLight = !gbLight;
			break;

		case 'P':
		case 'p':
			gbPerPixel = true;
			break;

		case 'V':
		case 'v':
			gbPerPixel = false;
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

	//// VERTEX SHADER: PER VERTEX /////////////////////////////////////////////////////////////
	const char *vertexShaderSourceCode_PerVertex =
		"cbuffer ConstantBuffer" \
		"{" \
		"	float4x4 worldMatrix;" \
		"	float4x4 viewMatrix;" \
		"	float4x4 projectionMatrix;" \

		"	float4   la_red;" \
		"	float4   ld_red;" \
		"	float4   ls_red;" \
		"	float4   light_position_red;" \

		"	float4   la_green;" \
		"	float4   ld_green;" \
		"	float4   ls_green;" \
		"	float4   light_position_green;" \

		"	float4   la_blue;" \
		"	float4   ld_blue;" \
		"	float4   ls_blue;" \
		"	float4   light_position_blue;" \

		"	float4   ka;" \
		"	float4   kd;" \
		"	float4   ks;" \
		"	float    material_shininess;" \

		"	uint	 keyPressed;" \
		"}" \

		"struct vertex_output" \
		"{" \
		"	float4 position        : SV_POSITION;" \
		"	float4 phong_ads_color : COLOR;" \
		"};" \

		"vertex_output main(float4 pos: POSITION, float4 normal: NORMAL)" \
		"{" \
		"	vertex_output output;" \

		"	if (keyPressed == 1)" \
		"	{ " \
		"		float4 eyeCoordinate = mul(worldMatrix, pos);" \
		"		eyeCoordinate = mul(viewMatrix, eyeCoordinate);" \
		"		float3 tnorm = normalize(mul((float3x3) worldMatrix, (float3)normal));" \
		"		float3 viewer_vector = normalize(-eyeCoordinate.xyz);" \

		"		float3 light_direction = (float3)normalize(light_position_red - eyeCoordinate);" \
		"		float tn_dot_ld = max(dot(tnorm, light_direction), 0.0);" \
		"		float3 reflection_vector = reflect(-light_direction, tnorm);" \

		"		float4 ambient = la_red * ka;" \
		"		float4 diffuse = ld_red * kd * tn_dot_ld;" \
		"		float4 specular = ls_red * ks * pow(max(dot(reflection_vector, viewer_vector), 0.0), material_shininess);" \

		"		output.phong_ads_color = ambient + diffuse + specular;" \

		"		light_direction = (float3)normalize(light_position_green - eyeCoordinate);" \
		"		tn_dot_ld = max(dot(tnorm, light_direction), 0.0);" \
		"		reflection_vector = reflect(-light_direction, tnorm);" \

		"		ambient = la_green * ka;" \
		"		diffuse = ld_green * kd * tn_dot_ld;" \
		"		specular = ls_green * ks * pow(max(dot(reflection_vector, viewer_vector), 0.0), material_shininess);" \

		"		output.phong_ads_color += ambient + diffuse + specular;" \

		"		light_direction = (float3)normalize(light_position_blue - eyeCoordinate);" \
		"		tn_dot_ld = max(dot(tnorm, light_direction), 0.0);" \
		"		reflection_vector = reflect(-light_direction, tnorm);" \

		"		ambient = la_blue * ka;" \
		"		diffuse = ld_blue * kd * tn_dot_ld;" \
		"		specular = ls_blue * ks * pow(max(dot(reflection_vector, viewer_vector), 0.0), material_shininess);" \

		"		output.phong_ads_color += ambient + diffuse + specular;" \
		"	} " \

		"	else " \
		"	{ " \
		"		output.phong_ads_color = float4(1.0, 1.0, 1.0, 1.0);" \
		"	} " \

		"	float4 position = mul(worldMatrix, pos);" \
		"	position = mul(viewMatrix, position);" \
		"	position = mul(projectionMatrix, position);" \
		"	output.position = position;" \
		"	return(output);" \
		"}";

	ID3DBlob *pID3DBlob_VertexShaderCode = NULL;
	ID3DBlob *pID3DBlob_Error = NULL;

	hr = D3DCompile(vertexShaderSourceCode_PerVertex,		// source code
		lstrlenA(vertexShaderSourceCode_PerVertex) + 1,		// source code length
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
		pID3DBlob_VertexShaderCode->GetBufferSize(), NULL, &gpID3D11VertexShader_PerVertex);
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
	
	//////////////////////////////////////////////////////////////////////////////////////////

	//// PIXEL SHADER: PER VERTEX /////////////////////////////////////////////////////////////
	const char *pixelShaderSourceCode_PerVertex =
		"float4 main(float4 pos: SV_POSITION, float4 phong_ads_color: COLOR) : SV_TARGET" \
		"{" \
		"	float4 color = phong_ads_color;" \
		"	return(color);" \
		"}";

	ID3DBlob *pID3DBlob_PixelShaderCode = NULL;
	pID3DBlob_Error = NULL;

	hr = D3DCompile(pixelShaderSourceCode_PerVertex,		// source code
		lstrlenA(pixelShaderSourceCode_PerVertex) + 1,		// source code length
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
		pID3DBlob_PixelShaderCode->GetBufferSize(), NULL, &gpID3D11PixelShader_PerVertex);
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

	////////////////////////////////////////////////////////////////////////////////////

	//// VERTEX SHADER: PER PIXEL ///////////////////////////////////////////////////////////////
	const char *vertexShaderSourceCode_PerPixel =
		"cbuffer ConstantBuffer" \
		"{" \
		"	float4x4 worldMatrix;" \
		"	float4x4 viewMatrix;" \
		"	float4x4 projectionMatrix;" \

		"	float4   la_red;" \
		"	float4   ld_red;" \
		"	float4   ls_red;" \
		"	float4   light_position_red;" \

		"	float4   la_green;" \
		"	float4   ld_green;" \
		"	float4   ls_green;" \
		"	float4   light_position_green;" \

		"	float4   la_blue;" \
		"	float4   ld_blue;" \
		"	float4   ls_blue;" \
		"	float4   light_position_blue;" \

		"	float4   ka;" \
		"	float4   kd;" \
		"	float4   ks;" \
		"	float    material_shininess;" \

		"	uint	 keyPressed;" \
		"}" \

		"struct vertex_output" \
		"{" \
		"	float4 position		         : SV_POSITION;" \
		"	float3 tnorm		         : NORMAL0;" \
		"	float3 viewer_vector         : NORMAL1;" \
		"	float3 light_direction_red   : NORMAL2;" \
		"	float3 light_direction_green : NORMAL3;" \
		"	float3 light_direction_blue  : NORMAL4;" \
		"};" \

		"vertex_output main(float4 pos: POSITION, float4 normal: NORMAL)" \
		"{" \
		"	vertex_output output;" \

		"	if (keyPressed == 1)" \
		"	{ " \
		"		float4 eyeCoordinate = mul(worldMatrix, pos);" \
		"		eyeCoordinate = mul(viewMatrix, eyeCoordinate);" \

		"		float3 tnorm = mul((float3x3) worldMatrix, (float3)normal);" \
		"		float3 viewer_vector = normalize(-eyeCoordinate.xyz);" \

		"		float3 light_direction_red = (float3)(light_position_red - eyeCoordinate);" \
		"		float3 light_direction_green = (float3)(light_position_green - eyeCoordinate);" \
		"		float3 light_direction_blue = (float3)(light_position_blue - eyeCoordinate);" \

		"		output.tnorm = tnorm;" \
		"		output.viewer_vector = viewer_vector;" \
		"		output.light_direction_red = light_direction_red;" \
		"		output.light_direction_green = light_direction_green;" \
		"		output.light_direction_blue = light_direction_blue;" \
		"	} " \

		"	float4 position = mul(worldMatrix, pos);" \
		"	position = mul(viewMatrix, position);" \
		"	position = mul(projectionMatrix, position);" \
		"	output.position = position;" \
		"	return(output);" \
		"}";

	pID3DBlob_VertexShaderCode = NULL;
	pID3DBlob_Error = NULL;

	hr = D3DCompile(vertexShaderSourceCode_PerPixel,			// source code
		lstrlenA(vertexShaderSourceCode_PerPixel) + 1,		// source code length
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
		pID3DBlob_VertexShaderCode->GetBufferSize(), NULL, &gpID3D11VertexShader_PerPixel);
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

	//////////////////////////////////////////////////////////////////////////////////////////

	//// PIXEL SHADER: PER PIXEL /////////////////////////////////////////////////////////////
	const char *pixelShaderSourceCode_PerPixel =
		"cbuffer ConstantBuffer" \
		"{" \
		"	float4x4 worldMatrix;" \
		"	float4x4 viewMatrix;" \
		"	float4x4 projectionMatrix;" \

		"	float4   la_red;" \
		"	float4   ld_red;" \
		"	float4   ls_red;" \
		"	float4   light_position_red;" \

		"	float4   la_green;" \
		"	float4   ld_green;" \
		"	float4   ls_green;" \
		"	float4   light_position_green;" \

		"	float4   la_blue;" \
		"	float4   ld_blue;" \
		"	float4   ls_blue;" \
		"	float4   light_position_blue;" \

		"	float4   ka;" \
		"	float4   kd;" \
		"	float4   ks;" \
		"	float    material_shininess;" \

		"	uint	 keyPressed;" \
		"}" \

		"struct vertex_output" \
		"{" \
		"	float4 position		         : SV_POSITION;" \
		"	float3 tnorm		         : NORMAL0;" \
		"	float3 viewer_vector         : NORMAL1;" \
		"	float3 light_direction_red   : NORMAL2;" \
		"	float3 light_direction_green : NORMAL3;" \
		"	float3 light_direction_blue  : NORMAL4;" \
		"};" \

		"float4 main(float4 pos: SV_POSITION, vertex_output input) : SV_TARGET" \
		"{" \
		"	float4 phong_ads_color = float4(1.0,1.0,1.0,1.0);" \

		"	if (keyPressed == 1)" \
		"	{ " \
		"		float3 ntnorm = normalize(input.tnorm);" \
		"		float3 nviewer_vector = normalize(input.viewer_vector);" \

		"		float3 nlight_direction = normalize(input.light_direction_red);" \
		"		float3 reflection_vector = reflect(-nlight_direction, ntnorm);" \
		"		float  tn_dot_ld = max(dot(ntnorm, nlight_direction), 0.0);" \

		"		float4 ambient = la_red * ka;" \
		"		float4 diffuse = ld_red * kd * tn_dot_ld;" \
		"		float4 specular = ls_red * ks * pow(max(dot(reflection_vector, nviewer_vector), 0.0), material_shininess);" \

		"		phong_ads_color = ambient + diffuse + specular;" \

		"		nlight_direction = normalize(input.light_direction_green);" \
		"		reflection_vector = reflect(-nlight_direction, ntnorm);" \
		"		tn_dot_ld = max(dot(ntnorm, nlight_direction), 0.0);" \

		"		ambient = la_green * ka;" \
		"		diffuse = ld_green * kd * tn_dot_ld;" \
		"		specular = ls_green * ks * pow(max(dot(reflection_vector, nviewer_vector), 0.0), material_shininess);" \

		"		phong_ads_color += ambient + diffuse + specular;" \

		"		nlight_direction = normalize(input.light_direction_blue);" \
		"		reflection_vector = reflect(-nlight_direction, ntnorm);" \
		"		tn_dot_ld = max(dot(ntnorm, nlight_direction), 0.0);" \

		"		ambient = la_blue * ka;" \
		"		diffuse = ld_blue * kd * tn_dot_ld;" \
		"		specular = ls_blue * ks * pow(max(dot(reflection_vector, nviewer_vector), 0.0), material_shininess);" \

		"		phong_ads_color += ambient + diffuse + specular;" \
		"	} " \

		"	float4 color = phong_ads_color;" \
		"	return(color);" \
		"}";

	pID3DBlob_PixelShaderCode = NULL;
	pID3DBlob_Error = NULL;

	hr = D3DCompile(pixelShaderSourceCode_PerPixel,			// source code
		lstrlenA(pixelShaderSourceCode_PerPixel) + 1,		// source code length
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
		pID3DBlob_PixelShaderCode->GetBufferSize(), NULL, &gpID3D11PixelShader_PerPixel);
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
	
	////////////////////////////////////////////////////////////////////////////////////

	// create and set input layout
	D3D11_INPUT_ELEMENT_DESC inputElementsDesc[2];
	inputElementsDesc[0].SemanticName = "POSITION";
	inputElementsDesc[0].SemanticIndex = 0;
	inputElementsDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementsDesc[0].InputSlot = 0;
	inputElementsDesc[0].AlignedByteOffset = 0;
	inputElementsDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputElementsDesc[0].InstanceDataStepRate = 0;

	inputElementsDesc[1].SemanticName = "NORMAL";
	inputElementsDesc[1].SemanticIndex = 0;
	inputElementsDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementsDesc[1].InputSlot = 1;
	inputElementsDesc[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	inputElementsDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputElementsDesc[1].InstanceDataStepRate = 0;

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

	getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
	gNumVertices = getNumberOfSphereVertices();
	gNumElements = getNumberOfSphereElements();

	/////////////////////////////////////////////////////////////////////////////////////

	// create vertex buffer for position
	D3D11_BUFFER_DESC bufferDesc_VertexBuffer_Position;
	ZeroMemory(&bufferDesc_VertexBuffer_Position, sizeof(D3D11_BUFFER_DESC));
	bufferDesc_VertexBuffer_Position.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc_VertexBuffer_Position.ByteWidth = sizeof(float) * ARRAYSIZE(sphere_vertices);
	bufferDesc_VertexBuffer_Position.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc_VertexBuffer_Position.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;

	hr = gpID3D11Device->CreateBuffer(&bufferDesc_VertexBuffer_Position, NULL, &gpID3D11Buffer_VertexBuffer_Position);
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
	gpID3D11DeviceContext->Map(gpID3D11Buffer_VertexBuffer_Position, 0,
		D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	memcpy(mappedSubresource.pData, sphere_vertices, sizeof(sphere_vertices));
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_VertexBuffer_Position, NULL);

	/////////////////////////////////////////////////////////////////////////////////////

	// create vertex buffer for normal
	D3D11_BUFFER_DESC bufferDesc_VertexBuffer_Normal;
	ZeroMemory(&bufferDesc_VertexBuffer_Normal, sizeof(D3D11_BUFFER_DESC));
	bufferDesc_VertexBuffer_Normal.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc_VertexBuffer_Normal.ByteWidth = sizeof(float) * ARRAYSIZE(sphere_normals);
	bufferDesc_VertexBuffer_Normal.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc_VertexBuffer_Normal.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;

	hr = gpID3D11Device->CreateBuffer(&bufferDesc_VertexBuffer_Normal, NULL, &gpID3D11Buffer_VertexBuffer_Normal);
	if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateBuffer() failed for Normal Buffer.\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateBuffer() succeeded for Normal Buffer.\n");
		fclose(gpFile);
	}

	// copy vertices into above buffer
	ZeroMemory(&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext->Map(gpID3D11Buffer_VertexBuffer_Normal, 0,
		D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	memcpy(mappedSubresource.pData, sphere_normals, sizeof(sphere_normals));
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_VertexBuffer_Normal, NULL);

	/////////////////////////////////////////////////////////////////////////////////////

	// create index buffer 
	D3D11_BUFFER_DESC bufferDesc_IndexBuffer;
	ZeroMemory(&bufferDesc_IndexBuffer, sizeof(D3D11_BUFFER_DESC));
	bufferDesc_IndexBuffer.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc_IndexBuffer.ByteWidth = gNumElements * sizeof(short);
	bufferDesc_IndexBuffer.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc_IndexBuffer.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;

	hr = gpID3D11Device->CreateBuffer(&bufferDesc_IndexBuffer, NULL, &gpID3D11Buffer_IndexBuffer);
	if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateBuffer() failed for Index Buffer.\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateBuffer() succeeded for Index Buffer.\n");
		fclose(gpFile);
	}

	// copy vertices into above buffer
	ZeroMemory(&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext->Map(gpID3D11Buffer_IndexBuffer, 0,
		D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	memcpy(mappedSubresource.pData, sphere_elements, gNumElements * sizeof(short));
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_IndexBuffer, NULL);

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

	// d3d clear color (blue)
	gClearColor[0] = 0.0f;
	gClearColor[1] = 0.0f;
	gClearColor[2] = 0.0f;
	gClearColor[3] = 1.0f;

	// set projection matrix
	gPerspectiveProjectionMatrix = XMMatrixIdentity();

	// RED light
	lights[0].lightAmbient[0] = 0.0f;
	lights[0].lightAmbient[1] = 0.0f;
	lights[0].lightAmbient[2] = 0.0f;
	lights[0].lightAmbient[3] = 1.0f;

	lights[0].lightDiffuse[0] = 1.0f;
	lights[0].lightDiffuse[1] = 0.0f;
	lights[0].lightDiffuse[2] = 0.0f;
	lights[0].lightDiffuse[3] = 1.0f;

	lights[0].lightSpecular[0] = 1.0f;
	lights[0].lightSpecular[1] = 0.0f;
	lights[0].lightSpecular[2] = 0.0f;
	lights[0].lightSpecular[3] = 1.0f;

	lights[0].lightPosition[0] = 0.0f;
	lights[0].lightPosition[1] = 0.0f;
	lights[0].lightPosition[2] = 0.0f;
	lights[0].lightPosition[3] = 1.0f;

	// GREEN light
	lights[1].lightAmbient[0] = 0.0f;
	lights[1].lightAmbient[1] = 0.0f;
	lights[1].lightAmbient[2] = 0.0f;
	lights[1].lightAmbient[3] = 1.0f;

	lights[1].lightDiffuse[0] = 0.0f;
	lights[1].lightDiffuse[1] = 1.0f;
	lights[1].lightDiffuse[2] = 0.0f;
	lights[1].lightDiffuse[3] = 1.0f;

	lights[1].lightSpecular[0] = 0.0f;
	lights[1].lightSpecular[1] = 1.0f;
	lights[1].lightSpecular[2] = 0.0f;
	lights[1].lightSpecular[3] = 1.0f;

	lights[1].lightPosition[0] = 0.0f;
	lights[1].lightPosition[1] = 0.0f;
	lights[1].lightPosition[2] = 0.0f;
	lights[1].lightPosition[3] = 1.0f;

	// BLUE light
	lights[2].lightAmbient[0] = 0.0f;
	lights[2].lightAmbient[1] = 0.0f;
	lights[2].lightAmbient[2] = 0.0f;
	lights[2].lightAmbient[3] = 1.0f;

	lights[2].lightDiffuse[0] = 0.0f;
	lights[2].lightDiffuse[1] = 0.0f;
	lights[2].lightDiffuse[2] = 1.0f;
	lights[2].lightDiffuse[3] = 1.0f;

	lights[2].lightSpecular[0] = 0.0f;
	lights[2].lightSpecular[1] = 0.0f;
	lights[2].lightSpecular[2] = 1.0f;
	lights[2].lightSpecular[3] = 1.0f;

	lights[2].lightPosition[0] = 0.0f;
	lights[2].lightPosition[1] = 0.0f;
	lights[2].lightPosition[2] = 0.0f;
	lights[2].lightPosition[3] = 1.0f;

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

	////// SPHERE ////////////////////////////////////////////////////////////////////////

	// select which vertex buffer to display
	UINT stride = sizeof(float) * 3;
	UINT offset = 0;
	gpID3D11DeviceContext->IASetVertexBuffers(0, 1, &gpID3D11Buffer_VertexBuffer_Position,
		&stride, &offset);

	// normals
	stride = sizeof(float) * 3;
	offset = 0;
	gpID3D11DeviceContext->IASetVertexBuffers(1, 1, &gpID3D11Buffer_VertexBuffer_Normal,
		&stride, &offset);

	// index buffer
	gpID3D11DeviceContext->IASetIndexBuffer(gpID3D11Buffer_IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// select geometry primitive
	gpID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// translation is concerned with world matrix transformation
	XMMATRIX worldMatrix = XMMatrixIdentity();
	XMMATRIX viewMatrix = XMMatrixIdentity();
	XMMATRIX translationMatrix = XMMatrixIdentity();

	// translations
	translationMatrix = XMMatrixTranslation(0.0f, 0.0f, 3.0f);

	// THIS MULTIPLICATION ORDER IS IMPORTANT!
	worldMatrix = translationMatrix;

	// load the data into the constant buffer
	CBUFFER constantBuffer;
	ZeroMemory(&constantBuffer, sizeof(CBUFFER));
	constantBuffer.WorldMatrix = worldMatrix;
	constantBuffer.ViewMatrix = viewMatrix;
	constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;
	if (gbLight == true)
	{
		constantBuffer.KeyPressed = 1;

		constantBuffer.La_Red = XMVectorSet(lights[0].lightAmbient[0], lights[0].lightAmbient[1], lights[0].lightAmbient[2], lights[0].lightAmbient[3]);
		constantBuffer.Ld_Red = XMVectorSet(lights[0].lightDiffuse[0], lights[0].lightDiffuse[1], lights[0].lightDiffuse[2], lights[0].lightDiffuse[3]);
		constantBuffer.Ls_Red = XMVectorSet(lights[0].lightSpecular[0], lights[0].lightSpecular[1], lights[0].lightSpecular[2], lights[0].lightSpecular[3]);
		constantBuffer.Light_Position_Red = XMVectorSet(lights[0].lightPosition[0], lights[0].lightPosition[1], lights[0].lightPosition[2], lights[0].lightPosition[3]);

		constantBuffer.La_Green = XMVectorSet(lights[1].lightAmbient[0], lights[1].lightAmbient[1], lights[1].lightAmbient[2], lights[1].lightAmbient[3]);
		constantBuffer.Ld_Green = XMVectorSet(lights[1].lightDiffuse[0], lights[1].lightDiffuse[1], lights[1].lightDiffuse[2], lights[1].lightDiffuse[3]);
		constantBuffer.Ls_Green = XMVectorSet(lights[1].lightSpecular[0], lights[1].lightSpecular[1], lights[1].lightSpecular[2], lights[1].lightSpecular[3]);
		constantBuffer.Light_Position_Green = XMVectorSet(lights[1].lightPosition[0], lights[1].lightPosition[1], lights[1].lightPosition[2], lights[1].lightPosition[3]);

		constantBuffer.La_Blue = XMVectorSet(lights[2].lightAmbient[0], lights[2].lightAmbient[1], lights[2].lightAmbient[2], lights[2].lightAmbient[3]);
		constantBuffer.Ld_Blue = XMVectorSet(lights[2].lightDiffuse[0], lights[2].lightDiffuse[1], lights[2].lightDiffuse[2], lights[2].lightDiffuse[3]);
		constantBuffer.Ls_Blue = XMVectorSet(lights[2].lightSpecular[0], lights[2].lightSpecular[1], lights[2].lightSpecular[2], lights[2].lightSpecular[3]);
		constantBuffer.Light_Position_Blue = XMVectorSet(lights[2].lightPosition[0], lights[2].lightPosition[1], lights[2].lightPosition[2], lights[2].lightPosition[3]);

		constantBuffer.Ka = XMVectorSet(materialAmbient[0], materialAmbient[1], materialAmbient[2], materialAmbient[3]);
		constantBuffer.Kd = XMVectorSet(materialDiffuse[0], materialDiffuse[1], materialDiffuse[2], materialDiffuse[3]);
		constantBuffer.Ks = XMVectorSet(materialSpecular[0], materialSpecular[1], materialSpecular[2], materialSpecular[3]);
		constantBuffer.Material_Shininess = 128.0f;
	}
	else
	{
		constantBuffer.KeyPressed = 0;
	}

	// set appropriate shaders in pipeline 
	if (gbPerPixel)
	{
		gpID3D11DeviceContext->VSSetShader(gpID3D11VertexShader_PerPixel, NULL, NULL);
		gpID3D11DeviceContext->PSSetShader(gpID3D11PixelShader_PerPixel, NULL, NULL);

		gpID3D11DeviceContext->VSSetConstantBuffers(0, 1, &gpID3D11Buffer_ConstantBuffer);
		gpID3D11DeviceContext->PSSetConstantBuffers(0, 1, &gpID3D11Buffer_ConstantBuffer);

	}
	else
	{
		gpID3D11DeviceContext->VSSetShader(gpID3D11VertexShader_PerVertex, NULL, NULL);
		gpID3D11DeviceContext->PSSetShader(gpID3D11PixelShader_PerVertex, NULL, NULL);

		gpID3D11DeviceContext->VSSetConstantBuffers(0, 1, &gpID3D11Buffer_ConstantBuffer);
	}

	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer, 0,
		NULL, &constantBuffer, 0, 0);

	// draw vertex buffer to render target
	gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);

	///////////////////////////////////////////////////////////////////////////////////////////

	// switch between front and back buffers
	gpIDXGISwapChain->Present(0, 0);
}

void update(void)
{
	static float angle = 0.0f;
	if (angle < 360.0f) angle += 0.00025f;
	else angle = 0.0f;

	lights[0].lightPosition[0] = 0.0f;
	lights[0].lightPosition[1] = 100.0f*cos(angle);
	lights[0].lightPosition[2] = 100.0f*sin(angle);

	lights[1].lightPosition[0] = 100.0f*cos(angle);
	lights[1].lightPosition[1] = 0.0f;
	lights[1].lightPosition[2] = 100.0f*sin(angle);

	lights[2].lightPosition[0] = 100.0f*cos(angle);
	lights[2].lightPosition[1] = 100.0f*sin(angle);
	lights[2].lightPosition[2] = 0.0f;
}

void uninitialize(void)
{
	// code

	if (gpID3D11Buffer_IndexBuffer)
	{
		gpID3D11Buffer_IndexBuffer->Release();
		gpID3D11Buffer_IndexBuffer = NULL;
	}

	if (gpID3D11Buffer_VertexBuffer_Position)
	{
		gpID3D11Buffer_VertexBuffer_Position->Release();
		gpID3D11Buffer_VertexBuffer_Position = NULL;
	}

	if (gpID3D11Buffer_VertexBuffer_Normal)
	{
		gpID3D11Buffer_VertexBuffer_Normal->Release();
		gpID3D11Buffer_VertexBuffer_Normal = NULL;
	}

	if (gpID3D11PixelShader_PerPixel)
	{
		gpID3D11PixelShader_PerPixel->Release();
		gpID3D11PixelShader_PerPixel = NULL;
	}

	if (gpID3D11VertexShader_PerPixel)
	{
		gpID3D11VertexShader_PerPixel->Release();
		gpID3D11VertexShader_PerPixel = NULL;
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
