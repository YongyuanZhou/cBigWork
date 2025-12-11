/**
 * 这个文件是获取游戏信息相关函数的头文件
 */

#pragma once

// 打印日志
void Log(int pos, const char *format, ...);
void Log(int pos, const TCHAR *format, ...);

// 游戏时间函数 - 获取当前游戏时间（单位：秒）
double GetGameTime();
// 游戏时间函数 - 获取两帧之间的时间差（单位：秒）
double GetDeltaTime();
