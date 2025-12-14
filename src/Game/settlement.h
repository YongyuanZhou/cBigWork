/**
 * 这个文件是场景5—结算场景的头文件
 * 务必定义并实现所有的场景核心函数
 */

#pragma once

 /* 场景核心函数 - 每个场景必须全部实现 */

 // 加载场景
void LoadScene_Settlement_Scene();

// 卸载场景
void UnloadScene_Settlement_Scene();

// 处理UI输入
void ProcessUiInput_Settlement_Scene();

// 碰撞检测
void CheckCollision_Settlement_Scene();

// 更新场景
void UpdateScene_Settlement_Scene(double deltaTime);

// 渲染场景
void RenderScene_Settlement_Scene(HDC hdc_memBuffer, HDC hdc_loadBmp);

/* 其他场景函数 - 建议用 static 关键字声明并定义在 cpp 函数里  */

// 上一局的分数与存活时间，由游戏场景在结束时填写，结算场景读取并显示
extern int g_lastScore;    // 最后一局的分数
extern double g_lastTime;  // 最后一局的存活时间（秒）