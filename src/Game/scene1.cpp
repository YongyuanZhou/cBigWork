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
// 资源位图句柄
extern HBITMAP bmp_StartButton;
extern HBITMAP bmp_SettingIcon;
extern HBITMAP bmp_HelpIcon;

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
    const int y = 180;
    // TODO: 创建一个在这个位置的按钮
    ButtonId startButtonId = CreateButton(x, y, width, height,
        RenderStartButton, OnStartButtonClick);
    EnableButton(startButtonId);

    // 使用帮助/设置位图的原始像素尺寸创建按钮
    BITMAP bmHelp = { 0 };
    BITMAP bmSet = { 0 };
    int helpW = 140, helpH = 50;   // 回退值，防止位图未加载
    int setW = 140, setH = 50;

    if (bmp_HelpIcon)
    {
        GetObject(bmp_HelpIcon, sizeof(BITMAP), &bmHelp);
        if (bmHelp.bmWidth > 0 && bmHelp.bmHeight > 0) { helpW = bmHelp.bmWidth; helpH = bmHelp.bmHeight; }
    }
    if (bmp_SettingIcon)
    {
        GetObject(bmp_SettingIcon, sizeof(BITMAP), &bmSet);
        if (bmSet.bmWidth > 0 && bmSet.bmHeight > 0) { setW = bmSet.bmWidth; setH = bmSet.bmHeight; }
    }
    // 帮助按钮：紧贴开始按钮正下方，水平居中对齐
    int helpX = x + (width - helpW) / 2;
    int helpY = y + height; // 紧贴，无间隙
    ButtonId helpBtn = CreateButton(helpX, helpY, helpW, helpH,
        RenderHelpButton, OnHelpButtonClick);
    EnableButton(helpBtn);

    // 设置按钮：紧贴在帮助按钮正下方，水平居中对齐
    int setX = x + (width - setW) / 2;
    int setY = helpY + helpH; // 紧贴，无间隙
    ButtonId settingsBtn = CreateButton(setX, setY, setW, setH,
        RenderSettingsButton, OnSettingsButtonClick);
    EnableButton(settingsBtn);

    /* 游戏对象创建 */
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

    /* 标题绘制：在界面上方居中显示 "飞机大战" */
    {
        HFONT hFont = TextUtil::GetCachedFont(90, FW_BOLD, UI_FONT_NAME);
        const int titleW = 600;
        const int titleH = 100;
        const int left = WINDOW_WIDTH / 2 - titleW / 2;
        const int top = 40; // 上方合适位置
        RECT rect = { left-20, top, left + titleW, top + titleH };
        TextUtil::DrawTextEx(hdc_memBuffer, TEXT("飞机大战"), rect, DT_CENTER, hFont, RGB(0, 0, 0), TRANSPARENT);
    }

    /* UI组件绘制 */
    // 绘制所有的按钮
    RenderButtons(hdc_memBuffer, hdc_loadBmp);
}


void RenderStartButton(Button *button, HDC hdc_memBuffer, HDC hdc_loadBmp)
{
    // 绘制开始按钮，确保使用图片真实尺寸作为源尺寸以避免裁剪
    SelectObject(hdc_loadBmp, bmp_StartButton);

    // 获取位图真实尺寸
    BITMAP bm = {0};
    GetObject(bmp_StartButton, sizeof(BITMAP), &bm);
    int srcW = bm.bmWidth > 0 ? bm.bmWidth : button->width;
    int srcH = bm.bmHeight > 0 ? bm.bmHeight : button->height;

    // 使用 TransparentBlt 做缩放绘制，目标为按钮的宽高，源为位图的真实宽高
    TransparentBlt(
        hdc_memBuffer,
        (int)button->position.x, (int)button->position.y,
        button->width, button->height,
        hdc_loadBmp, 0, 0, srcW, srcH,
        RGB(255, 255, 255));
}

// 渲染设置按钮，使用资源位图并缩放到按钮区域
void RenderSettingsButton(Button* button, HDC hdc_memBuffer, HDC hdc_loadBmp)
{
     SelectObject(hdc_loadBmp, bmp_SettingIcon);
     BITMAP bm = {0};
     GetObject(bmp_SettingIcon, sizeof(BITMAP), &bm);
     int srcW = bm.bmWidth > 0 ? bm.bmWidth : button->width;
     int srcH = bm.bmHeight > 0 ? bm.bmHeight : button->height;
     TransparentBlt(
         hdc_memBuffer,
         (int)button->position.x, (int)button->position.y,
         button->width, button->height,
         hdc_loadBmp, 0, 0, srcW, srcH,
     RGB(255, 255, 255));
}

// 渲染帮助按钮，使用资源位图并缩放到按钮区域
void RenderHelpButton(Button* button, HDC hdc_memBuffer, HDC hdc_loadBmp)
{
     SelectObject(hdc_loadBmp, bmp_HelpIcon);
     BITMAP bm = {0};
     GetObject(bmp_HelpIcon, sizeof(BITMAP), &bm);
     int srcW = bm.bmWidth > 0 ? bm.bmWidth : button->width;
     int srcH = bm.bmHeight > 0 ? bm.bmHeight : button->height;
     TransparentBlt(
      hdc_memBuffer,
      (int)button->position.x, (int)button->position.y,
         button->width, button->height,
         hdc_loadBmp, 0, 0, srcW, srcH,
         RGB(255, 255, 255));
}

void OnStartButtonClick(Button *button)
{
    // 切换到游戏场景
    ChangeScene(GameScene);
}
void OnSettingsButtonClick(Button* button)
{
    // 切换到设置场景
    ChangeScene(Setting_Scene);
}
void OnHelpButtonClick(Button* button)
{
    // 切换到帮助场景
    ChangeScene(Help_Scene);
}
#pragma endregion

#pragma region 碰撞检测
#pragma endregion
