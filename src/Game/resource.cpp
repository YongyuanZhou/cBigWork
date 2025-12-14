/**
 * 这个文件是静态资源初始化的源文件
 * 如果要添加新的资源，修改在这里
 */

#include "stdafx.h"

#include "resource.h"

#pragma region 静态图片资源

// 图片资源
HBITMAP bmp_WhiteBackground; // 生成的纯白背景
HBITMAP bmp_BlackBackground; // 生成的纯黑背景
HBITMAP bmp_StartButton;     // 开始按钮图片
HBITMAP bmp_Player;          // 角色图片
HBITMAP bmp_Enemy;           // 敌人图片
HBITMAP bmp_SettingIcon;     // 设置图标图片
HBITMAP bmp_HelpIcon;        // 帮助图标图片

void GameResourceInit(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    bmp_WhiteBackground = CreateBackground(hWnd, RGB(255, 255, 255));
    bmp_BlackBackground = CreateBackground(hWnd, RGB(0, 0, 0));
    bmp_StartButton = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
		MAKEINTRESOURCE(IDB_BITMAP_START_BUTTON));// 加载开始按钮图片
    bmp_Player = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
        MAKEINTRESOURCE(IDB_BITMAP_PLANE));
    // TODO: 引入其他的静态资源
    bmp_Enemy = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
        MAKEINTRESOURCE(IDB_BITMAP_ENEMY));
	bmp_SettingIcon = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
		MAKEINTRESOURCE(IDB_BITMAP_SETTING_ICON));
    bmp_HelpIcon = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
		MAKEINTRESOURCE(IDB_BITMAP_HELP_BUTTON));
}

#pragma endregion
