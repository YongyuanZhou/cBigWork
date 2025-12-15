/**
 * 这个文件是敌人对象的头文件
 * 敌人对象的逻辑实现在这
 */

#pragma once

struct Enemy
{
    Vector2 position;
    int width;
    int height;

    Attributes attributes;
    // 标记是否为 boss
    bool isBoss;
    // 每个敌人固定的目标偏移（相对于玩家中心），用于避免所有敌人聚集到同一点
    Vector2 targetOffset;
    // 是否已经发射过（当前仍存在）子弹，保证每个敌机最多只有一个子弹
    bool hasBullet;
    // TODO: 加入更多的敌人信息（类型等）
};

// 创建敌人
void CreateEnemy(double x, double y, double difficulty = 1.0);
void CreateRandomEnemy(double difficulty = 1.0);
// 创建 boss
void CreateBoss(double x, double y);
// 摧毁敌人
void DestroyEnemy(Enemy *enemy);
void DestroyEnemies();
// 获取所有敌人
std::vector<Enemy *> GetEnemies();
// 更新所有敌人
void UpdateEnemies(double deltaTime);
// 渲染所有敌人
void RenderEnemies(HDC hdc_memBuffer, HDC hdc_loadBmp);
// 重置敌人系统（销毁所有敌人并重置静态变量）
void ResetEnemySystem();
// 当游戏从暂停恢复时，调整敌人生成/日志计时器，避免因为暂停造成的追赶生成
void Enemy_AdjustTimersForPause(double pauseDuration);