/**
 * 这个文件是场景2-游戏场景的源文件
 * 场景的逻辑实现在这里
 * 务必定义并实现所有的场景核心函数
 */

#include "stdafx.h"

#include "scene.h"
#include "scene2.h"

#include "player.h"
#include "enemy.h"
#include "bullet.h"
#include "mouse.h"    // 新增：用于检测鼠标点击
#include "audio.h"

#include <fstream>
#include <algorithm>
#include <vector>
#include <string>
#include <cstdio>
#include <ctime>
#include <cstdlib>
#include <chrono>

#pragma region 碰撞检测
 // 检查角色和敌人的碰撞
static void CheckCollision_GameScene_Player_Enemies();
// 检查敌人和子弹的碰撞
static void CheckCollision_GameScene_Enemies_Bullets();
#pragma endregion

// 应用词条效果（被选择后立即调用）
static void ApplyPowerUp(int powerIndex)
{
    Player* p = GetPlayer();
    if (!p) return;

    switch (powerIndex)
    {
    case 0: // 生命增加二
        p->attributes.health += 2;
        {
            TCHAR buf[64];
            swprintf_s(buf, _countof(buf), TEXT("已应用：生命+2，当前生命 %d"), p->attributes.health);
            Log(3, buf);
        }
        break;
    case 1: // 子弹射击速度增加（降低最大子弹冷却时间）
    {
        // 把冷却缩短为原来的 75%，并设定最小值上限
        double old = p->attributes.maxBulletCd;
        p->attributes.maxBulletCd *= 0.75;
        if (p->attributes.maxBulletCd < 0.02) p->attributes.maxBulletCd = 0.02;
        TCHAR buf[64];
        swprintf_s(buf, _countof(buf), TEXT("已应用：射速提高（CD %.3f -> %.3f）"), old, p->attributes.maxBulletCd);
        Log(3, buf);
    }
    break;
    case 2: // 玩家移速增加
    {
        double old = p->attributes.speed;
        p->attributes.speed *= 1.25; // 提高25%
        TCHAR buf[64];
        swprintf_s(buf, _countof(buf), TEXT("已应用：移速提高（%.1f -> %.1f）"), old, p->attributes.speed);
        Log(3, buf);
    }
    break;
    case 3: // 5秒无敌时间
    {
        p->attributes.invincible = true;
        p->attributes.invincibleUntil = GetGameTime() + 5.0; // GetGameTime() 返回 s
        Log(3, TEXT("已应用：5秒无敌"));
    }
    break;
    case 4: // 子弹伤害增加
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

// --- 词条状态与标签（本文件内部管理） ---
static bool g_powerActive = false;                         // 是否正在显示/选择词条
static std::vector<int> g_powerOptions;                    // 当前显示的词条索引（大小等于 POWERBOX_COUNT）
static int g_nextScoreThreshold = 50;                      // 下一次触发阈值（每次 +50）
static const TCHAR* g_powerLabels[] = {
    TEXT("生命增加二"),
    TEXT("子弹射击速度增加"),
    TEXT("玩家移速增加"),
    TEXT("5秒无敌时间"),
    TEXT("子弹伤害增加")
};
static bool g_randSeeded = false;
static int g_powerSelected = -1;                           // 被选择的词条索引（-1 表示未选择）
static double g_powerStartTime = 0.0;                      // 词条选择开始时间
static const double g_powerTimeoutSec = 5.0;               // 词条选择超时时间（秒）

// 按键去抖（记录上一次按键状态）
static bool g_keyPrev[POWERBOX_COUNT] = { false };
// 鼠标按下去抖（记录上一帧鼠标左键状态）
static bool g_mousePrevDown = false;

// 将分数追加保存到磁盘（每行一个整数）
static void SaveScore(int score)
{
    std::ofstream ofs(LEADERBOARD_DATA_FILE, std::ios::app);
    if (!ofs.is_open())
    {
        Log(3, TEXT("无法打开排行榜文件，无法保存分数"));
        return;
    }
    ofs << score << '\n';
    ofs.close();
}
// 从磁盘加载分数并返回按降序排列的前若干分数
static std::vector<int> LoadTopScores()
{
    std::vector<int> scores;
    std::ifstream ifs(LEADERBOARD_DATA_FILE);
    if (!ifs.is_open())
    {
        return scores; // 文件不存在或无法打开，返回空
    }
    int s;
    while (ifs >> s)
    {
        scores.push_back(s);
    }
    ifs.close();
    // 按降序排序并截取前 N 项
    std::sort(scores.begin(), scores.end(), std::greater<int>());
    if ((int)scores.size() > LEADERBOARD_MAX_ITEMS)
        scores.resize(LEADERBOARD_MAX_ITEMS);
    return scores;
}

// 触发一次词条选择：随机选取 3 个不同词条，设置为活动并暂停场景
static void TriggerPowerUp()
{
    if (!g_randSeeded)
    {
        srand((unsigned)time(nullptr));
        g_randSeeded = true;
    }

    // 准备候选索引并打乱
    std::vector<int> pool;
    const int total = sizeof(g_powerLabels) / sizeof(g_powerLabels[0]);
    for (int i = 0; i < total; ++i) pool.push_back(i);
    std::random_shuffle(pool.begin(), pool.end()); // C++14 可用

    g_powerOptions.clear();
    for (int i = 0; i < POWERBOX_COUNT && i < (int)pool.size(); ++i)
        g_powerOptions.push_back(pool[i]);

	g_powerActive = true;// 激活词条选择
	g_powerSelected = -1;// 重置选择状态
    // 使用游戏时间（秒）记录开始时间，便于后续和 gameTime 一致判断
    g_powerStartTime = GetGameTime();

    // 将场景置为暂停
//    if (GetCurrentScene())
//        GetCurrentScene()->isPaused = true;
}

void LoadScene_GameScene()
{
    /* UI组件创建 */
    // 游戏场景暂时没有UI组件需要创建
    CreatePlayer();

    // 确保随机数只 seed 一次
    if (!g_randSeeded)
    {
        srand((unsigned)time(nullptr));
        g_randSeeded = true;
    }
    /* 游戏对象创建 */
    // TODO: 创建玩家对象
    // 敌人将在游戏过程中动态创建
    // 子弹将在游戏过程中动态创建
    // TODO: 游戏场景中需要创建的游戏对象
}

void UnloadScene_GameScene()
{
    /* UI组件销毁 */
    // 游戏场景暂时没有UI组件需要销毁
    /* 游戏对象销毁 */
    // TODO: 清空角色对象
    DestroyPlayer();
    // TODO: 清空敌人对象
    DestroyEnemies();
    // TODO: 清空子弹对象
    DestroyBullets();
    // TODO: 游戏场景卸载时需要销毁的游戏对象
}

void ProcessUiInput_GameScene()
{
    // 鼠标点击选择支持：当词条 UI 激活时，检测鼠标左键的按下（从未按->按下表示一次点击）
    if (g_powerActive)
    {
        bool mouseDown = IsMouseLButtonDown();
        if (mouseDown && !g_mousePrevDown)
        {
            // 鼠标单次按下事件：检测位置是否命中任一词条区域
            int mx = GetMouseX();
            int my = GetMouseY();

            // 计算词条区域布局（与 Render 中一致）
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
                    // 点击了第 i 个词条
                    if ((int)g_powerOptions.size() == POWERBOX_COUNT)
                    {
                        g_powerSelected = g_powerOptions[i];
                        g_powerActive = false; // 选择后全部消失
                        // 立即应用词条效果
                        ApplyPowerUp(g_powerSelected);
                        // 日志便于调试
                        TCHAR logbuf[128];
                        swprintf_s(logbuf, _countof(logbuf), TEXT("鼠标选择了词条：%s"), g_powerLabels[g_powerSelected]);
                        Log(3, logbuf);
                    }
                    break;
                }
            }
        }
        g_mousePrevDown = mouseDown;
    }
}

void CheckCollision_GameScene()
{
    // 玩家和敌人的碰撞
    CheckCollision_GameScene_Player_Enemies();
    // 敌人和子弹的碰撞
    CheckCollision_GameScene_Enemies_Bullets();

    // TODO: 更多的碰撞逻辑
}

void UpdateScene_GameScene(double deltaTime)
{
    /* UI组件更新 */
    // 游戏场景暂时没有UI组件需要更新

    /* 游戏对象更新 */
    // 更新角色对象
    UpdatePlayer(deltaTime);
    // 更新敌人对象
    UpdateEnemies(deltaTime);
    // 更新子弹对象
    UpdateBullets(deltaTime);
	// 检查触发词条选择的条件
    Player* p = GetPlayer();
    if (p && !g_powerActive)
    {
        if (p->attributes.score >= g_nextScoreThreshold)
        {
            TriggerPowerUp();
            // 推进下一个阈值（每次 +50）
            g_nextScoreThreshold += 50;
        }
    }
    // 如果词条 UI 激活，检查超时（5s），超时则隐藏 UI（不自动应用任何词条）
    if (g_powerActive)
    {
        // 使用游戏主循环时间 GetGameTime() 判断，避免与系统时钟/steady_clock 混用
        double now = GetGameTime();
        if (now - g_powerStartTime >= g_powerTimeoutSec)
        {
            g_powerActive = false;
            g_powerSelected = -1;
            Log(3, TEXT("词条 UI 超时消失"));
        }
    }
    if (p)
    {
        // 如果玩家处于无敌并且时间到了则恢复
        if (p->attributes.invincible && GetGameTime() >= p->attributes.invincibleUntil)
        {
            p->attributes.invincible = false;
            p->attributes.invincibleUntil = 0.0;
            Log(3, TEXT("无敌效果已结束"));
        }
    }
    // TODO: 游戏场景中需要更新的游戏对象
}

void RenderScene_GameScene(HDC hdc_memBuffer, HDC hdc_loadBmp)
{
    /*
     * 注意绘制顺序，后绘制的会覆盖先绘制的
     * 所以先绘制游戏对象，再绘制UI组件
     */

     /* 游戏对象绘制 */
     // 绘制角色对象
    RenderPlayer(hdc_memBuffer, hdc_loadBmp);
    // 绘制敌人对象
    RenderEnemies(hdc_memBuffer, hdc_loadBmp);
    // 绘制子弹对象
    RenderBullets(hdc_memBuffer, hdc_loadBmp);
    // TODO: 游戏场景中需要渲染的游戏对象

    /* UI组件绘制 */
    // 绘制一个边框表示游戏区域
    // 选择画笔颜色（边框颜色）
    HPEN hPen = CreatePen(PS_SOLID, GAME_BOARDER, RGB(0, 0, 0));
    HGDIOBJ oldPen = SelectObject(hdc_memBuffer, hPen);

    // 使用透明画刷防止填充
    HGDIOBJ oldBrush = SelectObject(hdc_memBuffer, GetStockObject(NULL_BRUSH));

    // 绘制矩形（仅边框）
    Rectangle(hdc_memBuffer, GAME_X, GAME_Y, GAME_WIDTH, GAME_HEIGHT);

    // 还原 GDI 对象
    SelectObject(hdc_memBuffer, oldBrush);
    SelectObject(hdc_memBuffer, oldPen);
    DeleteObject(hPen);

    // 在游戏区域内居中绘制三个词条区域（圆角矩形，显示当前词条或占位文字）
    {
        // 计算布局
        const int boxW = POWERBOX_WIDTH;
        const int boxH = POWERBOX_HEIGHT;
        const int spacing = POWERBOX_SPACING;
        const int totalW = POWERBOX_COUNT * boxW + (POWERBOX_COUNT - 1) * spacing;
        const int startX = GAME_X + (GAME_WIDTH - totalW) / 2;
        const int topY = GAME_Y + POWERBOX_MARGIN_TOP;

        // 准备字体（词条文本）
        HFONT hPowerFont = CreateFont(
            POWERBOX_FONT_HEIGHT, 0, 0, 0, FW_SEMIBOLD,
            FALSE, FALSE, FALSE, DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
            DEFAULT_PITCH | FF_SWISS, UI_FONT_NAME);
        HFONT hOldPowerFont = (HFONT)SelectObject(hdc_memBuffer, hPowerFont);

        // 透明背景以便文字叠在背景上
        int oldBkMode = SetBkMode(hdc_memBuffer, TRANSPARENT);
        COLORREF oldTextColor = SetTextColor(hdc_memBuffer, POWERBOX_TEXT_COLOR);
        for (int i = 0; i < POWERBOX_COUNT; ++i)
        {
            int left = startX + i * (boxW + spacing);
            int right = left + boxW;
            int bottom = topY + boxH;

            // 创建并选择边框笔与填充画刷
            HPEN hBoxPen = CreatePen(PS_SOLID, 2, POWERBOX_BORDER_COLOR);
            HBRUSH hBoxBrush = CreateSolidBrush(POWERBOX_BG_COLOR);
            HGDIOBJ oldBoxPen = SelectObject(hdc_memBuffer, hBoxPen);
            HGDIOBJ oldBoxBrush = SelectObject(hdc_memBuffer, hBoxBrush);

            // 圆角矩形
            RoundRect(hdc_memBuffer, left, topY, right, bottom, POWERBOX_RADIUS, POWERBOX_RADIUS);

            // 绘制文字：如果激活则显示随机词条，否则显示占位文本
            RECT tr = { left, topY, right, bottom };
            if (g_powerActive && (int)g_powerOptions.size() == POWERBOX_COUNT)
            {
                int idx = g_powerOptions[i];
                const TCHAR* txt = g_powerLabels[idx];

                // 显示按键提示（1/2/3）
                TCHAR buf[128];
                swprintf_s(buf, _countof(buf), TEXT("%d. %s"), i + 1, txt);
                DrawText(hdc_memBuffer, buf, -1, &tr, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            }
            else
            {
                // 若已被选择且刚刚选择，可以短暂显示已选择项（可选）
                if (g_powerSelected >= 0 && !g_powerActive)
                {
                    // 显示“已选择：XXX”
                    TCHAR buf[128];
                    swprintf_s(buf, _countof(buf), TEXT("已选择"));
                    DrawText(hdc_memBuffer, buf, -1, &tr, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                }
                else
                {
                    DrawText(hdc_memBuffer, TEXT("待触发"), -1, &tr, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                }
            }


            // 恢复并释放 GDI 对象
            SelectObject(hdc_memBuffer, oldBoxBrush);
            SelectObject(hdc_memBuffer, oldBoxPen);
            DeleteObject(hBoxBrush);
            DeleteObject(hBoxPen);
        }
        // 恢复字体与绘制模式
        SetTextColor(hdc_memBuffer, oldTextColor);
        SetBkMode(hdc_memBuffer, oldBkMode);
        SelectObject(hdc_memBuffer, hOldPowerFont);
        DeleteObject(hPowerFont);
    }

    // 绘制玩家的属性信息
    HFONT hFont = CreateFont(
        UI_FONT_HEIGHT,           // 字体高度
        0,                        // 字体宽度（0 表示自动计算）
        0,                        // 字体的倾斜角度
        0,                        // 字体的基线倾斜角度
        FW_NORMAL,                // 字体的粗细
        FALSE,                    // 是否斜体
        FALSE,                    // 是否下划线
        FALSE,                    // 是否删除线
        DEFAULT_CHARSET,          // 字符集
        OUT_DEFAULT_PRECIS,       // 输出精度
        CLIP_DEFAULT_PRECIS,      // 剪裁精度
        DEFAULT_QUALITY,          // 输出质量
        DEFAULT_PITCH | FF_SWISS, // 字体家族和间距
        UI_FONT_NAME              // 字体名称
    );
    // 选择自定义字体到设备上下文
    HFONT hOldFont = (HFONT)SelectObject(hdc_memBuffer, hFont);
    // 设置字体区域
    const int width = GAME_UI_WIDTH;
    const int height = 200;
    const int left = GAME_WIDTH;
    const int top = 80;
    const int right = left + width;
    const int bottom = top + height;
    RECT rect = { left, top, right, bottom };// 设置文本区域
    // 绘制
    // 安全检查玩家指针，避免空指针崩溃
    Player* p = GetPlayer();
    TCHAR buffer[128];
    if (p)
    {
        swprintf_s(buffer, sizeof(buffer) / sizeof(TCHAR),
            TEXT("生命值: %d\n\n积分: %d"),
            p->attributes.health,
            p->attributes.score);
    }
    else
    {
        swprintf_s(buffer, sizeof(buffer) / sizeof(TCHAR),
            TEXT("生命值: --\n\n积分: --"));
    }

    DrawText(hdc_memBuffer, buffer, -1, &rect, DT_CENTER);
    SelectObject(hdc_memBuffer, hOldFont);
    DeleteObject(hFont);

    // 绘制排行榜
    {
        // 先加载排行榜数据
        std::vector<int> topScores = LoadTopScores();
        // 排行榜字体（与 UI 字体一致）
        HFONT hLbFont = CreateFont(
            UI_FONT_HEIGHT, 0, 0, 0, FW_NORMAL,
            FALSE, FALSE, FALSE, DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
            DEFAULT_PITCH | FF_SWISS, UI_FONT_NAME);
        HFONT hOldLbFont = (HFONT)SelectObject(hdc_memBuffer, hLbFont);
        // 透明背景模式
        int oldBkMode = SetBkMode(hdc_memBuffer, TRANSPARENT);
        // 计算排行榜区域
        int itemHeight = UI_FONT_HEIGHT + LEADERBOARD_ITEM_VSPACING;
        int lbTop = bottom + LEADERBOARD_MARGIN_TOP;
        int lbHeight = itemHeight * (LEADERBOARD_MAX_ITEMS + 1); // +1 用作标题行高度
        RECT lbRect = { left, lbTop, right, lbTop + lbHeight };
        // 构造显示文本（支持缺少数据时显示 "--"）
        TCHAR line1[32], line2[32], line3[32];
        if ((int)topScores.size() > 0)
            swprintf_s(line1, _countof(line1), TEXT("%d"), topScores[0]);
        else
            swprintf_s(line1, _countof(line1), TEXT("--"));
        if ((int)topScores.size() > 1)
            swprintf_s(line2, _countof(line2), TEXT("%d"), topScores[1]);
        else
            swprintf_s(line2, _countof(line2), TEXT("--"));
        if ((int)topScores.size() > 2)
            swprintf_s(line3, _countof(line3), TEXT("%d"), topScores[2]);
        else
            swprintf_s(line3, _countof(line3), TEXT("--"));

        TCHAR lbBuffer[256];
        swprintf_s(lbBuffer, _countof(lbBuffer),
            TEXT("排行榜\n1. %s\n2. %s\n3. %s"),
            line1, line2, line3);
        // 先绘制阴影（偏移），再绘制文字，形成清晰可见的轮廓效果
        RECT lbShadow = lbRect;
        OffsetRect(&lbShadow, 2, 2); // 阴影偏移量，可调整

        SetTextColor(hdc_memBuffer, LEADERBOARD_SHADOW_COLOR); // 阴影颜色
        DrawText(hdc_memBuffer, lbBuffer, -1, &lbShadow, DT_CENTER | DT_TOP);

        SetTextColor(hdc_memBuffer, LEADERBOARD_TEXT_COLOR); // 正文字色
        DrawText(hdc_memBuffer, lbBuffer, -1, &lbRect, DT_CENTER | DT_TOP);

        // 恢复背景模式与字体
        SetBkMode(hdc_memBuffer, oldBkMode);
        SelectObject(hdc_memBuffer, hOldLbFont);
        DeleteObject(hLbFont);
    }

    // 如果处于暂停状态，在游戏区域居中绘制“暂停游戏”提示（修正确保文本可见）
    if (GetCurrentScene() && GetCurrentScene()->isPaused)
    {
        // 创建大字体
        HFONT hPauseFont = CreateFont(
            80, 0, 0, 0, FW_BOLD,
            FALSE, FALSE, FALSE, DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
            DEFAULT_PITCH | FF_SWISS, TEXT("微软雅黑"));
        HFONT hOld = (HFONT)SelectObject(hdc_memBuffer, hPauseFont);

        // 使用透明背景模式以避免背景被填充（关键）
        int oldBkMode = SetBkMode(hdc_memBuffer, TRANSPARENT);

        // 先绘制阴影（偏移），再绘制文字，形成清晰可见的轮廓效果
        RECT rcText = { GAME_X, GAME_Y, GAME_X + GAME_WIDTH, GAME_Y + GAME_HEIGHT };
        RECT rcShadow = rcText;
        OffsetRect(&rcShadow, 2, 2); // 阴影偏移量，可调整

        SetTextColor(hdc_memBuffer, RGB(0, 0, 0)); // 阴影黑色
        DrawText(hdc_memBuffer, TEXT("暂停游戏"), -1, &rcShadow, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        SetTextColor(hdc_memBuffer, RGB(255, 255, 255)); // 正文字白色
        DrawText(hdc_memBuffer, TEXT("暂停游戏"), -1, &rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        // 恢复背景模式与字体
        SetBkMode(hdc_memBuffer, oldBkMode);
        SelectObject(hdc_memBuffer, hOld);
        DeleteObject(hPauseFont);
    }
    // TODO: 游戏场景其他需要绘制的UI组件
}

#pragma region 碰撞检测

// 检查角色和敌人的碰撞
void CheckCollision_GameScene_Player_Enemies()
{
    // 玩家用简单矩形表示
    Player* player = GetPlayer();
    Rect rect1{};
    rect1.left = player->position.x;
    rect1.right = player->position.x + player->width;
    rect1.top = player->position.y;
    rect1.bottom = player->position.y + player->height;
    // 敌人用简单矩形表示
    std::vector<Enemy*> enemies = GetEnemies();
    Rect rect2{};
    for (Enemy* enemy : enemies)
    {
        rect2.left = enemy->position.x;
        rect2.right = enemy->position.x + enemy->width;
        rect2.top = enemy->position.y;
        rect2.bottom = enemy->position.y + enemy->height;
        if (IsRectRectCollision(rect1, rect2))
        {
            // 碰撞后扣血、加分摧毁敌人
            player->attributes.health--;
            player->attributes.score += enemy->attributes.score;
            DestroyEnemy(enemy);
            if (player->attributes.health <= 0)
            {
                // 游戏结束：先保存分数，再切换场景
                SaveScore(player->attributes.score);
                Log(1, TEXT("游戏结束！"));
                ChangeScene(SceneId::StartScene);
            }
        }
    }
    // TODO: 实现玩家和敌人的碰撞检测
}

// 检查敌人和子弹的碰撞
void CheckCollision_GameScene_Enemies_Bullets()
{
    // 敌人用简单矩形表示
    std::vector<Enemy*> enemies = GetEnemies();
    Rect rect{};
    // 子弹用简单圆形表示
    std::vector<Bullet*> bullets = GetBullets();
    Circle circle{};
    // TODO: 实现敌人和子弹的碰撞检测
    for (Enemy* enemy : enemies)
    {
        rect.left = enemy->position.x;
        rect.right = enemy->position.x + enemy->width;
        rect.top = enemy->position.y;
        rect.bottom = enemy->position.y + enemy->height;
        for (Bullet* bullet : bullets)
        {
            circle.center = bullet->position;
            circle.radius = bullet->radius;
            if (IsRectCircleCollision(rect, circle))
            {
                // 碰撞后扣血、加分摧毁敌人和子弹
                enemy->attributes.health -= bullet->damage;
                // 播放撞击或爆炸音效
                if (enemy->attributes.health <= 0)
                {
                    // 播放敌人被摧毁音效（需要 sounds\\explode.wav）
                    PlaySoundEffect(TEXT("sounds\\explode.wav"), false);
                    GetPlayer()->attributes.score += enemy->attributes.score;
                    DestroyEnemy(enemy);
                }
                else
                {
                    // 播放子弹命中音效（需要 sounds\\hit.wav）
                    PlaySoundEffect(TEXT("sounds\\hit.wav"), false);
                }
                DestroyBullet(bullet);
            }
        }
    }
}

#pragma endregion