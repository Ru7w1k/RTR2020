// headers
// clang-format off
#include <math.h>
#include <stdio.h>
#include <windows.h>

#include <d3d11.h>
#include <d3dcompiler.h>

// supress type conversion warning
#pragma warning (disable: 4838)
#include "xnamath/xnamath.h"

#include "D3D11Template.h"
// clang-format on

// libraries
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxgi.lib")

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

float anglePyramid = 0.0f;

// log macro
#define LogD(...)                      \
  fopen_s(&gpFile, "RMCLog.txt", "a"); \
  fprintf(gpFile, __VA_ARGS__);        \
  fclose(gpFile);

// D3D variables
IDXGISwapChain *gpIDXGISwapChain = NULL;
ID3D11Device *gpID3D11Device = NULL;
ID3D11DeviceContext *gpID3D11DeviceContext = NULL;
ID3D11RenderTargetView *gpID3D11RenderTargetView = NULL;
ID3D11RasterizerState *gpID3D11RasterizerState = NULL;
ID3D11DepthStencilView *gpID3D11DepthStencilView = NULL;

ID3D11VertexShader *gpID3D11VertexShader = NULL;
ID3D11PixelShader *gpID3D11PixelShader = NULL;
ID3D11Buffer *gpID3D11BufferVertexBufferPosition = NULL;
ID3D11Buffer *gpID3D11BufferConstantBuffer = NULL;
ID3D11InputLayout *gpID3D11InputLayout = NULL;


// ~ uniforms
struct CBUFFER {
  XMMATRIX WorldViewProjectionMatrix;
};

XMMATRIX gPerspectiveProjectionMatrix;

float gClearColor[4];

// WinMain()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpszCmdLine, int iCmdShow) {
  // function declarations
  HRESULT initialize(void);
  void display(void);
  void update(void);
  void uninitialize(void);

  // variable declarations
  bool bDone = false;
  WNDCLASSEX wndclass;
  HWND hwnd;
  MSG msg;
  TCHAR szAppName[] = TEXT("MyApp");
  HRESULT hr;

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
  hwnd = CreateWindowEx(
      WS_EX_APPWINDOW, szAppName, TEXT("D3D11 | BnW Pyramid"),
      WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
      (width / 2) - 400, (height / 2) - 300, WIN_WIDTH, WIN_HEIGHT, NULL, NULL,
      hInstance, NULL);

  ghwnd = hwnd;

  ShowWindow(hwnd, iCmdShow);
  SetForegroundWindow(hwnd);
  SetFocus(hwnd);

  hr = initialize();
  if (FAILED(hr)) {
    LogD("initialize() failed..\n");
    uninitialize();
    DestroyWindow(ghwnd);
    ghwnd = NULL;
  }
  LogD("initialize() successful..\n");

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
  HRESULT resize(int, int);
  void uninitialize();
  void ToggleFullscreen(void);

  HRESULT hr;

  // code
  switch (iMsg) {
    case WM_SETFOCUS:
      gbActiveWindow = true;
      break;

    case WM_KILLFOCUS:
      gbActiveWindow = false;
      break;

    case WM_SIZE:
      if (gpID3D11DeviceContext) {
        hr = resize(LOWORD(lParam), HIWORD(lParam));
        if (FAILED(hr)) {
          LogD("resize() failed..\n");
          return (hr);
        }
        LogD("resize() successful..\n");
      }
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

HRESULT D3DDeviceInfo(void) {
  // local variable declarations
  IDXGIFactory *pIDXGIFactory = NULL;
  IDXGIAdapter *pIDXGIAdapter = NULL;  // ~ physical graphics device

  DXGI_ADAPTER_DESC dxgiAdapterDesc;
  HRESULT hr;
  char str[255];

  // code

  // coInitialize() and coCreateInstance() is internally called by
  // CreateDXGIFactory() method
  hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void **)&pIDXGIFactory);
  if (FAILED(hr)) {
    LogD("CreateDXGIFactory() failed..\n");
    goto CLEANUP;
  }

  // enumerate all compatible adapters
  if (pIDXGIFactory->EnumAdapters(0, &pIDXGIAdapter) == DXGI_ERROR_NOT_FOUND) {
    LogD("Cannot find DXGIAdapter..\n");
    goto CLEANUP;
  }

  // fetch adapter descre
  ZeroMemory((void *)&dxgiAdapterDesc, sizeof(DXGI_ADAPTER_DESC));
  hr = pIDXGIAdapter->GetDesc(&dxgiAdapterDesc);
  if (FAILED(hr)) {
    LogD("GetDesc() failed..\n");
    goto CLEANUP;
  }

  // convert wide char string to ansi string
  WideCharToMultiByte(CP_ACP,  // ANSI CODE PAGE
                      0,
                      dxgiAdapterDesc.Description,  // source string
                      255,                          // size of input string
                      str,                          // destination buffer
                      255,  // size of destination buffer
                      NULL, NULL);

  LogD("GPU      : %s\n", str);
  LogD("GPU VRAM : %d GB\n", (int)ceil(dxgiAdapterDesc.DedicatedVideoMemory /
                                       1024.0 / 1024.0 / 1024.0));

CLEANUP:
  if (pIDXGIAdapter) {
    pIDXGIAdapter->Release();
    pIDXGIAdapter = NULL;
  }

  if (pIDXGIFactory) {
    pIDXGIFactory->Release();
    pIDXGIFactory = NULL;
  }

  return (hr);
}

HRESULT initialize(void) {
  // function declarations
  HRESULT resize(int, int);
  void uninitialize(void);
  HRESULT D3DDeviceInfo(void);
  HRESULT LoadD3DTexture(const wchar_t *, ID3D11ShaderResourceView **);

  // variables
  HRESULT hr;

  D3D_DRIVER_TYPE d3dDriverType;
  D3D_DRIVER_TYPE d3dDriverTypes[] = {D3D_DRIVER_TYPE_HARDWARE,
                                      D3D_DRIVER_TYPE_WARP,
                                      D3D_DRIVER_TYPE_REFERENCE};

  D3D_FEATURE_LEVEL d3dFeatureLevelRequired = D3D_FEATURE_LEVEL_11_0;
  D3D_FEATURE_LEVEL d3dFeatureLevelAcquired = D3D_FEATURE_LEVEL_10_0;

  UINT createDeviceFlags = 0;
  UINT numDriverTypes = 0;
  UINT numFeatureLevels = 1;

  // code
  numDriverTypes = sizeof(d3dDriverTypes) / sizeof(d3dDriverTypes[0]);

  // 0. D3D11 device log
  hr = D3DDeviceInfo();
  if (FAILED(hr)) {
    LogD("D3DDeviceInfo() failed..\n");
    return (hr);
  }

  // 1. create Swap Chain desc
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

  // 2. create Swap chain and device context using swap chain desc
  for (UINT driverType = 0; driverType < numDriverTypes; driverType++) {
    d3dDriverType = d3dDriverTypes[driverType];
    hr = D3D11CreateDeviceAndSwapChain(
        NULL,                      // Adapter
        d3dDriverType,             // Driver Type
        NULL,                      // Software Rasterizer
        createDeviceFlags,         // Flags (Debug, Multithreaded etc)
        &d3dFeatureLevelRequired,  // Required Feature Levels
        numFeatureLevels,          // Number of Feature Levels
        D3D11_SDK_VERSION,         // Direct3D SDK version
        &dxgiSwapChainDesc,        // Swap Chain Desc
        &gpIDXGISwapChain,         // Swap Chain
        &gpID3D11Device,           // Device
        &d3dFeatureLevelAcquired,  // Acquired Feature Level
        &gpID3D11DeviceContext     // Device Context
    );
    if (SUCCEEDED(hr)) break;
  }

  if (FAILED(hr)) {
    LogD("D3D11CreateDeviceAndSwapChain() failed..\n");
    return (hr);
  }

  LogD("D3D11CreateDeviceAndSwapChain() successful..\n");

  LogD("The Chosen Driver is of ");
  if (d3dDriverType == D3D_DRIVER_TYPE_HARDWARE) {
    LogD("Hardware Type. \n");
  } else if (d3dDriverType == D3D_DRIVER_TYPE_WARP) {
    LogD("Warp Type. \n");
  } else if (d3dDriverType == D3D_DRIVER_TYPE_REFERENCE) {
    LogD("Reference Type. \n");
  } else {
    LogD("Unknown Type. \n");
  }

  LogD("The Supported Highest Feature Level is ");
  if (d3dFeatureLevelAcquired == D3D_FEATURE_LEVEL_11_0) {
    LogD("11.0. \n");
  } else if (d3dFeatureLevelAcquired == D3D_FEATURE_LEVEL_10_1) {
    LogD("10.1. \n");
  } else if (d3dFeatureLevelAcquired == D3D_FEATURE_LEVEL_10_0) {
    LogD("10.0. \n");
  } else {
    LogD("Unknown. \n");
  }

  //// initialize requried shaders /////////////////////////////////////////////

  ///// vertex shader
  // clang-format off
  const char *vertexShaderSourceCode = 
    "cbuffer ConstantBuffer \n" \
    "{ \n" \
    "   float4x4 worldViewProjectionMatrix; \n" \
    "} \n" \

    "struct vertexOutput \n" \
    "{ \n" \
    "   float4 position: SV_POSITION; \n" \
    "}; \n" \

    "vertexOutput main(float4 pos: POSITION) \n" \
    "{" \
    "   vertexOutput output; \n" \
    "   output.position = mul(worldViewProjectionMatrix, pos); \n" \
    "   return(output); \n" \
    "} \n";
  // clang-format on

  ID3DBlob *pID3DBlobVertexShaderCode = NULL;
  ID3DBlob *pID3DBlobError = NULL;

  hr = D3DCompile(
      vertexShaderSourceCode,                // shader source code
      lstrlenA(vertexShaderSourceCode) + 1,  // lenght of source code
      "VS",                                  // shader stage to compile for
      NULL,                                  // macro (if required)
      D3D_COMPILE_STANDARD_FILE_INCLUDE,     // include files
      "main",                                // entry-point function name
      "vs_5_0",                              // shader model
      0,  // compiler flags if any for shader | optimization / debug /
      0,  // compiler flags if any for effect | profiling related flags
      &pID3DBlobVertexShaderCode,  // compiled shader code if successful
      &pID3DBlobError              // compilation errors if any
  );

  if (FAILED(hr)) {
    // error in shader source code
    if (pID3DBlobError != NULL) {
      LogD("D3DCompile() failed for Vertex Shader: \n%s.\n",
           (char *)pID3DBlobError->GetBufferPointer());
      pID3DBlobError->Release();
      pID3DBlobError = NULL;
      return (hr);
    }
    // error in COM
    else {
      LogD("D3DCompile() failed..\n");
      return (hr);
    }
  }
  LogD("D3DCompile() succeeded for Vertex Shader.\n");

  hr = gpID3D11Device->CreateVertexShader(
      pID3DBlobVertexShaderCode->GetBufferPointer(),  // source code
      pID3DBlobVertexShaderCode->GetBufferSize(),     // source code length
      NULL,                  // shared variables between multiple shaders
      &gpID3D11VertexShader  // vertex shader object!
  );
  if (FAILED(hr)) {
    LogD("CreateVertexShader() failed..\n");
    return (hr);
  }
  LogD("CreateVertexShader() succeeded..\n");

  gpID3D11DeviceContext->VSSetShader(
      gpID3D11VertexShader,  // vertex shader object
      NULL,                  // class linkage array (if any)
      0                      // class linkage array size (if any)
  );
  if (FAILED(hr)) {
    LogD("VSSetShader() failed..\n");
    return (hr);
  }
  LogD("VSSetShader() succeeded..\n");

  //////////////////////////////////////////////////////////////////////////////

  ///// pixel shader
  // clang-format off
  const char *pixelShaderSourceCode =
    "struct vertexOutput \n" \
    "{ \n" \
    "   float4 position: SV_POSITION; \n" \
    "}; \n" \

    "Texture2D myTexture2D; \n" \
    "SamplerState mySamplerState; \n" \

    "float4 main(vertexOutput input): SV_TARGET \n" \
    "{" \
    "   return(float4(1.0f, 1.0f, 1.0f, 1.0f)); \n" \
    "} \n";
  // clang-format on

  ID3DBlob *pID3DBlobPixelShaderCode = NULL;
  pID3DBlobError = NULL;

  hr = D3DCompile(
      pixelShaderSourceCode,                // shader source code
      lstrlenA(pixelShaderSourceCode) + 1,  // lenght of source code
      "PS",                                 // shader stage to compile for
      NULL,                                 // macro (if required)
      D3D_COMPILE_STANDARD_FILE_INCLUDE,    // include files
      "main",                               // entry-point function name
      "ps_5_0",                             // shader model
      0,  // compiler flags if any for shader | optimization / debug /
      0,  // compiler flags if any for effect | profiling related flags
      &pID3DBlobPixelShaderCode,  // compiled shader code if successful
      &pID3DBlobError             // compilation errors if any
  );

  if (FAILED(hr)) {
    // error in shader source code
    if (pID3DBlobError != NULL) {
      LogD("D3DCompile() failed for Pixel Shader: \n%s.\n",
           (char *)pID3DBlobError->GetBufferPointer());
      pID3DBlobError->Release();
      pID3DBlobError = NULL;
      return (hr);
    }
    // error in COM
    else {
      LogD("D3DCompile() failed..\n");
      return (hr);
    }
  }
  LogD("D3DCompile() succeeded for Pixel Shader.\n");

  hr = gpID3D11Device->CreatePixelShader(
      pID3DBlobPixelShaderCode->GetBufferPointer(),  // source code
      pID3DBlobPixelShaderCode->GetBufferSize(),     // source code length
      NULL,                 // shared variables between multiple shaders
      &gpID3D11PixelShader  // pixel shader object!
  );
  if (FAILED(hr)) {
    LogD("CreatePixelShader() failed..\n");
    return (hr);
  }
  LogD("CreatePixelShader() succeeded..\n");

  gpID3D11DeviceContext->PSSetShader(
      gpID3D11PixelShader,  // pixel shader object
      NULL,                 // class linkage array (if any)
      0                     // class linkage array size (if any)
  );
  if (FAILED(hr)) {
    LogD("PSSetShader() failed..\n");
    return (hr);
  }
  LogD("PSSetShader() succeeded..\n");

  // dont release vertex shader code blob yet, but we can release pixel shader
  if (pID3DBlobPixelShaderCode) {
    pID3DBlobPixelShaderCode->Release();
    pID3DBlobPixelShaderCode = NULL;
  }

  if (pID3DBlobError) {
    pID3DBlobError->Release();
    pID3DBlobError = NULL;
  }

  //////////////////////////////////////////////////////////////////////////////

  //// initialize input layout structure
  D3D11_INPUT_ELEMENT_DESC inputElementDesc[1];

  // position data
  inputElementDesc[0].SemanticName = "POSITION";  // ~ vPosition
  inputElementDesc[0].SemanticIndex = 0;          // multiple data (like mat4)
  inputElementDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;  //
  inputElementDesc[0].AlignedByteOffset = 0;                 // offset
  inputElementDesc[0].InputSlot = 0;  // ~ enum RMC_ATTRIBUTE_POSITION
  inputElementDesc[0].InputSlotClass =
      D3D11_INPUT_PER_VERTEX_DATA;               // pervertex
  inputElementDesc[0].InstanceDataStepRate = 0;  // useful if per instance data

  // create input layout
  hr = gpID3D11Device->CreateInputLayout(
      inputElementDesc,              // input layout arrat
      _ARRAYSIZE(inputElementDesc),  // size of input layout array
      pID3DBlobVertexShaderCode->GetBufferPointer(),  // shader input layout
      pID3DBlobVertexShaderCode->GetBufferSize(),  // size of shader source code
      &gpID3D11InputLayout                         // created input layout!
  );

  if (FAILED(hr)) {
    LogD("CreateInputLayout() failed..\n");
    if (pID3DBlobVertexShaderCode) {
      pID3DBlobVertexShaderCode->Release();
      pID3DBlobVertexShaderCode = NULL;
    }
    return (hr);
  }

  LogD("CreateInputLayout() succeeded..\n");
  if (pID3DBlobVertexShaderCode) {
    pID3DBlobVertexShaderCode->Release();
    pID3DBlobVertexShaderCode = NULL;
  }

  // set input layout in pipeline
  gpID3D11DeviceContext->IASetInputLayout(gpID3D11InputLayout);

  //////////////////////////////////////////////////////////////////////////////

  //// define geometry data
  const float vertices[] = {
      // front
      +0.0f, +1.0f, +0.0f,  //
      +1.0f, -1.0f, -1.0f,  //
      -1.0f, -1.0f, -1.0f,  //

      // right
      +0.0f, +1.0f, +0.0f,  //
      +1.0f, -1.0f, +1.0f,  //
      +1.0f, -1.0f, -1.0f,  //

      // back
      +0.0f, +1.0f, +0.0f,  //
      -1.0f, -1.0f, +1.0f,  //
      +1.0f, -1.0f, +1.0f,  //

      // left
      +0.0f, +1.0f, +0.0f,  //
      -1.0f, -1.0f, -1.0f,  //
      -1.0f, -1.0f, +1.0f,  //
  };

  // create vertex buffer for position
  D3D11_BUFFER_DESC bufferDesc;
  ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
  bufferDesc.ByteWidth = sizeof(float) * _ARRAYSIZE(vertices);
  bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  bufferDesc.Usage = D3D11_USAGE_DYNAMIC;

  hr = gpID3D11Device->CreateBuffer(
      &bufferDesc,  // buffer desc
      NULL,         // subresource data (used in static buffer)
      &gpID3D11BufferVertexBufferPosition  // vertex buffer !
  );
  if (FAILED(hr)) {
    LogD("CreateBuffer() failed..\n");
    return (hr);
  }
  LogD("CreateBuffer() succeeded..\n");

  D3D11_MAPPED_SUBRESOURCE mappedSubresource;
  ZeroMemory(&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
  gpID3D11DeviceContext->Map(
      gpID3D11BufferVertexBufferPosition,  // which buffer to map
      0,                                   // which index to use
      D3D11_MAP_WRITE_DISCARD,             // how to map
      0,                                   // map flags (do not wait)
      &mappedSubresource                   // mapped pointer
  );
  memcpy(mappedSubresource.pData, vertices, sizeof(vertices));
  gpID3D11DeviceContext->Unmap(gpID3D11BufferVertexBufferPosition,
                               0 /* index */);

  // these buffers will be set in pipeline while drawing..

  //////////////////////////////////////////////////////////////////////////////

  //// define and set the constant buffer (~ uniforms)
  D3D11_BUFFER_DESC bufferDescConstBufferDesc;
  ZeroMemory(&bufferDescConstBufferDesc, sizeof(D3D11_BUFFER_DESC));
  bufferDescConstBufferDesc.ByteWidth = sizeof(CBUFFER);
  bufferDescConstBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  bufferDescConstBufferDesc.Usage = D3D11_USAGE_DEFAULT;

  hr = gpID3D11Device->CreateBuffer(
      &bufferDescConstBufferDesc,    // buffer desc
      NULL,                          // subresource data (used in static buffer)
      &gpID3D11BufferConstantBuffer  // constant buffer !
  );
  if (FAILED(hr)) {
    LogD("CreateBuffer() failed for constant buffer..\n");
    return (hr);
  }
  LogD("CreateBuffer() succeeded for constant buffer..\n");

  // set constant buffer in pipeline
  gpID3D11DeviceContext->VSSetConstantBuffers(
      0,                             // slot
      1,                             // number of buffers
      &gpID3D11BufferConstantBuffer  // constant buffer
  );

  //////////////////////////////////////////////////////////////////////////////

  //// create and set rasterizer state
  D3D11_RASTERIZER_DESC rasterizerDesc;
  ZeroMemory((void *)&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
  rasterizerDesc.AntialiasedLineEnable = FALSE;
  rasterizerDesc.CullMode = D3D11_CULL_NONE;  // disable culling
  rasterizerDesc.DepthBias = 0;
  rasterizerDesc.DepthBiasClamp = 0.0f;
  rasterizerDesc.DepthClipEnable = TRUE;
  rasterizerDesc.FillMode = D3D11_FILL_SOLID;  // ~ glPolygonMode()
  rasterizerDesc.FrontCounterClockwise = FALSE;
  rasterizerDesc.MultisampleEnable = FALSE;
  rasterizerDesc.ScissorEnable = FALSE;
  rasterizerDesc.SlopeScaledDepthBias = 0.0f;

  hr = gpID3D11Device->CreateRasterizerState(&rasterizerDesc,
                                             &gpID3D11RasterizerState);
  if (FAILED(hr)) {
    LogD("CreateRasterizerState() failed..\n");
    return (hr);
  }
  LogD("CreateRasterizerState() succeeded..\n");

  gpID3D11DeviceContext->RSSetState(gpID3D11RasterizerState);

  //////////////////////////////////////////////////////////////////////////////

  // set clear color
  gClearColor[0] = 0.0f;
  gClearColor[1] = 0.0f;
  gClearColor[2] = 0.0f;
  gClearColor[3] = 1.0f;

  // set projection matrix
  gPerspectiveProjectionMatrix = XMMatrixIdentity();

  // warm-up resize call
  hr = resize(WIN_WIDTH, WIN_HEIGHT);
  if (FAILED(hr)) {
    LogD("resize() failed..\n");
    return (hr);
  } else {
    LogD("resize() successful..\n");
  }

  return (S_OK);
}

HRESULT resize(int width, int height) {
  // code
  HRESULT hr = S_OK;

  if (height < 0) {
    height = 1;
  }

  // free all size dependent resources
  if (gpID3D11DepthStencilView) {
    gpID3D11DepthStencilView->Release();
    gpID3D11DepthStencilView = NULL;
  }

  if (gpID3D11RenderTargetView) {
    gpID3D11RenderTargetView->Release();
    gpID3D11RenderTargetView = NULL;
  }

  // resize swap chain buffers
  gpIDXGISwapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM,
                                  0);

  // dummy texture buffer for render target
  ID3D11Texture2D *pID3D11Texture2D_BackBuffer = NULL;
  gpIDXGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
                              (LPVOID *)&pID3D11Texture2D_BackBuffer);

  // create render target view using above back buffer
  hr = gpID3D11Device->CreateRenderTargetView(pID3D11Texture2D_BackBuffer, NULL,
                                              &gpID3D11RenderTargetView);
  if (FAILED(hr)) {
    LogD("CreateRenderTargetView() failed..\n");
    return (hr);
  } else {
    LogD("CreateRenderTargetView() successful..\n");
  }

  pID3D11Texture2D_BackBuffer->Release();
  pID3D11Texture2D_BackBuffer = NULL;

  //// D E P T H ///////////////////////////////////////////////////////////////

  D3D11_TEXTURE2D_DESC texture2DDesc;
  ZeroMemory((void *)&texture2DDesc, sizeof(D3D11_TEXTURE2D_DESC));
  texture2DDesc.Width = (UINT)width;
  texture2DDesc.Height = (UINT)height;
  texture2DDesc.Format = DXGI_FORMAT_D32_FLOAT;
  texture2DDesc.Usage = D3D11_USAGE_DEFAULT;
  texture2DDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
  texture2DDesc.SampleDesc.Count = 1;
  texture2DDesc.SampleDesc.Quality = 0;
  texture2DDesc.ArraySize = 1;
  texture2DDesc.MipLevels = 1;
  texture2DDesc.CPUAccessFlags = 0;
  texture2DDesc.MiscFlags = 0;

  ID3D11Texture2D *pID3D11Texture2D_DepthBuffer = NULL;
  hr = gpID3D11Device->CreateTexture2D(&texture2DDesc, NULL,
                                       &pID3D11Texture2D_DepthBuffer);
  if (FAILED(hr)) {
    LogD("CreateTexture2D() failed..\n");
    return (hr);
  } else {
    LogD("CreateTexture2D() successful..\n");
  }

  // create depth stencil view
  D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
  ZeroMemory((void *)&depthStencilViewDesc,
             sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

  depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
  depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
  hr = gpID3D11Device->CreateDepthStencilView(pID3D11Texture2D_DepthBuffer,
                                              &depthStencilViewDesc,
                                              &gpID3D11DepthStencilView);
  if (FAILED(hr)) {
    LogD("CreateDepthStencilView() failed..\n");
    return (hr);
  } else {
    LogD("CreateDepthStencilView() successful..\n");
  }

  pID3D11Texture2D_DepthBuffer->Release();
  pID3D11Texture2D_DepthBuffer = NULL;

  //////////////////////////////////////////////////////////////////////////////

  // set render target view as render output
  gpID3D11DeviceContext->OMSetRenderTargets(1, &gpID3D11RenderTargetView,
                                            gpID3D11DepthStencilView);

  // set viewport
  D3D11_VIEWPORT d3d11Viewport;
  ZeroMemory((void *)&d3d11Viewport, sizeof(D3D11_VIEWPORT));
  d3d11Viewport.TopLeftX = 0;
  d3d11Viewport.TopLeftY = 0;
  d3d11Viewport.Width = (float)width;
  d3d11Viewport.Height = (float)height;
  d3d11Viewport.MinDepth = 0.0f;
  d3d11Viewport.MaxDepth = 1.0f;

  gpID3D11DeviceContext->RSSetViewports(1, &d3d11Viewport);

  gPerspectiveProjectionMatrix = XMMatrixPerspectiveFovLH(
      XMConvertToRadians(45.0f), (float)width / (float)height, 0.1f, 100.0f);

  return (hr);
}

void display(void) {
  // code

  // clear render target view
  gpID3D11DeviceContext->ClearRenderTargetView(gpID3D11RenderTargetView,
                                               gClearColor);
  gpID3D11DeviceContext->ClearDepthStencilView(gpID3D11DepthStencilView,
                                               D3D11_CLEAR_DEPTH, 1.0f, 0);

  // set vertex buffer into pipeline
  // position buffer
  UINT stride = sizeof(float) * 3;
  UINT offset = 0;
  gpID3D11DeviceContext->IASetVertexBuffers(
      0,                                    // input slot index
      1,                                    // number of buffers
      &gpID3D11BufferVertexBufferPosition,  // vertex buffer
      &stride, &offset);

  // set primitive topology
  gpID3D11DeviceContext->IASetPrimitiveTopology(
      D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);  // ~ GL_TRIANGLES

  // transformation matrices
  XMMATRIX translationMatrix = XMMatrixIdentity();
  XMMATRIX rotationMatrix = XMMatrixIdentity();
  XMMATRIX worldMatrix = XMMatrixIdentity();
  XMMATRIX viewMatrix = XMMatrixIdentity();

  translationMatrix = XMMatrixTranslation(0.0f, 0.0f, 5.0f);
  rotationMatrix = XMMatrixRotationY(XMConvertToRadians(-anglePyramid));

  worldMatrix = rotationMatrix * translationMatrix;

  XMMATRIX wvpMatrix = worldMatrix * viewMatrix * gPerspectiveProjectionMatrix;

  // push constant buffer
  CBUFFER constantBuffer;
  constantBuffer.WorldViewProjectionMatrix = wvpMatrix;

  // ~ glUniform(...)
  gpID3D11DeviceContext->UpdateSubresource(
      gpID3D11BufferConstantBuffer,  // constant buffer
      0,                             // index
      NULL,                          // box
      &constantBuffer,               // source data
      0, 0                           // row pitch and depth pitch of box
  );

  // draw primitive
  gpID3D11DeviceContext->Draw(12, 0);

  // Swap Buffers!
  gpIDXGISwapChain->Present(1, 0);
}

void update(void) {
  // code
  anglePyramid += 1.0f;
  if (anglePyramid >= 360.0f) anglePyramid = 0.0f;
}

void uninitialize(void) {
  // code
  if (gpID3D11RasterizerState) {
    gpID3D11RasterizerState->Release();
    gpID3D11RasterizerState = NULL;
  }

  if (gpID3D11BufferConstantBuffer) {
    gpID3D11BufferConstantBuffer->Release();
    gpID3D11BufferConstantBuffer = NULL;
  }

  if (gpID3D11InputLayout) {
    gpID3D11InputLayout->Release();
    gpID3D11InputLayout = NULL;
  }

  if (gpID3D11BufferVertexBufferPosition) {
    gpID3D11BufferVertexBufferPosition->Release();
    gpID3D11BufferVertexBufferPosition = NULL;
  }

  if (gpID3D11PixelShader) {
    gpID3D11PixelShader->Release();
    gpID3D11PixelShader = NULL;
  }

  if (gpID3D11VertexShader) {
    gpID3D11VertexShader->Release();
    gpID3D11VertexShader = NULL;
  }

  if (gpID3D11RenderTargetView) {
    gpID3D11RenderTargetView->Release();
    gpID3D11RenderTargetView = NULL;
  }

  if (gpID3D11DeviceContext) {
    gpID3D11DeviceContext->Release();
    gpID3D11DeviceContext = NULL;
  }

  if (gpID3D11Device) {
    gpID3D11Device->Release();
    gpID3D11Device = NULL;
  }

  if (gpIDXGISwapChain) {
    gpIDXGISwapChain->Release();
    gpIDXGISwapChain = NULL;
  }

  LogD("==== Application Terminated ====\n");
}

