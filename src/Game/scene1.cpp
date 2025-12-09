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

#pragma region 按钮逻辑
static void RenderStartButton(Button *button, HDC hdc_memBuffer, HDC hdc_loadBmp);
static void OnStartButtonClick(Button *button);
#pragma endregion

#pragma region 碰撞检测
#pragma endregion

void LoadScene_StartScene()
{
    /* UI组件创建 */
    // 创建按钮
    const int width = 300;
    const int height = 200;
    const int x = (WINDOW_WIDTH - width) / 2 - 10; // what the f**k offset to center this?
    const int y = 196;
    // TODO: 创建一个在这个位置的按钮
    ButtonId startButtonId = CreateButton(x, y, width, height,
        RenderStartButton, OnStartButtonClick);
    EnableButton(startButtonId);
    /* 游戏对象创建 */
    // 开始场景暂时没有游戏对象需要创建
}

void UnloadScene_StartScene()
{
    /* UI组件销毁 */
    // TODO: 销毁所有按钮
    DestroyButtons();
    /* 游戏对象销毁 */
    // 开始场景暂时没有游戏对象需要销毁
}

void ProcessUiInput_StartScene()
{
    // TODO: 处理鼠标点击按钮
    // TODO: 处理鼠标点击按钮
    if (IsMouseLButtonDown())
    {
        PressButtons(GetMouseX(), GetMouseY());
    }
}

void CheckCollision_StartScene()
{
    // 开始场景暂时没有游戏对象需要碰撞检测
}

void UpdateScene_StartScene(double deltaTime)
{
    /* UI组件更新 */
    // 开始场景暂时没有UI组件需要更新

    /* 游戏对象更新 */
    // 开始场景暂时没有游戏对象需要更新
}

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

    // 绘制提示文字
    // 创建一个字体，设置高度为 30（单位是逻辑单元）
    HFONT hFont = CreateFont(
        30,                       // 字体高度
        0,                        // 字体宽度（0 表示自动计算）
        0,                        // 字体的倾斜角度
        0,                        // 字体的基线倾斜角度
        FW_NORMAL,                // 字体的粗细
        FALSE,                    // 是否斜体
        FALSE,                    // 是否下划线
        FALSE,                    // 是否删除线
        DEFAULT_CHARSET,          // 字符集
        OUT_DEFAULT_PRECIS,       // 输出精度
        CLIP_DEFAULT_PRECIS,      // 剪裁精度
        DEFAULT_QUALITY,          // 输出质量
        DEFAULT_PITCH | FF_SWISS, // 字体家族和间距
        TEXT("微软雅黑")          // 字体名称
    );
    // 选择自定义字体到设备上下文
    HFONT hOldFont = (HFONT)SelectObject(hdc_memBuffer, hFont);
    // 设置字体区域
    const int width = 800;
    const int height = 300;
    const int left = WINDOW_WIDTH / 2 - width / 2 - 10;
    const int top = 456;
    const int right = left + width;
    const int bottom = top + height;
    RECT rect = {left, top, right, bottom};
    // 绘制
    DrawText(hdc_memBuffer, TEXT("使用WASD或方向键控制飞机移动\n使用空格发射子弹\n\n请大家好好学习这个框架_(:зゝ∠)_"), -1, &rect, DT_CENTER);
    // 恢复原来的字体
    SelectObject(hdc_memBuffer, hOldFont);
    // 删除自定义字体以释放资源
    DeleteObject(hFont);

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

#pragma endregion

#pragma region 碰撞检测
#pragma endregion
