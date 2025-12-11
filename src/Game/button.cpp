/**
 * 这个文件是按钮的源文件
 * 按钮的管理实现这，按钮的逻辑实现在对应的场景文件中
 */

#include "stdafx.h"

#include "button.h"

static std::set<Button *> buttons;

ButtonId CreateButton(double x, double y, int width, int height, RenderButtonFunc render, OnButtonClickFunc onClick)
{
    Button *button = new Button();

    button->position.x = x;
    button->position.y = y;
    button->width = width;
    button->height = height;

    button->isEnabled = false;

    button->render = render;
    button->onClick = onClick;

    buttons.insert(button);

    return reinterpret_cast<ButtonId>(button);
}

void DestroyButton(ButtonId buttonId)
{
    if (Button *button = GetButton(buttonId))
    {
        buttons.erase(button);
        delete button;
    }
}

void DestroyButtons()
{
    for (Button *button : buttons)
    {
        delete button;
    }
    buttons.clear();
}

std::vector<Button *> GetButtons()
{
    // 返回的是按钮指针的副本列表 - 避免边遍历边删除时出错
    return CopyFromSet(buttons);
}

Button *GetButton(ButtonId buttonId)
{
    Button *button = reinterpret_cast<Button *>(buttonId);
    return buttons.count(button) ? button : nullptr;
}

void EnableButton(ButtonId buttonId)
{
    if (Button *button = GetButton(buttonId))
    {
        button->isEnabled = true;
    }
}

void DisableButton(ButtonId buttonId)
{
    if (Button *button = GetButton(buttonId))
    {
        button->isEnabled = false;
    }
}

void PressButtons(int mouseX, int mouseY)
{
    // 注意：如果有按钮重叠，点击结果是不确定的！
    Button *pressedButton = nullptr;
    for (Button *button : buttons)
    {
        if (!button->isEnabled)
        {
            continue;
        }
        if (button->position.x <= mouseX &&
            button->position.x + button->width >= mouseX &&
            button->position.y <= mouseY &&
            button->position.y + button->height >= mouseY)
        {
            pressedButton = button;
            break;
        }
    }
    if (pressedButton)
    {
        pressedButton->onClick(pressedButton);
    }
}

void RenderButtons(HDC hdc_memBuffer, HDC hdc_loadBmp)
{
    for (Button *button : buttons)
    {
        if (!button->isEnabled)
        {
            continue;
        }
        button->render(button, hdc_memBuffer, hdc_loadBmp);
    }
}

