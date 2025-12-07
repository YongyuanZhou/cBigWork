/**
 * 这个文件是按钮的头文件
 * 按钮的管理实现这，按钮的逻辑实现在对应的场景文件中
 */

#pragma once

// 这真的是纯C语言的内容
struct Button;
typedef size_t ButtonId;
typedef void (*RenderButtonFunc)(Button *button, HDC hdc_memBuffer, HDC hdc_loadBmp);
typedef void (*OnButtonClickFunc)(Button *button);

struct Button
{
    Vector2 position; // 坐标 (x, y)
    int width;        // 宽度
    int height;       // 高度

    bool isEnabled; // 是否激活

    RenderButtonFunc render;
    OnButtonClickFunc onClick;
};

// 创建按钮（注意：按钮创建时处于禁用状态）
ButtonId CreateButton(double x, double y, int width, int height, RenderButtonFunc render, OnButtonClickFunc onClick);
// 销毁按钮
void DestroyButton(ButtonId buttonId);
void DestroyButtons();
// 获取按钮
Button *GetButton(ButtonId buttonId);
std::vector<Button *> GetButtons();
// 启用按钮
void EnableButton(ButtonId buttonId);
// 禁用按钮
void DisableButton(ButtonId buttonId); // 如果禁用按钮，就相当于按钮被隐藏了
// 点击按钮
void PressButtons(int mouseX, int mouseY);
// 渲染按钮
void RenderButtons(HDC hdc_memBuffer, HDC hdc_loadBmp);
