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
    // TODO: 加入更多的敌人信息（类型等）
};

// 创建敌人
void CreateEnemy(double x, double y);
void CreateRandomEnemy();
// 摧毁敌人
void DestroyEnemy(Enemy *enemy);
void DestroyEnemies();
// 获取所有敌人
std::vector<Enemy *> GetEnemies();
// 更新所有敌人
void UpdateEnemies(double deltaTime);
// 渲染所有敌人
void RenderEnemies(HDC hdc_memBuffer, HDC hdc_loadBmp);
