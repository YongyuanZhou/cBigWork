#include "stdafx.h"

#include "scene.h"
#include "help_scene.h"

#include "button.h"
#include "mouse.h"
#include "ui_text.h"
#include "config.h"

#include <vector>
#include <string>
#include <fstream>

// 按钮
static ButtonId btnReset = 0;
static ButtonId btnConfirmReset = 0;
static ButtonId btnCancelReset = 0;
static ButtonId btnBack = 0;

// 状态
static bool s_showConfirmReset = false;
// 鼠标去抖
static bool g_mousePrevDown_Help = false;

// 渲染规则文本
static std::vector<std::wstring> s_rules = {
    L"1. 控制飞机躲避敌机并击落它们。",
    L"2. 收集词条以获得临时增益。",
    L"3. 每击落敌机可获得积分，积分会触发词条。",
    L"4. 游戏结束后会进入结算界面，保存分数到排行榜。",
};

// 按钮渲染
static void RenderSimpleButton(Button* b, HDC mem, HDC load, const TCHAR* text, COLORREF color)
{
    RECT rc = {(int)b->position.x, (int)b->position.y, (int)(b->position.x + b->width), (int)(b->position.y + b->height)};
    HBRUSH hb = CreateSolidBrush(color); FillRect(mem, &rc, hb); DeleteObject(hb);
    HFONT f = TextUtil::GetCachedFont(18, FW_BOLD, UI_FONT_NAME); HFONT old = (HFONT)SelectObject(mem, f);
    SetBkMode(mem, TRANSPARENT); SetTextColor(mem, RGB(0,0,0)); DrawText(mem, text, -1, &rc, DT_CENTER|DT_VCENTER|DT_SINGLELINE); SelectObject(mem, old);
}

static void RenderReset(Button* b, HDC mem, HDC load) { RenderSimpleButton(b, mem, load, TEXT("重置排行榜"), RGB(200,100,100)); }
static void RenderConfirm(Button* b, HDC mem, HDC load) { RenderSimpleButton(b, mem, load, TEXT("确认重置"), RGB(100,200,100)); }
static void RenderCancel(Button* b, HDC mem, HDC load) { RenderSimpleButton(b, mem, load, TEXT("取消"), RGB(200,200,200)); }
static void RenderBack(Button* b, HDC mem, HDC load) { RenderSimpleButton(b, mem, load, TEXT("返回"), RGB(180,180,180)); }

// Helper: show/hide confirm dialog and enable/disable appropriate buttons
static void SetConfirmDialogVisible(bool visible)
{
    s_showConfirmReset = visible;
    if (visible)
    {
        // enable confirm/cancel, disable others to avoid accidental clicks
        EnableButton(btnConfirmReset);
        EnableButton(btnCancelReset);
        DisableButton(btnReset);
        DisableButton(btnBack);
    }
    else
    {
        DisableButton(btnConfirmReset);
        DisableButton(btnCancelReset);
        EnableButton(btnReset);
        EnableButton(btnBack);
    }
}

// 点击处理
static void OnReset(Button* b) { SetConfirmDialogVisible(true); }
static void OnConfirmReset(Button* b) {
    // 清空排行榜文件
    std::ofstream ofs(LEADERBOARD_DATA_FILE, std::ofstream::trunc);
    if (ofs.is_open()) ofs.close();
    SetConfirmDialogVisible(false);
}
static void OnCancelReset(Button* b) { SetConfirmDialogVisible(false); }
static void OnBack(Button* b) { ChangeScene(SceneId::StartScene); }

void LoadScene_Help_Scene()
{
    // 布局居中
    int centerX = WINDOW_WIDTH/2;
    int top = 120;
    int btnW = 140, btnH = 40, gap = 20;
    btnReset = CreateButton(centerX - btnW/2, WINDOW_HEIGHT - 140, btnW, btnH, RenderReset, OnReset); EnableButton(btnReset);
    btnBack = CreateButton(40, WINDOW_HEIGHT - 80, 120, btnH, RenderBack, OnBack); EnableButton(btnBack);
    // 确认对话的按钮（默认隐藏/禁用）
    btnConfirmReset = CreateButton(centerX - btnW - gap/2, WINDOW_HEIGHT/2 + 40, btnW, btnH, RenderConfirm, OnConfirmReset);
    btnCancelReset = CreateButton(centerX + gap/2, WINDOW_HEIGHT/2 + 40, btnW, btnH, RenderCancel, OnCancelReset);
    // ensure confirm buttons disabled initially
    DisableButton(btnConfirmReset);
    DisableButton(btnCancelReset);
    s_showConfirmReset = false;
    g_mousePrevDown_Help = false;
}

void UnloadScene_Help_Scene()
{
    DestroyButtons();
}

void ProcessUiInput_Help_Scene()
{
    bool mouseDown = IsMouseLButtonDown();
    // only trigger on edge from not-pressed to pressed
    if (mouseDown && !g_mousePrevDown_Help)
    {
        // if confirm dialog visible, only allow confirm/cancel clicks
        if (s_showConfirmReset)
        {
            // PressButtons will check enabled buttons, so just call it
            PressButtons(GetMouseX(), GetMouseY());
        }
        else
        {
            PressButtons(GetMouseX(), GetMouseY());
        }
    }
    g_mousePrevDown_Help = mouseDown;
}

void CheckCollision_Help_Scene() {}
void UpdateScene_Help_Scene(double deltaTime) {}

void RenderScene_Help_Scene(HDC hdc_memBuffer, HDC hdc_loadBmp)
{
    SetBkMode(hdc_memBuffer, TRANSPARENT);
    HFONT hTitle = TextUtil::GetCachedFont(36, FW_BOLD, UI_FONT_NAME);
    HFONT old = (HFONT)SelectObject(hdc_memBuffer, hTitle);
    RECT rcTitle = {0, 20, WINDOW_WIDTH, 80};
    SetTextColor(hdc_memBuffer, RGB(0,0,0)); DrawText(hdc_memBuffer, TEXT("游戏帮助与规则"), -1, &rcTitle, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
    SelectObject(hdc_memBuffer, old);

    // 绘制规则列表
    HFONT hRule = TextUtil::GetCachedFont(18, FW_NORMAL, UI_FONT_NAME);
    HFONT oldR = (HFONT)SelectObject(hdc_memBuffer, hRule);
    int startY = 120;
    for (size_t i=0;i<s_rules.size();++i) {
        RECT r = {100, startY + (int)i*36, WINDOW_WIDTH-100, startY + (int)i*36 + 30};
        DrawTextW(hdc_memBuffer, s_rules[i].c_str(), -1, &r, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    }
    SelectObject(hdc_memBuffer, oldR);

    // 绘制排行榜（右侧）
    HFONT hLb = TextUtil::GetCachedFont(18, FW_NORMAL, UI_FONT_NAME);
    HFONT oldLb = (HFONT)SelectObject(hdc_memBuffer, hLb);
    std::vector<int> scores;
    std::ifstream ifs(LEADERBOARD_DATA_FILE);
    std::string line;
    while (ifs.is_open() && std::getline(ifs, line)) {
        try { scores.push_back(std::stoi(line)); } catch(...){}
    }
    ifs.close();
    std::sort(scores.begin(), scores.end(), std::greater<int>());
    RECT rLb = { WINDOW_WIDTH - 300, 120, WINDOW_WIDTH - 40, 400 };
    TCHAR buf[256];
    swprintf_s(buf, _countof(buf), TEXT("排行榜"));
    DrawText(hdc_memBuffer, buf, -1, &rLb, DT_LEFT | DT_TOP);
    for (int i=0;i<3;i++) {
        TCHAR item[32];
        if (i < (int)scores.size()) swprintf_s(item, _countof(item), TEXT("%d. %d"), i+1, scores[i]); else swprintf_s(item, _countof(item), TEXT("%d. --"), i+1);
        RECT ir = { WINDOW_WIDTH - 300, 170 + i*30, WINDOW_WIDTH - 40, 200 + i*30 };
        DrawText(hdc_memBuffer, item, -1, &ir, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    }
    SelectObject(hdc_memBuffer, oldLb);

    // 绘制底部按钮或确认框
    if (s_showConfirmReset) {
        // 半透明遮罩
        HBRUSH hb = CreateSolidBrush(RGB(0,0,0));
        SetBkMode(hdc_memBuffer, TRANSPARENT);
        RECT mask = {0,0,WINDOW_WIDTH,WINDOW_HEIGHT};
        FrameRect(hdc_memBuffer, &mask, hb);
        // 对话框背景
        RECT dlg = { WINDOW_WIDTH/2 - 250, WINDOW_HEIGHT/2 - 80, WINDOW_WIDTH/2 + 250, WINDOW_HEIGHT/2 + 80 };
        HBRUSH hb2 = CreateSolidBrush(RGB(240,240,240)); FillRect(hdc_memBuffer, &dlg, hb2); DeleteObject(hb2);
        HFONT hf = TextUtil::GetCachedFont(20, FW_BOLD, UI_FONT_NAME);
        HFONT oldf = (HFONT)SelectObject(hdc_memBuffer, hf);
        RECT t = { dlg.left, dlg.top + 20, dlg.right, dlg.top + 60 };
        DrawText(hdc_memBuffer, TEXT("确认重置排行榜？"), -1, &t, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        SelectObject(hdc_memBuffer, oldf);
        DeleteObject(hb);
        // 绘制确认取消按钮
        RenderButtons(hdc_memBuffer, hdc_loadBmp);
    } else {
        RenderButtons(hdc_memBuffer, hdc_loadBmp);
    }
}

