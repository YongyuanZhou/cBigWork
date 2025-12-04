/**
 * 这个文件是游戏核心逻辑的头文件
 * 除非你想要修改游戏框架，否则不需要改这个文件
 */

#pragma once

/* 游戏初始化函数 */
void GameInit(HWND hWnd, WPARAM wParam, LPARAM lParam);
void GameResourceInit(HWND hWnd, WPARAM wParam, LPARAM lParam);

/* 游戏主循环函数 */
void GameLoop(HWND hWnd);

/* 游戏渲染函数 */
void GameRender(HWND hWnd, WPARAM wParam, LPARAM lParam);
