/**
 * 这个文件是场景1-开始场景的头文件
 * 务必定义并实现所有的场景核心函数
 */

#pragma once

/* 场景核心函数 - 每个场景必须全部实现 */

// 加载场景
void LoadScene_StartScene();

// 卸载场景
void UnloadScene_StartScene();

// 处理UI输入
void ProcessUiInput_StartScene();

// 碰撞检测
void CheckCollision_StartScene();

// 更新场景
void UpdateScene_StartScene(double deltaTime);

// 渲染场景
void RenderScene_StartScene(HDC hdc_memBuffer, HDC hdc_loadBmp);

/* 其他场景函数 - 建议用 static 关键字声明并定义在 cpp 函数里  */
