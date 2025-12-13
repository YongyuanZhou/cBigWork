#include "stdafx.h"
#include "setting_scene.h"
#include "mouse.h"
#include "button.h"

void LoadScene_Setting_Scene()
{
}

void UnloadScene_Setting_Scene()
{
}

void ProcessUiInput_Setting_Scene()
{
    // 处理帮助场景的鼠标按钮点击
    if (IsMouseLButtonDown())
    {
        PressButtons(GetMouseX(), GetMouseY());
    }
}

void CheckCollision_Setting_Scene()
{
}

void UpdateScene_Setting_Scene(double deltaTime)
{
}

void RenderScene_Setting_Scene(HDC hdc_memBuffer, HDC hdc_loadBmp)
{
}