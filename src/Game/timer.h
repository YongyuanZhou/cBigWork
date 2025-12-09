/**
 * 这个文件是游戏的定时器头文件
 * 除非你想要自己使用新的计时器，否则不需要改这个文件
 */

#pragma once

// TQ Timer
void InitTQTimer(HWND hWnd);
void DeleteTQTimer();

// MM Timer
void InitMMTimer(HWND hWnd);
void DeleteMMTimer();
