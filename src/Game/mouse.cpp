/**
 * 这个文件是鼠标输入的源文件
 * 如果要添加新的鼠标功能，修改在这里
 */

#include "stdafx.h"

#include "mouse.h"

static int mouseX;
static int mouseY;
static bool isMouseLButtonDown;

// 鼠标移动事件处理函数
void MouseMove(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    mouseX = LOWORD(lParam);
    mouseY = HIWORD(lParam);
}

// 鼠标左键按下事件处理函数
void LButtonDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    mouseX = LOWORD(lParam);
    mouseY = HIWORD(lParam);
    isMouseLButtonDown = true;
}

// 鼠标左键松开事件处理函数
void LButtonUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    mouseX = LOWORD(lParam);
    mouseY = HIWORD(lParam);
    isMouseLButtonDown = false;
}

// 获取当前是否有按钮按下
bool IsMouseLButtonDown()
{
    return isMouseLButtonDown;
}

// 获取当前鼠标的X坐标
int GetMouseX()
{
    return mouseX;
}

// 获取当前鼠标的Y坐标
int GetMouseY()
{
    return mouseY;
}
