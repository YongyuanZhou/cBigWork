#pragma once

#include <Windows.h>

//初始化词条系统（调用一次即可）
void Power_Init();

//重置/清理词条内部状态（在卸载场景时调用）
void Power_Reset();

//触发词条选择（生成选项并激活UI）
void Power_Trigger();

//渲染词条UI（从场景渲染调用）
void Power_Render(HDC hdc_memBuffer, HDC hdc_loadBmp);

//处理鼠标点击事件（在(mx, my)位置）。返回选中的词条索引（0..n-1）或-1（如果没有选中）。
//调用者负责应用选中的词条效果（如果有）并恢复场景。
int Power_TrySelectByPoint(int mx, int my);

//每帧更新（传入当前游戏时间，单位：秒）。处理超时和内部状态。
void Power_Update(double now);


