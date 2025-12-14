#include "stdafx.h"

#include "power.h"
#include "config.h"
#include "util.h"
#include "player.h"
#include "scene.h"
#include "audio.h"
#include "info.h" // for Log

#include <vector>
#include <algorithm>
#include <tchar.h>

// Internal state definitions (previously in config.h as static)
bool g_powerActive = false;
std::vector<int> g_powerOptions;
int g_nextScoreThreshold = DEFAULT_NEXT_SCORE_THRESHOLD;
const TCHAR* g_powerLabels[] = {
    TEXT("生命增加二"),
    TEXT("子弹射击速度增加"),
    TEXT("玩家移速增加"),
    TEXT("5秒无敌时间"),
    TEXT("子弹伤害增加")
};
bool g_randSeeded = false;
int g_powerSelected = -1;
double g_powerStartTime = 0.0;
const double g_powerTimeoutSec = 5.0;

// mouse debounce
static bool s_mousePrevDown = false;

void Power_Init()
{
    // currently nothing to init
}

void Power_Reset()
{
    g_powerActive = false;
    g_powerOptions.clear();
    g_powerSelected = -1;
    g_powerStartTime = 0.0;
    g_randSeeded = false;
    s_mousePrevDown = false;
    // 重置触发阈值为初始值，防止新一局继续沿用上一局阈值
    g_nextScoreThreshold = DEFAULT_NEXT_SCORE_THRESHOLD;
}

void Power_Trigger()
{
	//功能触发时随机选择词条选项
    std::vector<int> pool;
    const int total = sizeof(g_powerLabels) / sizeof(g_powerLabels[0]);
    for (int i = 0; i < total; ++i) pool.push_back(i);
    ShuffleIntVector(pool);

    g_powerOptions.clear();
    for (int i = 0; i < POWERBOX_COUNT && i < (int)pool.size(); ++i)
        g_powerOptions.push_back(pool[i]);

    g_powerActive = true;
    g_powerSelected = -1;
    g_powerStartTime = GetGameTime();

    Scene* cur = GetCurrentScene();
    if (cur) cur->isPaused = true;

    s_mousePrevDown = false;
}

int Power_TrySelectByPoint(int mx, int my)
{
    if (!g_powerActive) return -1;

    // compute layout
    const int boxW = POWERBOX_WIDTH;
    const int boxH = POWERBOX_HEIGHT;
    const int spacing = POWERBOX_SPACING;
    const int totalW = POWERBOX_COUNT * boxW + (POWERBOX_COUNT - 1) * spacing;
    const int startX = GAME_X + (GAME_WIDTH - totalW) / 2;
    const int topY = GAME_Y + POWERBOX_MARGIN_TOP;

    for (int i = 0; i < POWERBOX_COUNT; ++i)
    {
        int left = startX + i * (boxW + spacing);
        int right = left + boxW;
        int bottom = topY + boxH;
        if (mx >= left && mx <= right && my >= topY && my <= bottom)
        {
            if ((int)g_powerOptions.size() == POWERBOX_COUNT)
            {
                g_powerSelected = g_powerOptions[i];
                g_powerActive = false;
                // apply effect here
                Player* p = GetPlayer();
                if (p)
                {
                    switch (g_powerSelected)
                    {
                    case POWERUP_HEALTH_UP:
                        p->attributes.health += 2;
                        {
                            TCHAR buf[64];
                            swprintf_s(buf, _countof(buf), TEXT("已应用：生命+2，当前生命 %d"), p->attributes.health);
                            Log(3, buf);
                        }
                        break;
                    case POWERUP_BULLET_SPEED_UP:
                        {
                            double old = p->attributes.maxBulletCd;
                            p->attributes.maxBulletCd *= 0.75;
                            if (p->attributes.maxBulletCd < 0.02) p->attributes.maxBulletCd = 0.02;
                            TCHAR buf[64];
                            swprintf_s(buf, _countof(buf), TEXT("已应用：射速提高（CD %.3f -> %.3f）"), old, p->attributes.maxBulletCd);
                            Log(3, buf);
                        }
                        break;
                    case POWERUP_PLAYER_SPEED_UP:
                        {
                            double old = p->attributes.speed;
                            p->attributes.speed *= 1.25;
                            TCHAR buf[64];
                            swprintf_s(buf, _countof(buf), TEXT("已应用：移速提高（%.1f -> %.1f）"), old, p->attributes.speed);
                            Log(3, buf);
                        }
                        break;
                    case POWERUP_INVINCIBILITY:
                        p->attributes.invincible = true;
                        p->attributes.invincibleUntil = GetGameTime() + 5.0;
                        Log(3, TEXT("已应用：5秒无敌"));
                        break;
                    case POWERUP_BULLET_DAMAGE_UP:
                        {
                            int old = p->attributes.bulletDamage;
                            p->attributes.bulletDamage += 1;
                            TCHAR buf[64];
                            swprintf_s(buf, _countof(buf), TEXT("已应用：子弹伤害增加（%d -> %d）"), old, p->attributes.bulletDamage);
                            Log(3, buf);
                        }
                        break;
                    default:
                        break;
                    }
                }

                // resume scene
                Scene* cur = GetCurrentScene();
                if (cur) cur->isPaused = false;

                // log selected label
                if (g_powerSelected >= 0)
                {
                    TCHAR logbuf[128];
                    swprintf_s(logbuf, _countof(logbuf), TEXT("鼠标选择了词条：%s"), g_powerLabels[g_powerSelected]);
                    Log(3, logbuf);
                }

                return g_powerSelected;
            }
            break;
        }
    }
    return -1;
}

void Power_Update(double now)
{
    if (!g_powerActive) return;
    if (now - g_powerStartTime >= g_powerTimeoutSec)
    {
        g_powerActive = false;
        g_powerSelected = -1;
        Log(3, TEXT("词条 UI 超时消失"));
        // resume scene when timed out
        Scene* cur = GetCurrentScene();
        if (cur) cur->isPaused = false;
    }
}

void Power_Render(HDC hdc_memBuffer, HDC hdc_loadBmp)
{
    if (g_powerActive && (int)g_powerOptions.size() == POWERBOX_COUNT)
    {
		// 计算布局
        const int boxW = POWERBOX_WIDTH;
        const int boxH = POWERBOX_HEIGHT;
        const int spacing = POWERBOX_SPACING;
        const int totalW = POWERBOX_COUNT * boxW + (POWERBOX_COUNT - 1) * spacing;
        const int startX = GAME_X + (GAME_WIDTH - totalW) / 2;
        const int topY = GAME_Y + POWERBOX_MARGIN_TOP;

		// 字体设置
        HFONT hPowerFont = CreateFont(
            POWERBOX_FONT_HEIGHT, 0, 0, 0, FW_SEMIBOLD,
            FALSE, FALSE, FALSE, DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
            DEFAULT_PITCH | FF_SWISS, UI_FONT_NAME);
        HFONT hOldPowerFont = (HFONT)SelectObject(hdc_memBuffer, hPowerFont);

        int oldBkMode = SetBkMode(hdc_memBuffer, TRANSPARENT);
        COLORREF oldTextColor = SetTextColor(hdc_memBuffer, POWERBOX_TEXT_COLOR);

        for (int i = 0; i < POWERBOX_COUNT; ++i)
        {
            int left = startX + i * (boxW + spacing);
            int right = left + boxW;
            int bottom = topY + boxH;

            HPEN hBoxPen = CreatePen(PS_SOLID, 2, POWERBOX_BORDER_COLOR);
            HBRUSH hBoxBrush = CreateSolidBrush(POWERBOX_BG_COLOR);
            HGDIOBJ oldBoxPen = SelectObject(hdc_memBuffer, hBoxPen);
            HGDIOBJ oldBoxBrush = SelectObject(hdc_memBuffer, hBoxBrush);

            RoundRect(hdc_memBuffer, left, topY, right, bottom, POWERBOX_RADIUS, POWERBOX_RADIUS);

            RECT tr = { left, topY, right, bottom };
            int idx = g_powerOptions[i];
            const TCHAR* txt = g_powerLabels[idx];
            TCHAR buf[128];
            swprintf_s(buf, _countof(buf), TEXT("%d. %s"), i + 1, txt);
            DrawText(hdc_memBuffer, buf, -1, &tr, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            SelectObject(hdc_memBuffer, oldBoxBrush);
            SelectObject(hdc_memBuffer, oldBoxPen);
            DeleteObject(hBoxBrush);
            DeleteObject(hBoxPen);
        }
        SetTextColor(hdc_memBuffer, oldTextColor);
        SetBkMode(hdc_memBuffer, oldBkMode);
        SelectObject(hdc_memBuffer, hOldPowerFont);
        DeleteObject(hPowerFont);
    }
}
