/**
 * 这个文件是工具库的源文件
 * 想要实现的工具函数，都可以放在这里
 */

#include "stdafx.h"

static std::mt19937 rng((unsigned int)std::time(nullptr));

// 生成随机数
int GetRandomInt(int min, int max)
{
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}

double GetRandomDouble(double min, double max)
{
    std::uniform_real_distribution<double> dist(min, max);
    return dist(rng);
}

// 向量模长
double Length(Vector2 in)
{
    return sqrt(in.x * in.x + in.y * in.y);
}

// 归一化向量
Vector2 Normalize(Vector2 in)
{
    double length = Length(in);
    if (length == 0)
    {
        return {0, 0};
    }
    Vector2 out = {in.x / length, in.y / length};
    return out;
}

// 创建纯色图像
HBITMAP CreateBackground(HWND hWnd, COLORREF color)
{
    // 开始绘制
    PAINTSTRUCT ps;
    HDC hdc_window = BeginPaint(hWnd, &ps);

    // 创建缓存
    HDC hdc_memBuffer = CreateCompatibleDC(hdc_window);

    // 初始化缓存
    HBITMAP bmp_output = CreateCompatibleBitmap(hdc_window, WINDOW_WIDTH, WINDOW_HEIGHT);
    SelectObject(hdc_memBuffer, bmp_output);

    // 创建纯白色画刷
    HBRUSH hBrush = CreateSolidBrush(color);

    // 创建一个矩形
    RECT rect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};

    // 将缓存填充为白色
    FillRect(hdc_memBuffer, &rect, hBrush);

    // 回收资源所占的内存（非常重要）
    DeleteObject(hBrush);
    DeleteDC(hdc_memBuffer);

    // 结束绘制
    EndPaint(hWnd, &ps);

    return bmp_output;
}

// 矩形与圆碰撞检测
bool IsRectCircleCollision(Rect rect, Circle c)
{
    // clamp(cx, rect.left, rect.right)
    double closestX = max(rect.left, min(c.center.x, rect.right));
    double closestY = max(rect.top, min(c.center.y, rect.bottom));

    double dx = c.center.x - closestX;
    double dy = c.center.y - closestY;

    return (dx * dx + dy * dy) <= (c.radius * c.radius);
}

// 矩形与矩形碰撞检测
bool IsRectRectCollision(Rect r1, Rect r2)
{
    return !(r2.left > r1.right ||
             r2.right < r1.left ||
             r2.top > r1.bottom ||
             r2.bottom < r1.top);
}
