#pragma once

// 设置场景头文件

void LoadScene_Setting_Scene();
void UnloadScene_Setting_Scene();
void ProcessUiInput_Setting_Scene();
void CheckCollision_Setting_Scene();
void UpdateScene_Setting_Scene(double deltaTime);
void RenderScene_Setting_Scene(HDC hdc_memBuffer, HDC hdc_loadBmp);
