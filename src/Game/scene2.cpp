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
#include "ui_text.h" // 新增：文字绘制工具
#include "config.h"
#include "power.h"

#include <fstream>
#include <algorithm>
#include <vector>
#include <string>
#include <cstdio>
#include <ctime>
#include <cstdlib>
#include <chrono>

 // 检查角色和敌人的碰撞
static void CheckCollision_GameScene_Player_Enemies();
// 检查敌人和子弹的碰撞
static void CheckCollision_GameScene_Enemies_Bullets();
// 新增：重置本场景内部静态状态（在加载/卸载时调用）
static void ResetGameSceneStatics();
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
    ofs << score << std::endl; // 使用 endl 确保刷新并写入换行
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
    std::string line;
    while (std::getline(ifs, line))
    {
        // 去除首尾空白
        size_t start = line.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) continue;
        size_t end = line.find_last_not_of(" \t\r\n");
        std::string token = line.substr(start, end - start + 1);
        // 尝试把 token 转为整数，忽略无法解析的行（例如合并冲突标记）
        try
        {
            int s = std::stoi(token);
            scores.push_back(s);
        }
        catch (...) {
            // 忽略非整数行
            continue;
        }
    }
    ifs.close();
    // 按降序排序并截取前 N 项
    std::sort(scores.begin(), scores.end(), std::greater<int>());
    if ((int)scores.size() > LEADERBOARD_MAX_ITEMS)
        scores.resize(LEADERBOARD_MAX_ITEMS);
    return scores;
}

// 加载游戏场景
void LoadScene_GameScene()
{
    /* UI组件创建 */
    // 游戏场景暂时没有UI组件需要创建
    CreatePlayer();
    /* 游戏对象创建 */
    // TODO: 创建玩家对象
    // 敌人将在游戏过程中动态创建
    // 子弹将在游戏过程中动态创建
    // TODO: 游戏场景中需要创建的游戏对象
}

// 卸载游戏场景
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
    // 重置静态变量，防止卸载后残留影响下一场
    ResetGameSceneStatics();
}

// 处理游戏场景中的用户输入
void ProcessUiInput_GameScene()
{
    // 鼠标点击选择支持：当词条 UI 激活时，检测鼠标左键的按下（从未按->按下表示一次点击）
    if (g_powerActive)
    {
        bool mouseDown = IsMouseLButtonDown();
        if (mouseDown && !g_mousePrevDown)
        {
            int mx = GetMouseX();
            int my = GetMouseY();
            int sel = Power_TrySelectByPoint(mx, my);
            // Power_TrySelectByPoint 已在模块内处理应用效果与恢复场景
            (void)sel;
        }
        g_mousePrevDown = mouseDown;
    }
}

// 检查游戏场景中的碰撞
void CheckCollision_GameScene()
{
    // 玩家和敌人的碰撞
    CheckCollision_GameScene_Player_Enemies();
    // 敌人和子弹的碰撞
    CheckCollision_GameScene_Enemies_Bullets();

    // TODO: 更多的碰撞逻辑
}

// 更新游戏场景
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
            Power_Trigger();
            // 推进下一个阈值（每次 +50）
            g_nextScoreThreshold += 50;
        }
    }
    // 交由 power 模块处理超时逻辑
    Power_Update(GetGameTime());
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

// 在内存缓冲区上绘制游戏场景
void RenderScene_GameScene(HDC hdc_memBuffer, HDC hdc_loadBmp)
{
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

    // 使用 power 模块绘制词条区域
    Power_Render(hdc_memBuffer, hdc_loadBmp);

    // 绘制玩家的属性信息
    // 使用点大小创建 DPI 兼容的字体高度（例如 14pt）
    int pointSize = 14;
    int logPixY = GetDeviceCaps(hdc_memBuffer, LOGPIXELSY);
    int lfHeight = -MulDiv(pointSize, logPixY, 72); // 负值表示按字符高度
    HFONT hFont = TextUtil::GetCachedFont(30, FW_NORMAL, TEXT("微软雅黑"));
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

    // 使用工具函数绘制（自动选择/恢复字体）
    TextUtil::DrawTextEx(hdc_memBuffer, buffer, rect, DT_CENTER, hFont, RGB(0, 0, 0), TRANSPARENT);

    // 绘制排行榜
    {
        // 先加载排行榜数据
        std::vector<int> topScores = LoadTopScores();
        // 排行榜字体（与 UI 字体一致）
    // 使用点大小创建 DPI 兼容的字体高度（例如 14pt）
        int pointSize = 14;
        int logPixY = GetDeviceCaps(hdc_memBuffer, LOGPIXELSY);
        int lfHeight = -MulDiv(pointSize, logPixY, 72); // 负值表示按字符高度
        HFONT hLbFont = TextUtil::GetCachedFont(30, FW_NORMAL, TEXT("微软雅黑"));

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

    // 使用工具函数绘制（自动选择/恢复字体）
        TextUtil::DrawTextEx(hdc_memBuffer, buffer, rect, DT_CENTER, hFont, RGB(0, 0, 0), TRANSPARENT);
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
            if(player->attributes.invincible==0)
                player->attributes.health--;
            player->attributes.score += enemy->attributes.score;
            DestroyEnemy(enemy);
            if (player->attributes.health <= 0)
            {
                SaveScore(player->attributes.score);  // 保存分数
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
                }
                DestroyBullet(bullet);
            }
        }
    }
}

static void ResetGameSceneStatics()
{
    // 清理并重置敌机系统（包括清空敌机、重置生成计时器与间隔）
    ResetEnemySystem();
    // 清理子弹
    DestroyBullets();

    // 交由 power 模块重置词条状态
    Power_Reset();

    Log(3, TEXT(""));

    // 随机数种子策略：按需保留或重置（这里重置以便下一次重新 seed）
    g_randSeeded = false;

    // 重置按键/鼠标去抖状态
    for (int i = 0; i < POWERBOX_COUNT; ++i) g_keyPrev[i] = false;
    g_mousePrevDown = false;
}

#pragma endregion