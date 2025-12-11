#pragma once
#include "stdafx.h"
#include <string>

namespace TextUtil
{
    // 获取或创建缓存字体（调用方不需要 DeleteObject）
    HFONT GetCachedFont(int height, int weight, LPCTSTR faceName = UI_FONT_NAME);

    // 释放所有缓存的字体（在场景卸载或程序退出时调用一次）
    void ReleaseAllCachedFonts();

    // 在指定 rect 内绘制文本（自动选择并恢复字体/颜色/背景模式）
    // format 使用 DrawText 的 DT_ 值
    void DrawTextEx(HDC hdc, LPCTSTR text, const RECT& rect, UINT format, HFONT font, COLORREF textColor, int bkMode = TRANSPARENT);

    // 绘制带阴影的文字（先绘制阴影再绘制文本）
    void DrawTextWithShadow(HDC hdc, LPCTSTR text, const RECT& rect, UINT format, HFONT font,
        COLORREF textColor, COLORREF shadowColor,
        int shadowOffsetX = 2, int shadowOffsetY = 2);
}