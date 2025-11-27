/**
 * 这个文件是Windows应用程序的入口
 * 除非需要添加新的事件处理逻辑，否则不需要修改这个文件
 */

#include "stdafx.h"

#include "resource.h"
#include "timer.h"

#include "core.h"
#include "mouse.h"
#include "keyboard.h"

#include <atlconv.h>
#include <commctrl.h>
#include <time.h>

#define MAX_LOADSTRING 100

// 全局变量:
static HINSTANCE hInst;                     // 当前实例
static WCHAR szTitle[MAX_LOADSTRING];       // 标题栏文本
static WCHAR szWindowClass[MAX_LOADSTRING]; // 主窗口类名
static HWND hWnd;                           // 主窗口
static HWND hStatus;                        // 状态栏

// 状态栏日志缓冲区
static WCHAR LogMessage[sizeof(statusBarParts) / sizeof(decltype(*statusBarParts))][256];

// 此代码模块中包含的函数的前向声明:
static ATOM MyRegisterClass(HINSTANCE hInstance);
static BOOL InitInstance(HINSTANCE, int);
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);
static BOOL CreateStatusBar(HWND hWnd, WPARAM wParam, LPARAM lParam);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance,
                      _In_ LPWSTR lpCmdLine,
                      _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // 初始化公共控件
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_BAR_CLASSES;
    InitCommonControlsEx(&icex);

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_GAME, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GAME));

    MSG msg;

    // 主消息循环:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GAME));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_GAME);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // 将实例句柄存储在全局变量中

    RECT rc{0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX | WS_CLIPCHILDREN, TRUE);

    hWnd = CreateWindow(
        szWindowClass, szTitle,
        WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX | WS_CLIPCHILDREN,
        WINDOW_X,
        WINDOW_Y,
        rc.right - rc.left,
        rc.bottom - rc.top,
        nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_TIMER    - 处理定时器TimeOut事件
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        // 创建状态栏
        CreateStatusBar(hWnd, wParam, lParam);
        // 游戏初始化
        GameInit(hWnd, wParam, lParam);
        // 游戏主循环开始
#if TIMER_USE == TIMER_WM_TIMER
        SetTimer(hWnd, MAIN_TIMER_ID, 1000 / FPS, NULL);
#elif TIMER_USE == TIMER_TQ_TIMER
        InitTQTimer(hWnd);
#elif TIMER_USE == TIMER_MM_TIMER
        InitMMTimer(hWnd);
#endif
    }
    break;
    case WM_KEYDOWN:
    {
        // 键盘按下事件
        KeyDown(hWnd, wParam, lParam);
    }
    break;
    case WM_KEYUP:
    {
        // 键盘松开事件
        KeyUp(hWnd, wParam, lParam);
    }
    break;
    case WM_MOUSEMOVE:
    {
        // 鼠标移动事件
        MouseMove(hWnd, wParam, lParam);
    }
    break;
    case WM_LBUTTONDOWN:
    {
        // 鼠标左键按下事件
        LButtonDown(hWnd, wParam, lParam);
    }
    break;
    case WM_LBUTTONUP:
    {
        // 鼠标左键松开事件
        LButtonUp(hWnd, wParam, lParam);
    }
    break;
    case WM_TIMER:
    {
        // 游戏主循环逻辑
        if (wParam == MAIN_TIMER_ID)
        {
            GameLoop(hWnd);
        }
    }
    break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // 分析菜单选择:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        // 游戏渲染逻辑
        GameRender(hWnd, wParam, lParam);
    }
    break;
    case WM_DESTROY:
#if TIMER_USE == TIMER_WM_TIMER
        KillTimer(hWnd, MAIN_TIMER_ID);
#elif TIMER_USE == TIMER_TQ_TIMER
        DeleteTQTimer();
#elif TIMER_USE == TIMER_MM_TIMER
        DeleteMMTimer();
#endif
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”框的消息处理程序
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

// 创建状态栏
BOOL CreateStatusBar(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    hStatus = CreateWindowEx(
        0, STATUSCLASSNAME, NULL,
        WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        hWnd, (HMENU)1, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

    if (!hStatus)
    {
        MessageBox(hWnd, TEXT("状态栏创建失败"), TEXT("错误"), MB_OK);
        return FALSE;
    }

    RECT rcStatus;
    GetWindowRect(hStatus, &rcStatus);
    RECT rc{0, 0, WINDOW_WIDTH, WINDOW_HEIGHT + rcStatus.bottom - rcStatus.top};
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX | WS_CLIPCHILDREN, TRUE);
    SetWindowPos(hWnd, nullptr, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOMOVE);
    SendMessage(hStatus, WM_SIZE, 0, 0);

    SendMessage(hStatus, SB_SETPARTS, sizeof(statusBarParts) / sizeof(int), (LPARAM)statusBarParts);

    Log(0, TEXT("状态栏已启用"));

    return TRUE;
}

// 状态栏打印日志
void Log(int pos, const char *format, ...)
{
    va_list args;
    va_start(args, format);

    char message[256];
    vsprintf_s(message, format, args);

    va_end(args);

    MultiByteToWideChar(CP_ACP, 0, message, -1, LogMessage[pos], 256);

    PostMessage(hStatus, SB_SETTEXT, pos, (LPARAM)LogMessage[pos]);
}

// 状态栏打印日志
void Log(int pos, const TCHAR *format, ...)
{
    va_list args;
    va_start(args, format);

    wsprintf(LogMessage[pos], format, args);

    va_end(args);

    PostMessage(hStatus, SB_SETTEXT, pos, (LPARAM)LogMessage[pos]);
}
