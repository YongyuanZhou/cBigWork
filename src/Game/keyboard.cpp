/**
 * 这个文件是键盘输入的源文件
 * 如果要添加新的键盘功能，修改在这里
 */

#include "stdafx.h"

#include "keyboard.h"

static std::vector<bool> keyboard(128, 0);

// 键盘按下事件处理函数
void KeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if (wParam > 127)
    {
        return;
    }
    keyboard[wParam] = true;
}

// 键盘松开事件处理函数
void KeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if (wParam > 127)
    {
        return;
    }
    keyboard[wParam] = false;
}

// 获取按键是否按下
bool GetKeyDown(int keycode)
{
    if (keycode > 127)
    {
        return false;
    }
    return keyboard[keycode];
}
