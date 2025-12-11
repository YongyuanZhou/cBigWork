/**
 * 这个文件是鼠标输入的头文件
 * 如果要添加新的鼠标功能，修改在这里
 */

#pragma once

// Windows应用程序框架的写入函数
void MouseMove(HWND hWnd, WPARAM wParam, LPARAM lParam);
void LButtonDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
void LButtonUp(HWND hWnd, WPARAM wParam, LPARAM lParam);

// 游戏逻辑的读取函数
bool IsMouseLButtonDown();
int GetMouseX();
int GetMouseY();
