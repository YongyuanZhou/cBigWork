/**
 * 这个文件是敌人对象的源文件
 * 敌人对象的逻辑实现在这
 */

#include "stdafx.h"

#include "enemy.h"

static std::set<Enemy *> enemies;

// 渲染资源
extern HBITMAP bmp_Enemy;
static int frameIndex = 0;
static const int bmp_RowSize = 1;
static const int bmp_ColSize = 1;
static const int bmp_CellWidth = 200;
static const int bmp_CellHeight = 200;

// 难度与生成控制参数（可调）
static const double BASE_GENERATE_TIME = 2.0;     // 初始生成间隔（秒）
static const double MIN_GENERATE_TIME = 0.3;      // 最小生成间隔（秒）
static const double DIFFICULTY_SCALE_PER_MIN = 0.25; // 每分钟难度增加量（线性）
// 刷新时间
static double lastGenerateTime = 0;
static double deltaGenerateTime = 2;
// 用于节流日志输出（避免每帧打印）
static double lastLogTime = 0.0;

void CreateEnemy(double x, double y, double difficulty = 1.0)
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
    enemies.insert(enemy);// 将敌人添加到敌人集合中
}

void CreateRandomEnemy(double difficulty = 1.0)
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
    double gameTime = GetGameTime();
	double difficulty = 1.0 + (gameTime / 30.0) * DIFFICULTY_SCALE_PER_MIN;// 难度随时间线性增加12.2测试：30.0是一个合适的调节参数 
    if (difficulty < 1.0) difficulty = 1.0;
    // 根据 difficulty 调整生成间隔（interval = base / difficulty），并限制最小值
    deltaGenerateTime = BASE_GENERATE_TIME / difficulty;
    if (deltaGenerateTime < MIN_GENERATE_TIME)
    {
        deltaGenerateTime = MIN_GENERATE_TIME;
    }
    // 生成敌人
    if (gameTime - lastGenerateTime > deltaGenerateTime)
    {
        CreateRandomEnemy(difficulty);
        lastGenerateTime = gameTime;
    }// 每隔 deltaGenerateTime 秒生成一个敌人
    // 每秒输出一次 gameTime 到状态栏（Log 的第 0 区）
    // 使用 char* 版本的 Log 以保证浮点格式能正确被格式化
    if (gameTime - lastLogTime >= 1.0)
    {
        Log(2, "gameTime: %.3fs", gameTime);
        lastLogTime = gameTime;
    }
    // TODO: 敌人的移动逻辑
    for (Enemy* enemy : GetEnemies())
    {
        // 敌人向下移动
        enemy->position.y += enemy->attributes.speed * deltaTime;
        // 超出屏幕的敌人删除
        if (enemy->position.y > GAME_HEIGHT + 50)
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
