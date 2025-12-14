#include "stdafx.h"

#include "scene.h"
#include "config.h"
#include "ui_text.h"
#include "settlement.h"
#include "mouse.h"
#include "button.h"
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

// 上一局的数据定义（由游戏场景填写）
int g_lastScore = 0;
double g_lastTime = 0.0;

static std::vector<int> LoadAllScores()
{
    std::vector<int> scores;
    std::ifstream ifs(LEADERBOARD_DATA_FILE);
    if (!ifs.is_open()) return scores;
    std::string line;
    while (std::getline(ifs, line))
    {
        // trim and parse int safely
        size_t start = line.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) continue;
        size_t end = line.find_last_not_of(" \t\r\n");
        std::string token = line.substr(start, end - start + 1);
        try { scores.push_back(std::stoi(token)); }
        catch (...) { continue; }
    }
    ifs.close();
    return scores;
}

// 按钮句 HANDLE
static ButtonId g_quitBtn = 0;
static ButtonId g_retryBtn = 0;

// 按钮渲染与回调
static void RenderQuitButton(Button* button, HDC hdc_memBuffer, HDC hdc_loadBmp)
{
    RECT rc = { (int)button->position.x, (int)button->position.y, (int)(button->position.x + button->width), (int)(button->position.y + button->height) };
    HBRUSH hBrush = CreateSolidBrush(RGB(200, 50, 50));
    FillRect(hdc_memBuffer, &rc, hBrush);
    DeleteObject(hBrush);
    // 文字
    HFONT hFont = TextUtil::GetCachedFont(20, FW_BOLD, UI_FONT_NAME);
    HFONT hOld = (HFONT)SelectObject(hdc_memBuffer, hFont);
    SetBkMode(hdc_memBuffer, TRANSPARENT);
    SetTextColor(hdc_memBuffer, RGB(0,0,0));
    DrawText(hdc_memBuffer, TEXT("退出游戏"), -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    SelectObject(hdc_memBuffer, hOld);
}

static void RenderRetryButton(Button* button, HDC hdc_memBuffer, HDC hdc_loadBmp)
{
    RECT rc = { (int)button->position.x, (int)button->position.y, (int)(button->position.x + button->width), (int)(button->position.y + button->height) };
    HBRUSH hBrush = CreateSolidBrush(RGB(100, 200, 100));
    FillRect(hdc_memBuffer, &rc, hBrush);
    DeleteObject(hBrush);
    // 文字
    HFONT hFont = TextUtil::GetCachedFont(20, FW_BOLD, UI_FONT_NAME);
    HFONT hOld = (HFONT)SelectObject(hdc_memBuffer, hFont);
    SetBkMode(hdc_memBuffer, TRANSPARENT);
    SetTextColor(hdc_memBuffer, RGB(0,0,0));
    DrawText(hdc_memBuffer, TEXT("继续挑战"), -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    SelectObject(hdc_memBuffer, hOld);
}

static void OnQuitButtonClick(Button* button)
{
    // 发送退出消息，主循环会退出
    PostQuitMessage(0);
}

static void OnRetryButtonClick(Button* button)
{
    // 切换回游戏场景重新开始
    ChangeScene(SceneId::StartScene);
}

void LoadScene_Settlement_Scene()
{
    // 创建按钮：放在窗口下方中央区域
    int btnW = 180;
    int btnH = 50;
    int gap = 40;
    int centerX = WINDOW_WIDTH / 2;
    int y = WINDOW_HEIGHT - 140;
    int leftX = centerX - btnW - gap/2;
    int rightX = centerX + gap/2;

    g_retryBtn = CreateButton(leftX, y, btnW, btnH, RenderRetryButton, OnRetryButtonClick);
    EnableButton(g_retryBtn);
    g_quitBtn = CreateButton(rightX, y, btnW, btnH, RenderQuitButton, OnQuitButtonClick);
    EnableButton(g_quitBtn);
}

void UnloadScene_Settlement_Scene()
{
    // 销毁本场景按钮（销毁所有按钮）
    DestroyButtons();
}

void ProcessUiInput_Settlement_Scene()
{
    if (IsMouseLButtonDown())
    {
        PressButtons(GetMouseX(), GetMouseY());
    }
}

void CheckCollision_Settlement_Scene()
{
}

void UpdateScene_Settlement_Scene(double deltaTime)
{
}

void RenderScene_Settlement_Scene(HDC hdc_memBuffer, HDC hdc_loadBmp)
{
    // 清除不必要的排行榜绘制，仅显示存活时间与得分（分行，黑色）
    SetBkMode(hdc_memBuffer, TRANSPARENT);
    HFONT hTitleFont = TextUtil::GetCachedFont(48, FW_BOLD, UI_FONT_NAME);
    HFONT hOld = (HFONT)SelectObject(hdc_memBuffer, hTitleFont);

    // 标题
    RECT rcTitle = { 0, 40, WINDOW_WIDTH, 40 + 80 };
    SetTextColor(hdc_memBuffer, RGB(0,0,0));
    DrawText(hdc_memBuffer, TEXT("本次成绩"), -1, &rcTitle, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // 分别显示存活时间与得分，黑色，分行
    HFONT hInfoFont = TextUtil::GetCachedFont(36, FW_NORMAL, UI_FONT_NAME);
    SelectObject(hdc_memBuffer, hInfoFont);
    TCHAR timeBuf[64];
    swprintf_s(timeBuf, _countof(timeBuf), TEXT("存活时间: %.2f 秒"), g_lastTime);
    RECT rcTime = { 0, WINDOW_HEIGHT/2 - 40, WINDOW_WIDTH, WINDOW_HEIGHT/2 };
    SetTextColor(hdc_memBuffer, RGB(0,0,0));
    DrawText(hdc_memBuffer, timeBuf, -1, &rcTime, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    TCHAR scoreBuf[64];
    swprintf_s(scoreBuf, _countof(scoreBuf), TEXT("得分: %d"), g_lastScore);
    RECT rcScore = { 0, WINDOW_HEIGHT/2 + 10, WINDOW_WIDTH, WINDOW_HEIGHT/2 + 50 };
    SetTextColor(hdc_memBuffer, RGB(0,0,0));
    DrawText(hdc_memBuffer, scoreBuf, -1, &rcScore, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // 绘制按钮
    RenderButtons(hdc_memBuffer, hdc_loadBmp);

    // 恢复字体
    SelectObject(hdc_memBuffer, hOld);
}
