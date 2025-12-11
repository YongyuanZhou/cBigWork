#include "stdafx.h"
#include "ui_text.h"
#include <unordered_map>

namespace TextUtil
{
    // 简单 key：height_weight_facename（widestring）
	static std::unordered_map<std::wstring, HFONT> g_fontCache;// 字体缓存

    static std::wstring BuildKey(int height, int weight, LPCTSTR faceName)
    {
#ifdef UNICODE
        std::wstring key = std::to_wstring(height) + L"_" + std::to_wstring(weight) + L"_" + (faceName ? faceName : L"");
#else
        // ANSI 编译时也支持
        std::wstring fname;
        if (faceName)
        {
            std::string s(faceName);
            fname.assign(s.begin(), s.end());
        }
        std::wstring key = std::to_wstring(height) + L"_" + std::to_wstring(weight) + L"_" + fname;
#endif
        return key;
    }

    HFONT GetCachedFont(int height, int weight, LPCTSTR faceName)
    {
        std::wstring key = BuildKey(height, weight, faceName);
        auto it = g_fontCache.find(key);
        if (it != g_fontCache.end())
            return it->second;

        HFONT hFont = CreateFont(
            height, 0, 0, 0, weight,
            FALSE, FALSE, FALSE, DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
            DEFAULT_PITCH | FF_SWISS, faceName ? faceName : UI_FONT_NAME);

        g_fontCache.emplace(key, hFont);
        return hFont;
    }

    void ReleaseAllCachedFonts()
    {
        for (auto& p : g_fontCache)
        {
            if (p.second)
                DeleteObject(p.second);
        }
        g_fontCache.clear();
    }

    void DrawTextEx(HDC hdc, LPCTSTR text, const RECT& rect, UINT format, HFONT font, COLORREF textColor, int bkMode)
    {
        if (!hdc || !text) return;

        // 保存旧状态
        int oldBk = SetBkMode(hdc, bkMode);
        COLORREF oldColor = SetTextColor(hdc, textColor);

        HFONT hOldFont = (HFONT)SelectObject(hdc, font);
        DrawText(hdc, text, -1, const_cast<RECT*>(&rect), format);
        // 恢复
        SelectObject(hdc, hOldFont);
        SetTextColor(hdc, oldColor);
        SetBkMode(hdc, oldBk);
    }

    void DrawTextWithShadow(HDC hdc, LPCTSTR text, const RECT& rect, UINT format, HFONT font,
        COLORREF textColor, COLORREF shadowColor,
        int shadowOffsetX, int shadowOffsetY)
    {
        if (!hdc || !text) return;

        RECT shadowRect = rect;
        OffsetRect(&shadowRect, shadowOffsetX, shadowOffsetY);

        // 绘制阴影
        DrawTextEx(hdc, text, shadowRect, format, font, shadowColor, TRANSPARENT);
        // 绘制正文
        DrawTextEx(hdc, text, rect, format, font, textColor, TRANSPARENT);
    }
}