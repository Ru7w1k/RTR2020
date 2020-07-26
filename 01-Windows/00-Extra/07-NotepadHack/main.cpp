// headers
#include <windows.h>

// global function declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL    CALLBACK WndEnunProc(HWND, LPARAM);

// global variables
HWND  hwndNotepad;
HICON hiconNotepad;
HMENU hmenuNotepad;

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
                        TEXT("Win32 | Notepad Hack"),
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        600,
                        400,
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

    return ((int)msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    // code
    switch (iMsg)
    {
    case WM_CHAR:
        switch (wParam)
        {
            case 'h':
                EnumWindows(WndEnunProc, NULL);
            break;

            case 'i':
                hiconNotepad = LoadIcon(NULL, IDI_APPLICATION);
                SendMessage(hwndNotepad, WM_SETICON, (WPARAM)ICON_SMALL, (LPARAM)hiconNotepad);
            break;

            case 'c':
                SetWindowText(hwndNotepad, "NOT Notepad!");
            break;
        }
        break;

    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_ESCAPE:
            DestroyWindow(hwnd);
            break;
        
        default:
            break;
        }
    break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }
    return (DefWindowProc(hwnd, iMsg, wParam, lParam));
}

BOOL CALLBACK WndEnunProc(HWND hwnd, LPARAM lParam)
{
    char szClassName[256];
    GetClassName(hwnd, szClassName, 256);

    if (strncmp(szClassName, "Notepad", 7) == 0)
    {
        hwndNotepad = hwnd;
        MessageBox(NULL, "Notepad handle found!", "Win32 | Notepad hack", MB_OK | MB_ICONINFORMATION);
        return FALSE;
    }
    return TRUE;
}

