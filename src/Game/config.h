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

/* 12.2可调的默认属性（统一管理） */
#define PLAYER_DEFAULT_HEALTH 3           // 玩家初始生命值
#define PLAYER_DEFAULT_SPEED 500.0        // 玩家默认移动速度（像素/秒）
#define PLAYER_BULLET_SPEED 800.0         // 玩家子弹速度（像素/秒）
#define PLAYER_MAX_BULLET_CD 0.1          // 玩家子弹最大冷却时间（秒）

#define ENEMY_DEFAULT_HEALTH 3            // 敌机默认生命值
#define ENEMY_DEFAULT_SPEED 200.0         // 敌机默认移动速度（像素/秒）
#define ENEMY_DEFAULT_SCORE 1             // 击毁敌机获得分数

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

/* 状态栏设置 */
#define UI_FONT_HEIGHT 30                       // 右侧 UI（血量/积分/排行榜）字体高度（与原来一致）
#define UI_FONT_NAME TEXT("微软雅黑")           // UI 字体名称

#define LEADERBOARD_MAX_ITEMS 3                 // 排行榜显示条目数（显示前 N 条）
#define LEADERBOARD_MARGIN_TOP 10               // 排行榜与血量/积分区域的垂直间距（像素）
#define LEADERBOARD_ITEM_VSPACING 6             // 排行榜每项垂直间距（像素）

#define LEADERBOARD_TEXT_COLOR RGB(255,255,255) // 排行榜正文字色（白色）
#define LEADERBOARD_SHADOW_COLOR RGB(0,0,0)     // 排行榜阴影色（黑色）

/* 分数储存文件 */
#define LEADERBOARD_DATA_FILE "scores.txt"

