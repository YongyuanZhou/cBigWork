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
