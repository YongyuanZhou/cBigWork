#include "stdafx.h"

#include "scene.h"
#include "setting_scene.h"

#include "mouse.h"
#include "button.h"
#include "ui_text.h"
#include "config.h"

#include <fstream>
#include <string>

// 按钮句 handle
static ButtonId btnEasy = 0;
static ButtonId btnNormal = 0;
static ButtonId btnHard = 0;
static ButtonId btnVolDec = 0;
static ButtonId btnVolInc = 0;
static ButtonId btnConfirm = 0;
static ButtonId btnCancel = 0;

// 临时选择
static int s_selectedDifficulty = 1; // 0/1/2
static int s_selectedVolume = g_masterVolume;

// 渲染辅助
static void DrawOptionButton(Button* b, HDC hdc_memBuffer, HDC hdc_loadBmp, const TCHAR* text, bool selected)
{
    RECT rc = { (int)b->position.x, (int)b->position.y, (int)(b->position.x + b->width), (int)(b->position.y + b->height) };
    HBRUSH hBrush = CreateSolidBrush(selected ? RGB(180,180,250) : RGB(200,200,200));
    FillRect(hdc_memBuffer, &rc, hBrush);
    DeleteObject(hBrush);
    HFONT f = TextUtil::GetCachedFont(18, FW_BOLD, UI_FONT_NAME);
    HFONT old = (HFONT)SelectObject(hdc_memBuffer, f);
    SetBkMode(hdc_memBuffer, TRANSPARENT);
    SetTextColor(hdc_memBuffer, RGB(0,0,0));
    DrawText(hdc_memBuffer, text, -1, &rc, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
    SelectObject(hdc_memBuffer, old);
}

// 各按钮的渲染与回调
static void RenderEasy(Button* b, HDC mem, HDC load) { DrawOptionButton(b, mem, load, TEXT("简单"), s_selectedDifficulty==0); }
static void RenderNormal(Button* b, HDC mem, HDC load) { DrawOptionButton(b, mem, load, TEXT("普通"), s_selectedDifficulty==1); }
static void RenderHard(Button* b, HDC mem, HDC load) { DrawOptionButton(b, mem, load, TEXT("困难"), s_selectedDifficulty==2); }
static void OnEasy(Button* b) { s_selectedDifficulty = 0; }
static void OnNormal(Button* b) { s_selectedDifficulty = 1; }
static void OnHard(Button* b) { s_selectedDifficulty = 2; }

static void RenderVolDec(Button* b, HDC mem, HDC load) {
    RECT rc = { (int)b->position.x, (int)b->position.y, (int)(b->position.x + b->width), (int)(b->position.y + b->height) };
    HBRUSH hb = CreateSolidBrush(RGB(220,220,220)); FillRect(mem, &rc, hb); DeleteObject(hb);
    HFONT f = TextUtil::GetCachedFont(18, FW_BOLD, UI_FONT_NAME); HFONT old = (HFONT)SelectObject(mem, f);
    SetBkMode(mem, TRANSPARENT); SetTextColor(mem, RGB(0,0,0)); DrawText(mem, TEXT("-"), -1, &rc, DT_CENTER|DT_VCENTER|DT_SINGLELINE); SelectObject(mem, old);
}
static void RenderVolInc(Button* b, HDC mem, HDC load) {
    RECT rc = { (int)b->position.x, (int)b->position.y, (int)(b->position.x + b->width), (int)(b->position.y + b->height) };
    HBRUSH hb = CreateSolidBrush(RGB(220,220,220)); FillRect(mem, &rc, hb); DeleteObject(hb);
    HFONT f = TextUtil::GetCachedFont(18, FW_BOLD, UI_FONT_NAME); HFONT old = (HFONT)SelectObject(mem, f);
    SetBkMode(mem, TRANSPARENT); SetTextColor(mem, RGB(0,0,0)); DrawText(mem, TEXT("+"), -1, &rc, DT_CENTER|DT_VCENTER|DT_SINGLELINE); SelectObject(mem, old);
}
static void OnVolDec(Button* b) { if (s_selectedVolume > 0) { s_selectedVolume -= 2; if (s_selectedVolume < 0) s_selectedVolume = 0; } }
static void OnVolInc(Button* b) { if (s_selectedVolume < 100) { s_selectedVolume += 2; if (s_selectedVolume > 100) s_selectedVolume = 100; } }

static void RenderConfirm(Button* b, HDC mem, HDC load) {
    RECT rc = { (int)b->position.x, (int)b->position.y, (int)(b->position.x + b->width), (int)(b->position.y + b->height) };
    HBRUSH hb = CreateSolidBrush(RGB(100,200,100)); FillRect(mem, &rc, hb); DeleteObject(hb);
    HFONT f = TextUtil::GetCachedFont(18, FW_BOLD, UI_FONT_NAME); HFONT old = (HFONT)SelectObject(mem, f);
    SetBkMode(mem, TRANSPARENT); SetTextColor(mem, RGB(0,0,0)); DrawText(mem, TEXT("确认"), -1, &rc, DT_CENTER|DT_VCENTER|DT_SINGLELINE); SelectObject(mem, old);
}
static void RenderCancel(Button* b, HDC mem, HDC load) {
    RECT rc = { (int)b->position.x, (int)b->position.y, (int)(b->position.x + b->width), (int)(b->position.y + b->height) };
    HBRUSH hb = CreateSolidBrush(RGB(200,200,200)); FillRect(mem, &rc, hb); DeleteObject(hb);
    HFONT f = TextUtil::GetCachedFont(18, FW_BOLD, UI_FONT_NAME); HFONT old = (HFONT)SelectObject(mem, f);
    SetBkMode(mem, TRANSPARENT); SetTextColor(mem, RGB(0,0,0)); DrawText(mem, TEXT("取消"), -1, &rc, DT_CENTER|DT_VCENTER|DT_SINGLELINE); SelectObject(mem, old);
}

static void OnConfirm(Button* b)
{
    // 写回全局设置并保存文件
    g_difficultyIndex = s_selectedDifficulty;
    g_masterVolume = s_selectedVolume;
    // 保存到文件
    std::ofstream ofs(SETTINGS_DATA_FILE);
    if (ofs.is_open()) {
        ofs << g_masterVolume << std::endl;
        ofs << g_difficultyIndex << std::endl;
        ofs.close();
    }
    // 切回开始场景
    ChangeScene(SceneId::StartScene);
}
static void OnCancel(Button* b)
{
    ChangeScene(SceneId::StartScene);
}

void LoadScene_Setting_Scene()
{
    // 初始化临时值
    s_selectedDifficulty = g_difficultyIndex;
    s_selectedVolume = g_masterVolume;

    int top = 150;
    int btnW = 120; int btnH = 40; int spacing = 20;
    // Center the whole controls block
    int totalBtnWidth = 3 * btnW + 2 * spacing; // difficulty buttons total width
    int centerX = WINDOW_WIDTH / 2;
    int left = centerX - totalBtnWidth / 2;

    btnEasy = CreateButton(left, top, btnW, btnH, RenderEasy, OnEasy);
    EnableButton(btnEasy);
    btnNormal = CreateButton(left + (btnW+spacing), top, btnW, btnH, RenderNormal, OnNormal);
    EnableButton(btnNormal);
    btnHard = CreateButton(left + 2*(btnW+spacing), top, btnW, btnH, RenderHard, OnHard);
    EnableButton(btnHard);

    // 音量按钮（居中于难度区域下方）
    int volW = 50;
    int volSpacing = 10;
    int volTotalW = volW * 2 + volSpacing;
    int volLeft = centerX - volTotalW / 2;
    btnVolDec = CreateButton(volLeft, top + 100, volW, btnH, RenderVolDec, OnVolDec);
    EnableButton(btnVolDec);
    btnVolInc = CreateButton(volLeft + volW + volSpacing, top + 100, volW, btnH, RenderVolInc, OnVolInc);
    EnableButton(btnVolInc);

    // 确认/取消 (centered)
    int confW = 120; int confGap = 20;
    int confTotal = confW * 2 + confGap;
    int confLeft = centerX - confTotal / 2;
    btnConfirm = CreateButton(confLeft, top + 200, confW, btnH, RenderConfirm, OnConfirm);
    EnableButton(btnConfirm);
    btnCancel = CreateButton(confLeft + confW + confGap, top + 200, confW, btnH, RenderCancel, OnCancel);
    EnableButton(btnCancel);
}

void UnloadScene_Setting_Scene()
{
    DestroyButtons();
}

void ProcessUiInput_Setting_Scene()
{
    if (IsMouseLButtonDown()) {
        PressButtons(GetMouseX(), GetMouseY());
    }
}

void CheckCollision_Setting_Scene() {}
void UpdateScene_Setting_Scene(double deltaTime) {}

void RenderScene_Setting_Scene(HDC hdc_memBuffer, HDC hdc_loadBmp)
{
    SetBkMode(hdc_memBuffer, TRANSPARENT);
    HFONT fh = TextUtil::GetCachedFont(40, FW_BOLD, UI_FONT_NAME);
    HFONT old = (HFONT)SelectObject(hdc_memBuffer, fh);
    RECT rc = {0,20,WINDOW_WIDTH,100};
    SetTextColor(hdc_memBuffer, RGB(0,0,0)); DrawText(hdc_memBuffer, TEXT("设置"), -1, &rc, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
    SelectObject(hdc_memBuffer, old);

    // 绘制当前音量值和难度提示，及左侧标签（居中布局）
    HFONT f2 = TextUtil::GetCachedFont(20, FW_NORMAL, UI_FONT_NAME);
    HFONT old2 = (HFONT)SelectObject(hdc_memBuffer, f2);
    TCHAR buf[64];
    swprintf_s(buf, _countof(buf), TEXT("音量: %d"), s_selectedVolume);
    // compute layout same as LoadScene_Setting_Scene
    int top = 150;
    int btnW = 120; int spacing = 20; int btnH = 40;
    int totalBtnW = 3 * btnW + 2 * spacing;
    int centerX = WINDOW_WIDTH / 2;
    int left = centerX - totalBtnW / 2;
    // Difficulty label to the left of difficulty buttons
    RECT lblDiff = { left - 160, top, left - 20, top + btnH };
    SetTextColor(hdc_memBuffer, RGB(0,0,0));
    DrawText(hdc_memBuffer, TEXT("难度设置"), -1, &lblDiff, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    // Draw difficulty description
    swprintf_s(buf, _countof(buf), TEXT("难度: %s"), s_selectedDifficulty==0?TEXT("简单"):(s_selectedDifficulty==1?TEXT("普通"):TEXT("困难")));
    RECT r3 = { centerX - 100, top + btnH + 10, centerX + 100, top + btnH + 40 };
    DrawText(hdc_memBuffer, buf, -1, &r3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    // Volume label to the left of volume buttons
    int volW = 50;
    int volSpacing = 10;
    int volTotalW = volW * 2 + volSpacing;
    int volLeft = centerX - volTotalW / 2;
    RECT lblVol = { volLeft - 160, top + 100, volLeft - 20, top + 100 + btnH };
    DrawText(hdc_memBuffer, TEXT("音量设置"), -1, &lblVol, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    swprintf_s(buf, _countof(buf), TEXT("音量: %d"), s_selectedVolume);
    RECT r2 = { centerX - 100, top + 100 + btnH + 10, centerX + 100, top + 100 + btnH + 40 };
    DrawText(hdc_memBuffer, buf, -1, &r2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    SelectObject(hdc_memBuffer, old2);

    // 绘制按钮
    RenderButtons(hdc_memBuffer, hdc_loadBmp);
}