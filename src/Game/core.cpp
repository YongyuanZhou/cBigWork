/**
 * 这个文件是游戏核心逻辑的源文件
 * 除非你想要修改游戏框架，否则不需要改这个文件
 */

#include "stdafx.h"

#include "core.h"
#include "scene.h"

// 内部游戏时间：单位（ms）
static double gameTime = 0;// 游戏运行时间，从游戏开始到当前的时间
static double deltaTime = 0;// 两帧之间的时间差

// 时间戳：单位（ms）
static double firstFrameTimestamp = 0;// 游戏开始时的时间戳
static double lastFrameTimestamp = 0;// 上一帧的时间戳
static double lastRenderTimestamp = 0;// 上一次渲染的时间戳
static double GetCurrentTimestamp();// 工具函数声明

// 帧耗时统计：单位（ms）
static std::deque<double> deltaTimeHistory;// 用于统计更新帧耗时的历史数据
static std::deque<double> renderDeltaTimeHistory;// 用于统计渲染帧耗时的历史数据
static size_t deltaTimeHistorySize = 120;// 统计历史数据的最大数量
static void UpdateDeltaTime(std::deque<double>& history, double currentDeltaTime);// 工具函数声明
static double GetAverageDeltaTime(const std::deque<double>& history);// 工具函数声明
static void ShowAverageFps();// 工具函数声明
static void ShowAverageRenderFps();// 工具函数声明

// 游戏初始化
void GameInit(HWND hWnd, WPARAM wParam, LPARAM lParam)// 初始化游戏资源，切换到开始场景
{
    firstFrameTimestamp = GetCurrentTimestamp();
    lastFrameTimestamp = firstFrameTimestamp;
	lastRenderTimestamp = firstFrameTimestamp;
    // 初始化游戏资源
    GameResourceInit(hWnd, wParam, lParam);
    // 切换到开始场景
    ChangeScene(StartScene);
}

// 游戏循环
void GameLoop(HWND hWnd)// 每一帧调用一次，处理游戏逻辑，然后请求渲染
{
    // 游戏时间处理
	double currentFrameTimestamp = GetCurrentTimestamp();// 获取当前时间戳
	gameTime = currentFrameTimestamp - firstFrameTimestamp;// 计算游戏运行时间
	deltaTime = (currentFrameTimestamp - lastFrameTimestamp);// 计算两帧之间的时间差
	lastFrameTimestamp = currentFrameTimestamp;// 更新上一帧时间戳
	UpdateDeltaTime(deltaTimeHistory, deltaTime);// 更新帧耗时统计
	ShowAverageFps();// 显示FPS

    // 场景循环更新
	SceneLoop();// 更新当前场景的逻辑

    // 最后进行渲染，实际的渲染函数是GameRender，只重绘画面部分
	InvalidateRect(hWnd, nullptr, FALSE);// 请求重绘窗口客户区
}

// 渲染资源
extern HBITMAP bmp_WhiteBackground;// 白色背景位图资源

void GameRender(HWND hWnd, WPARAM wParam, LPARAM lParam)// 每次窗口需要重绘时调用，负责绘制当前场景
{
    // 渲染时间处理
	double currentTimestamp = GetCurrentTimestamp();// 获取当前时间戳
	double renderDeltaTime = (currentTimestamp - lastRenderTimestamp);// 计算两次渲染之间的时间差
	lastRenderTimestamp = currentTimestamp;// 更新上一次渲染时间戳
	UpdateDeltaTime(renderDeltaTimeHistory, renderDeltaTime);// 更新渲染帧耗时统计
	ShowAverageRenderFps();// 显示渲染FPS

    // 开始绘制
	PAINTSTRUCT ps;// 绘制结构体
	HDC hdc_window = BeginPaint(hWnd, &ps);// 获取窗口的设备上下文

    // 创建缓存
    HDC hdc_memBuffer = CreateCompatibleDC(hdc_window);
    HDC hdc_loadBmp = CreateCompatibleDC(hdc_window);

    // 初始化缓存
    HBITMAP blankBmp = CreateCompatibleBitmap(hdc_window, WINDOW_WIDTH, WINDOW_HEIGHT);
    SelectObject(hdc_memBuffer, blankBmp);

    // 清空背景
    SelectObject(hdc_loadBmp, bmp_WhiteBackground);
    BitBlt(hdc_memBuffer, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, hdc_loadBmp, 0, 0, SRCCOPY);

    // 绘制场景
    RenderScene(hdc_memBuffer, hdc_loadBmp);

    // 最后将所有的信息绘制到屏幕上
    BitBlt(hdc_window, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, hdc_memBuffer, 0, 0, SRCCOPY);

    // 回收资源所占的内存（非常重要）
    DeleteObject(blankBmp);
    DeleteDC(hdc_loadBmp);
    DeleteDC(hdc_memBuffer);

    // 结束绘制
    EndPaint(hWnd, &ps);
}

// 获取当前游戏时间（单位：s）
double GetGameTime()
{
    return gameTime / 1000.0;
}

// 获取两帧之间的时间差（单位：s）
double GetDeltaTime()
{
    return deltaTime / 1000.0;
}

// 工具函数：获取当前时间戳（单位：ms）
double GetCurrentTimestamp()
{
    LARGE_INTEGER freq, counter;
    QueryPerformanceFrequency(&freq);  // 每秒多少tick
    QueryPerformanceCounter(&counter); // 当前tick
    return (double)counter.QuadPart * 1000.0 / freq.QuadPart;
}

// 工具函数：更新帧耗时统计（单位：ms）
void UpdateDeltaTime(std::deque<double> &history, double currentDeltaTime)
{
    history.push_back(currentDeltaTime);
    if (history.size() > deltaTimeHistorySize)
    {
        history.pop_front();
    }
}

// 工具函数：获取平均帧耗时（单位：ms）
double GetAverageDeltaTime(const std::deque<double> &history)
{
    double sum = std::accumulate(history.begin(), history.end(), .0);
    return sum / history.size();
}

// 工具函数：显示FPS
void ShowAverageFps()
{
    static double lastShowTime = -1;
    // 控制刷新间隔
    if (GetGameTime() - lastShowTime < 1)
    {
        return;
    }
    lastShowTime = GetGameTime();
    double averageFps = 1000.0 / GetAverageDeltaTime(deltaTimeHistory);
    // 在状态栏位置0显示FPS
    Log(0, "Update FPS: %.1f", averageFps);
}

// 工具函数：显示渲染FPS
void ShowAverageRenderFps()
{
    static double lastShowTime = -1;
    // 控制刷新间隔
    if (GetGameTime() - lastShowTime < 1)
    {
        return;
    }
    lastShowTime = GetGameTime();
    double averageFps = 1000.0 / GetAverageDeltaTime(renderDeltaTimeHistory);
    // 在状态栏位置1显示FPS
    Log(1, "Render FPS: %.1f", averageFps);
}
