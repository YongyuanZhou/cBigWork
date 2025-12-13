/**
 * 这个文件是场景1-开始场景的源文件
 * 场景的逻辑实现在这里
 * 务必定义并实现所有的场景核心函数
 */

#include "stdafx.h"

#include "scene.h"
#include "scene1.h"

#include "mouse.h"
#include "button.h"
#include "ui_text.h" // 新增：文字绘制工具

#pragma region 按钮逻辑
static void RenderStartButton(Button *button, HDC hdc_memBuffer, HDC hdc_loadBmp);
static void OnStartButtonClick(Button *button);

// 新增：设置与帮助按钮的渲染与回调
static void RenderSettingsButton(Button* button, HDC hdc_memBuffer, HDC hdc_loadBmp);
static void OnSettingsButtonClick(Button* button);
static void RenderHelpButton(Button* button, HDC hdc_memBuffer, HDC hdc_loadBmp);
static void OnHelpButtonClick(Button* button);
#pragma endregion

#pragma region 碰撞检测
#pragma endregion

// 加载开始场景
void LoadScene_StartScene()
{
    /* UI组件创建 */
    // 创建按钮
    const int width = 300;
    const int height = 200;
	const int x = (WINDOW_WIDTH - width) / 2 - 10; // 微调左移10像素
    const int y = 196;
    // TODO: 创建一个在这个位置的按钮
    ButtonId startButtonId = CreateButton(x, y, width, height,
        RenderStartButton, OnStartButtonClick);
    EnableButton(startButtonId);

    // 新增：设置与帮助按钮（放在开始按钮正下方并排）
    const int smallW = 140;
    const int smallH = 50;
    const int spacing = 20;
    const int smallY = y + height + spacing;

    // 左侧设置按钮，靠近开始按钮左侧内边距
    int settingsX = x + 20;
    ButtonId settingsBtn = CreateButton(settingsX, smallY, smallW, smallH,
        RenderSettingsButton, OnSettingsButtonClick);
    EnableButton(settingsBtn);

    // 右侧帮助按钮，靠近开始按钮右侧内边距
    int helpX = x + width - 20 - smallW;
    ButtonId helpBtn = CreateButton(helpX, smallY, smallW, smallH,
        RenderHelpButton, OnHelpButtonClick);
    EnableButton(helpBtn);

    /* 游戏对象创建 */
    // 开始场景暂时没有游戏对象需要创建
}

// 卸载开始场景
void UnloadScene_StartScene()
{
    /* UI组件销毁 */
    // TODO: 销毁所有按钮
    DestroyButtons();
    /* 游戏对象销毁 */
    // 开始场景暂时没有游戏对象需要销毁
}

// 处理开始场景的用户输入
void ProcessUiInput_StartScene()
{
    // TODO: 处理鼠标点击按钮
    // TODO: 处理鼠标点击按钮
    if (IsMouseLButtonDown())
    {
        PressButtons(GetMouseX(), GetMouseY());
    }
}

// 碰撞检测开始场景
void CheckCollision_StartScene()
{
    // 开始场景暂时没有游戏对象需要碰撞检测
}

// 更新开始场景
void UpdateScene_StartScene(double deltaTime)
{
    /* UI组件更新 */
    // 开始场景暂时没有UI组件需要更新

    /* 游戏对象更新 */
    // 开始场景暂时没有游戏对象需要更新
}

// 渲染开始场景
void RenderScene_StartScene(HDC hdc_memBuffer, HDC hdc_loadBmp)
{
    /*
     * 注意绘制顺序，后绘制的会覆盖先绘制的
     * 所以先绘制游戏对象，再绘制UI组件
     */

    /* 游戏对象绘制 */
    // 开始场景暂时没有游戏对象需要绘制

    /* UI组件绘制 */
    // 绘制所有的按钮
    RenderButtons(hdc_memBuffer, hdc_loadBmp);
    // 绘制提示文字（使用 TextUtil 缓存字体并绘制）
    //HFONT hFont = TextUtil::GetCachedFont(30, FW_NORMAL, TEXT("微软雅黑"));
    // 选择自定义字体到设备上下文
    //HFONT hOldFont = (HFONT)SelectObject(hdc_memBuffer, hFont);
    // 设置字体区域
    //const int width = 800;
    //const int height = 300;
    //const int left = WINDOW_WIDTH / 2 - width / 2 - 10;
    //const int top = 456;
    //const int right = left + width;
    //const int bottom = top + height;
    //RECT rect = {left, top, right, bottom};
    // 使用 DrawTextEx（透明背景，白色文字）
    //TextUtil::DrawTextEx(hdc_memBuffer,
        //TEXT("使用WASD或方向键控制飞机移动\n使用空格发射子弹\n\n请大家好好学习这个框架_(:зゝ∠)_"),
        //rect, DT_CENTER, hFont, RGB(0, 0, 0), TRANSPARENT);
    // TODO: 开始场景其他需要绘制的UI组件
}

#pragma region 按钮逻辑

extern HBITMAP bmp_StartButton;

void RenderStartButton(Button *button, HDC hdc_memBuffer, HDC hdc_loadBmp)
{
    // TODO: 绘制开始按钮
    SelectObject(hdc_loadBmp, bmp_StartButton);
    TransparentBlt(
        hdc_memBuffer, (int)button->position.x, (int)button->position.y,
        button->width, button->height,
        hdc_loadBmp, 0, 0, button->width, button->height,
        RGB(255, 255, 255));
}

void OnStartButtonClick(Button *button)
{
    // TODO: 开始按钮点击事件处理
    Log(1, TEXT("游戏开始！"));
    ChangeScene(GameScene);
}

// 渲染设置按钮（简单矩形 + 文本）
void RenderSettingsButton(Button* button, HDC hdc_memBuffer, HDC hdc_loadBmp)
{
    // 背景
    HBRUSH hBrush = CreateSolidBrush(RGB(60, 60, 60));
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc_memBuffer, hBrush);
    HPEN hPen = CreatePen(PS_SOLID, 2, RGB(200, 200, 200));
    HPEN oldPen = (HPEN)SelectObject(hdc_memBuffer, hPen);

    Rectangle(hdc_memBuffer,
        (int)button->position.x, (int)button->position.y,
        (int)(button->position.x + button->width),
        (int)(button->position.y + button->height));
    // 文本
    HFONT hFont = TextUtil::GetCachedFont(18, FW_NORMAL, UI_FONT_NAME);
    HFONT hOld = (HFONT)SelectObject(hdc_memBuffer, hFont);
    SetBkMode(hdc_memBuffer, TRANSPARENT);
    SetTextColor(hdc_memBuffer, RGB(255, 255, 255));
    RECT tr = {
        (int)button->position.x,
        (int)button->position.y,
        (int)(button->position.x + button->width),
        (int)(button->position.y + button->height)
    };
    DrawText(hdc_memBuffer, TEXT("设置"), -1, &tr, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // 清理
    SelectObject(hdc_memBuffer, hOld);
    SelectObject(hdc_memBuffer, oldBrush);
    SelectObject(hdc_memBuffer, oldPen);
    DeleteObject(hBrush);
    DeleteObject(hPen);
}
// 点击设置按钮
void OnSettingsButtonClick(Button* button)
{
    Log(1, TEXT("进入设置界面！"));
    ChangeScene(Setting_Scene);
}
void OnHelpButtonClick(Button* button) 
{
    Log(1, TEXT("进入帮助界面！"));
    ChangeScene(Help_Scene);
}

// 渲染帮助按钮（简单矩形 + 文本）
void RenderHelpButton(Button* button, HDC hdc_memBuffer, HDC hdc_loadBmp)
{
    // 背景
    HBRUSH hBrush = CreateSolidBrush(RGB(60, 60, 60));
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc_memBuffer, hBrush);
    HPEN hPen = CreatePen(PS_SOLID, 2, RGB(200, 200, 200));
    HPEN oldPen = (HPEN)SelectObject(hdc_memBuffer, hPen);

    Rectangle(hdc_memBuffer,
        (int)button->position.x, (int)button->position.y,
        (int)(button->position.x + button->width),
        (int)(button->position.y + button->height));

    // 文本
    HFONT hFont = TextUtil::GetCachedFont(18, FW_NORMAL, UI_FONT_NAME);
    HFONT hOld = (HFONT)SelectObject(hdc_memBuffer, hFont);
    SetBkMode(hdc_memBuffer, TRANSPARENT);
    SetTextColor(hdc_memBuffer, RGB(255, 255, 255));
    RECT tr = {
        (int)button->position.x,
        (int)button->position.y,
        (int)(button->position.x + button->width),
        (int)(button->position.y + button->height) };
    DrawText(hdc_memBuffer, TEXT("帮助"), -1, &tr, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // 清理
    SelectObject(hdc_memBuffer, hOld);
    SelectObject(hdc_memBuffer, oldBrush);
    SelectObject(hdc_memBuffer, oldPen);
    DeleteObject(hBrush);
    DeleteObject(hPen);
}

#pragma endregion

#pragma region 碰撞检测
#pragma endregion
