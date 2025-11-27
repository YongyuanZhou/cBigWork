/**
 * 这个文件是游戏配置文件
 * 可以把需要调整的游戏参数放在这里，方便统一管理
 */

#pragma once

/* 游戏窗口位置与大小 */
#define WINDOW_X 150      // 游戏窗口位置X坐标
#define WINDOW_Y 80       // 游戏窗口位置Y坐标
#define WINDOW_WIDTH 1000 // 游戏窗口宽度
#define WINDOW_HEIGHT 800 // 游戏窗口高度

/* UI参数 */
#define GAME_BOARDER 10                                        // 游戏场景左边区域的边框宽度
#define GAME_UI_WIDTH 300                                      // 游戏场景右边UI的宽度
#define GAME_X GAME_BOARDER / 2                                // 游戏场景左边区域的左上角X坐标
#define GAME_Y GAME_BOARDER / 2                                // 游戏场景左边区域的左上角Y坐标
#define GAME_WIDTH WINDOW_WIDTH - GAME_UI_WIDTH - GAME_BOARDER // 游戏场景左边区域的宽度
#define GAME_HEIGHT WINDOW_HEIGHT - GAME_BOARDER / 2 + 1       // 游戏场景左边区域的高度

/* 对象参数 */
#define PLAYER_WIDTH 100
#define PLAYER_HEIGHT PLAYER_WIDTH / 2 * 3
#define ENEMY_WIDTH 80
#define ENEMY_HEIGHT ENEMY_WIDTH
#define BULLET_RADIUS 8

/* 状态栏 */
constexpr const int statusBarParts[3] = {150, 300, -1}; // 状态栏分栏，第一个分区宽度100，第二个占满剩余

/* 游戏内参数 */
#define FPS 100 // 游戏运行帧率

/* 使用的计时器 */
#define MAIN_TIMER_ID 1
#define TIMER_WM_TIMER 0
#define TIMER_TQ_TIMER 1
#define TIMER_MM_TIMER 2
#define TIMER_USE TIMER_MM_TIMER
