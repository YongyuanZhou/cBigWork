#include "stdafx.h"
#include "help_scene.h"
#include "mouse.h"
#include "button.h"

void LoadScene_Help_Scene()
{
}   

void UnloadScene_Help_Scene()
{
}

void ProcessUiInput_Help_Scene()
{
    // 处理帮助场景的鼠标按钮点击
    if (IsMouseLButtonDown())
    {
        PressButtons(GetMouseX(), GetMouseY());
    }
}

void CheckCollision_Help_Scene()
{
}

void UpdateScene_Help_Scene(double deltaTime)
{
}

void RenderScene_Help_Scene(HDC hdc_memBuffer, HDC hdc_loadBmp)
{
}

