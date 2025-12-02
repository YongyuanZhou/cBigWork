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

// 刷新时间
static double lastGenerateTime = 0;
static double deltaGenerateTime = 2;

void CreateEnemy(double x, double y)
{
    Enemy* enemy = new Enemy();// 创建敌人对象
    enemy->position.x = x;// 设置敌人位置
    enemy->position.y = y;// 设置敌人位置
    enemy->width = ENEMY_WIDTH;// 设置敌人宽度
    enemy->height = ENEMY_HEIGHT;// 设置敌人高度
    enemy->attributes.health = ENEMY_DEFAULT_HEALTH;// 设置敌人生命值
    enemy->attributes.speed = ENEMY_DEFAULT_SPEED;// 设置敌人速度
    enemy->attributes.score = ENEMY_DEFAULT_SCORE;// 设置敌人分数
    enemies.insert(enemy);// 将敌人添加到敌人集合中
}

void CreateRandomEnemy()
{
    CreateEnemy(
        GetRandomDouble(30, GAME_WIDTH - ENEMY_WIDTH - 30),
        -100 // 从上方稍微高一点的位置生成
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
    // TODO: 每隔一定时间在随机位置创建一个敌人
    double gameTime = GetGameTime();
    if (gameTime - lastGenerateTime > deltaGenerateTime)
    {
        CreateRandomEnemy();
        lastGenerateTime = gameTime;
	}// 每隔 deltaGenerateTime 秒生成一个敌人
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
