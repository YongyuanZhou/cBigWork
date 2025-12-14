/**
 * 这个文件是键盘输入的源文件
 * 如果要添加新的键盘功能，修改在这里
 */

#include "stdafx.h"

#include "keyboard.h"
#include "scene.h"
#include "enemy.h"

static std::vector<bool> keyboard(128, 0);

// 键盘按下事件处理函数
void KeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if (wParam > 127)
    {
        return;
    }
    keyboard[wParam] = true;
}

// 键盘松开事件处理函数
void KeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if (wParam > 127)
    {
        return;
    }
    keyboard[wParam] = false;
    //暂停功能核心修改： 使用 ESC 切换暂停（在按键松开时切换可避免按住时反复触发）
    if (wParam == VK_ESCAPE)
    {
        Scene* s = GetCurrentScene();
        if (s)
        {
            // 切换暂停状态
            bool newPaused = !s->isPaused;
            s->isPaused = newPaused;
            double now = GetGameTime();
            if (newPaused)
            {
                // 记录暂停开始时刻（绝对 game time）
                s->pauseStartTime = now;
            }
            else
            {
                // 恢复：计算这次暂停持续时间并累加
                if (s->pauseStartTime >= 0.0)
                {
                    double pauseDuration = now - s->pauseStartTime;
                    if (pauseDuration > 0.0)
                    {
                        s->pausedTimeAccum += pauseDuration;
                        // 调整敌人模块内部计时器，避免由于暂停导致的突增
                        Enemy_AdjustTimersForPause(pauseDuration);
                    }
                }
                s->pauseStartTime = -1.0;
            }
        }
    }
}

// 获取按键是否按下
bool GetKeyDown(int keycode)
{
    if (keycode > 127)
    {
        return false;
    }
    return keyboard[keycode];
}
