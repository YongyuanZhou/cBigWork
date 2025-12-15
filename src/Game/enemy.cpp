/**
 * 这个文件是敌人对象的源文件
 * 敌人对象的逻辑实现在这
 */

#include "stdafx.h"

#include "enemy.h"
#include "scene.h"
#include "settings.h"
#include "player.h" // needed to access player position
#include "util.h"   // for Normalize/Length
#include "bullet.h" // to create enemy bullets

static std::set<Enemy *> enemies;

// 渲染资源
extern HBITMAP bmp_Enemy;
static int frameIndex = 0;
static const int bmp_RowSize = 1;
static const int bmp_ColSize = 1;
static const int bmp_CellWidth = 200;
static const int bmp_CellHeight = 200;

// 难度与生成控制参数（可调）
static const double MIN_GENERATE_TIME = 0.3;      // 最小生成间隔（秒）
static const double DIFFICULTY_SCALE_PER_MIN = 0.25; // 每30s难度增加量（线性）

// 基准生成间隔，可被 ResetEnemySystem 从设置中覆盖
static double BASE_GENERATE_TIME = DIFFICULTY_BASE_INTERVALS[1];

// 刷新时间（以绝对时刻保存），以及局内起始时间（用于计算局内 elapsed）
static double enemyStartTime = 0.0;
static double lastGenerateTime = 0.0;
static double deltaGenerateTime = BASE_GENERATE_TIME;     // 初始生成间隔（秒） 
// 用于节流日志输出（避免每帧打印）
static double lastLogTime = 0.0;

// Boss 生成计时
static double lastBossTime = 0.0;
static const double BOSS_INTERVAL = 30.0; // 每30秒生成一个boss

// 每个敌人的目标偏移范围（像素）
static const double TARGET_OFFSET_RADIUS = 140.0; // 可调整，越大敌人分散越开
// 敌人射击设置
static const double ENEMY_SHOT_COOLDOWN = 1.5; // 每架敌机射击间隔（秒）
static const double ENEMY_BULLET_SPEED_MULT = 1.2; // 敌弹速度为敌机速度的倍数

void CreateEnemy(double x, double y, double difficulty)
{
    Enemy* enemy = new Enemy();// 创建敌人对象
    enemy->position.x = x;// 设置敌人位置
    enemy->position.y = y;// 设置敌人位置
    enemy->width = ENEMY_WIDTH;// 设置敌人宽度
    enemy->height = ENEMY_HEIGHT;// 设置敌人高度
    // 根据难度缩放属性
    enemy->attributes.health = static_cast<int>(std::ceil(ENEMY_DEFAULT_HEALTH * difficulty));
    // 速度按非线性方式适度增加（避免速度飙得太快）
    enemy->attributes.speed = ENEMY_DEFAULT_SPEED * (1.0 + (difficulty - 1.0) * 0.5);
    enemy->attributes.score = static_cast<int>(std::ceil(ENEMY_DEFAULT_SCORE * difficulty));
    enemy->isBoss = false;
    enemy->hasBullet = false;
    // 随机固定目标偏移，使用极坐标采样以均匀分布
    double ang = GetRandomDouble(0.0, 2.0 * 3.14159265358979323846);
    double r = GetRandomDouble(0.0, TARGET_OFFSET_RADIUS);
    enemy->targetOffset.x = cos(ang) * r;
    enemy->targetOffset.y = sin(ang) * r;
    // 使用 attributes.bulletCd 作为射击冷却计时器（重用字段以减少结构修改）
    enemy->attributes.bulletCd = 0.0;
    enemies.insert(enemy);// 将敌人添加到敌人集合中
}

void CreateBoss(double x, double y)
{
    Enemy* boss = new Enemy();
    boss->position.x = x;
    boss->position.y = y;
    boss->width = ENEMY_WIDTH * 3; // 体积三倍
    boss->height = ENEMY_HEIGHT * 3;
    boss->attributes.health = static_cast<int>(std::ceil(ENEMY_DEFAULT_HEALTH * 3.0)); // 生命值三倍
    boss->attributes.speed = ENEMY_DEFAULT_SPEED; // 速度不变
    boss->attributes.score = static_cast<int>(std::ceil(ENEMY_DEFAULT_SCORE * 3.0));
    boss->isBoss = true;
    boss->hasBullet = false;
    // boss 不需要偏移，或者可设小偏移
    boss->targetOffset.x = 0.0;
    boss->targetOffset.y = 0.0;
    boss->attributes.bulletCd = 0.0;
    enemies.insert(boss);
}

void CreateRandomEnemy(double difficulty)
{
    CreateEnemy(
        GetRandomDouble(30, GAME_WIDTH - ENEMY_WIDTH - 30),
        -100 ,// 从上方稍微高一点的位置生成
        difficulty
    );
}

void DestroyEnemy(Enemy *enemy)
{
    enemies.erase(enemy);
    delete enemy;
}

void DestroyEnemies()
{
    for (Enemy *enemy : enemies)
    {
        delete enemy;
    }
    enemies.clear();
}

std::vector<Enemy *> GetEnemies()
{
    // 返回的是敌人指针的副本列表 - 避免边遍历边删除时出错
    return CopyFromSet(enemies);
}

void UpdateEnemies(double deltaTime)
{
    // 难度计算：基于游戏运行时间做线性增长，也可以改为基于分数或波次
    // TODO: 每隔一定时间在随机位置创建一个敌人
    // 使用局内 elapsed 时间计算难度，避免使用进程启动以来的全局时间
    double now = GetGameTime();
    // 从当前场景获取累计暂停时间，排除暂停时长
    double pausedAccum = 0.0;
    Scene* s = GetCurrentScene();
    if (s)
    {
        pausedAccum = s->pausedTimeAccum;
    }
    double elapsed = now - enemyStartTime - pausedAccum; // 局内时间（秒），不包含暂停时间
    if (elapsed < 0.0) elapsed = 0.0;
	double difficulty = 1.0 + (elapsed / 30.0) * DIFFICULTY_SCALE_PER_MIN;// 难度随时间线性增加12.2测试：30.0是一个合适的调节参数 
    if (difficulty < 1.0) difficulty = 1.0;
	deltaGenerateTime = BASE_GENERATE_TIME / difficulty;// 随难度增加生成间隔缩短
    if (deltaGenerateTime < MIN_GENERATE_TIME)
    {
        deltaGenerateTime = MIN_GENERATE_TIME;
    }
    // 生成敌人
    if (now - lastGenerateTime > deltaGenerateTime)
    {
        CreateRandomEnemy(difficulty);
        lastGenerateTime = now;
    }// 每隔 deltaGenerateTime 秒生成一个敌人
    // 每秒输出一次 gameTime 到状态栏（Log 的第 2 区）
    // 使用绝对时间比较 lastLogTime，且在暂停恢复时会通过调整 lastLogTime 避免突发输出
    if (now - lastLogTime >= 1.0)
    {
        // 为避免混淆，使用 elapsed 来输出
        Log(2, "游戏时间gameTime: %.3fs", elapsed);
        lastLogTime = now;
    }

    // 每隔 BOSS_INTERVAL 秒生成一个 boss
    if (now - lastBossTime >= BOSS_INTERVAL)
    {
        // 在上方中央生成 boss
        double bx = (GAME_WIDTH - ENEMY_WIDTH * 3) / 2.0;
        double by = - (ENEMY_HEIGHT * 3) - 50; // 从更高处生成
        CreateBoss(bx, by);
        lastBossTime = now;
    }

    // 敌人的移动逻辑：朝向玩家的前进方向（预测位置），boss 直线向下
    Player* player = GetPlayer();
    const double lookaheadDistance = 200.0; // 预测前方多少像素
    for (Enemy* enemy : GetEnemies())
    {
        // 更新射击冷却
        if (enemy->attributes.bulletCd > 0.0)
        {
            enemy->attributes.bulletCd -= deltaTime;
            if (enemy->attributes.bulletCd < 0.0) enemy->attributes.bulletCd = 0.0;
        }

        Vector2 moveDir = {0, 1}; // 默认向下
        if (!enemy->isBoss && player)
        {
            // 玩家中心（用于偏移计算）
            Vector2 playerCenter = { player->position.x + player->width/2.0, player->position.y + player->height/2.0 };

            // 计算玩家前向向量（当前位置 - 上一帧位置）
            Vector2 playerForward = { player->position.x - player->lastPosition.x, player->position.y - player->lastPosition.y };
            double fLen = Length(playerForward);
            if (fLen < 1e-6)
            {
                // 玩家基本静止，直接朝向玩家中心 + 偏移
                Vector2 target = { playerCenter.x + enemy->targetOffset.x, playerCenter.y + enemy->targetOffset.y };
                Vector2 toPlayer = { target.x - (enemy->position.x + enemy->width/2.0), target.y - (enemy->position.y + enemy->height/2.0) };
                double tLen = Length(toPlayer);
                if (tLen > 1e-6) moveDir = Normalize(toPlayer);
            }
            else
            {
                // 预测玩家前方位置并朝向该点（再加上每个敌人的偏移）
                Vector2 forwardN = Normalize(playerForward);
                Vector2 predictedCenter = { playerCenter.x + forwardN.x * lookaheadDistance, playerCenter.y + forwardN.y * lookaheadDistance };
                Vector2 target = { predictedCenter.x + enemy->targetOffset.x, predictedCenter.y + enemy->targetOffset.y };
                Vector2 toPred = { target.x - (enemy->position.x + enemy->width/2.0), target.y - (enemy->position.y + enemy->height/2.0) };
                double tLen = Length(toPred);
                if (tLen > 1e-6) moveDir = Normalize(toPred);
            }
        }

        // 敌人开火逻辑：如果还没有子弹并且冷却为0且位于可见区域，则发射一枚子弹（向下）
        if (!enemy->hasBullet && enemy->attributes.bulletCd <= 0.0)
        {
            // 只在敌机进入可见区域时才开火，避免生成后立即被销毁
            double spawnY = enemy->position.y + enemy->height + 5.0;
            if (spawnY > - (double)BULLET_RADIUS) // 当弹出口不在屏幕上方太远时允许发射
            {
                Vector2 bdir = {0, 1};
                double bspeed = enemy->attributes.speed * ENEMY_BULLET_SPEED_MULT;
                double bx = enemy->position.x + enemy->width / 2.0;
                double by = spawnY; // 出口在机体下方
                CreateBullet(bx, by, 1, bspeed, bdir, true, enemy);
                enemy->hasBullet = true;
                enemy->attributes.bulletCd = ENEMY_SHOT_COOLDOWN;
            }
        }

        // 禁止向上移动（不允许后退）：将 moveDir.y 截断为 >= MIN_DOWN_COMPONENT，然后重新归一化
        {
            const double MIN_DOWN_COMPONENT = 0.25; // 最小向下分量，保证敌机始终有向下进展
            double mx = moveDir.x;
            double my = moveDir.y;
            if (my < MIN_DOWN_COMPONENT) my = MIN_DOWN_COMPONENT;
            double mlen = sqrt(mx * mx + my * my);
            if (mlen < 1e-6)
            {
                // 如果方向变为零或极小，使用向下移动
                moveDir.x = 0.0;
                moveDir.y = 1.0;
            }
            else
            {
                moveDir.x = mx / mlen;
                moveDir.y = my / mlen;
            }
        }

        // 应用速度移动
        enemy->position.x += moveDir.x * enemy->attributes.speed * deltaTime;
        enemy->position.y += moveDir.y * enemy->attributes.speed * deltaTime;

        // 超出屏幕的敌人删除
        if (enemy->position.y > GAME_HEIGHT + 50 || enemy->position.x < -100 || enemy->position.x > GAME_WIDTH + 100)
        {
            DestroyEnemy(enemy);
        }
    }
}

void RenderEnemies(HDC hdc_memBuffer, HDC hdc_loadBmp)
{
    // TODO: 画出敌人
    const int frameRowIndex = frameIndex / bmp_ColSize;
    const int frameColIndex = frameIndex % bmp_ColSize;
    for (Enemy* enemy : enemies)
    {
        SelectObject(hdc_loadBmp, bmp_Enemy);
        TransparentBlt(
            hdc_memBuffer, (int)enemy->position.x, (int)enemy->position.y,
            enemy->width, enemy->height,
            hdc_loadBmp, frameColIndex * bmp_CellWidth, frameRowIndex *
            bmp_CellHeight, bmp_CellWidth, bmp_CellHeight,
            RGB(255, 255, 255));
    }
}
// 新增：重置敌机系统状态
void ResetEnemySystem()
{
    // 清除当前所有敌机
    DestroyEnemies();
    // 重置生成计时器与间隔、日志时间为初始值
 	enemyStartTime = GetGameTime();
	lastGenerateTime = enemyStartTime;// 重置上次生成时间
	// Initialize from settings API
	BASE_GENERATE_TIME = GetDifficultyBaseInterval();
	deltaGenerateTime = BASE_GENERATE_TIME;// 重置生成间隔
	lastLogTime = enemyStartTime;// 重置日志时间
	lastBossTime = enemyStartTime; // 重置 boss 计时
}

// 在从暂停恢复时，调整内部计时器，避免由于暂停导致的 "时间差" 突然触发多次生成或日志
void Enemy_AdjustTimersForPause(double pauseDuration)
{
    if (pauseDuration <= 0.0) return;
    lastGenerateTime += pauseDuration;
    lastLogTime += pauseDuration;
    lastBossTime += pauseDuration;
}
