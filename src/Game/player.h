/**
 * 这个文件是玩家对象的头文件
 * 玩家对象的逻辑实现在这
 */

#pragma once

// 玩家
struct Player
{
    Vector2 position;
    int width;
    int height;

    Attributes attributes;
    // 上一帧的位置，用于计算朝向/速度（供敌人行为参考）
    Vector2 lastPosition;
};

// 创建角色
void CreatePlayer();
// 删除角色
void DestroyPlayer();
// 获取角色
Player *GetPlayer();
// 更新角色
void UpdatePlayer(double deltaTime);
// 渲染角色
void RenderPlayer(HDC hdc_memBuffer, HDC hdc_loadBmp);
