/**
 * 这个文件是玩家对象的源文件
 * 玩家对象的逻辑实现在这
 */

#include "stdafx.h"

#include "keyboard.h"

#include "player.h"
#include "bullet.h"

static Player *player;

// 渲染资源
extern HBITMAP bmp_Player;
static int frameIndex = 0;
static const int bmp_RowSize = 1;
static const int bmp_ColSize = 4;
static const int bmp_CellWidth = 200;
static const int bmp_CellHeight = 300;

void CreatePlayer()
{
    player = new Player();
    // 玩家初始位置
    player->position.x = (GAME_WIDTH - PLAYER_WIDTH) / 2;
    player->position.y = GAME_HEIGHT - PLAYER_HEIGHT - 20;
    player->width = PLAYER_WIDTH;
    player->height = PLAYER_HEIGHT;
    // 玩家初始属性
    player->attributes.health = 3;
    player->attributes.score = 0;
    player->attributes.speed = 500;
    player->attributes.maxBulletCd = 0.1;
    player->attributes.bulletCd = 0.0;
	//可以补充其他属性
}

void DestroyPlayer()
{
    delete player;
    player = nullptr;
}

Player *GetPlayer()
{
    return player;
}

void UpdatePlayer(double deltaTime)
{
    // TODO: 取键盘输入，然后控制角色位置（归一化方向向量，保证所有方向移动速度一致）
    Vector2 direction = { 0, 0 };
    if (GetKeyDown(VK_W) || GetKeyDown(VK_UP))
    {
        direction.y -= 1;
    }
    if (GetKeyDown(VK_S) || GetKeyDown(VK_DOWN))
    {
        direction.y += 1;
    }
    if (GetKeyDown(VK_A) || GetKeyDown(VK_LEFT))
    {
        direction.x -= 1;
    }
    if (GetKeyDown(VK_D) || GetKeyDown(VK_RIGHT))
    {
        direction.x += 1;
    }
    // 归一化方向向量，保证所有方向移动速度一致
    direction = Normalize(direction);
    player->position.x += direction.x * player->attributes.speed * deltaTime;
    player->position.y += direction.y * player->attributes.speed * deltaTime;

    // TODO: 限制角色在屏幕内
    if (player->position.x < 0)
    {
        player->position.x = 0;
    }
    if (player->position.x > GAME_WIDTH - player->width)
    {
        player->position.x = GAME_WIDTH - player->width;
    }
    if (player->position.y < 0)
    {
        player->position.y = 0;
    }
    if (player->position.y > GAME_HEIGHT - player->height)
    {
        player->position.y = GAME_HEIGHT - player->height;
    }
    // TODO: 发射子弹

    // TODO: 更新角色帧动画（假设1s播放完全部的动画）
    frameIndex = (int)(GetGameTime() * bmp_RowSize * bmp_ColSize) % (bmp_RowSize * bmp_ColSize);
    // TODO: 更多的角色逻辑
    // 发射子弹
    //if (GetKeyDown(VK_SPACE))
    //{
    //    // 创建子弹，子弹从飞机顶部中央位置发射
    //    CreateBullet(
    //        player->position.x + player->width / 2.0,
    //        player->position.y,
    //        1, // 伤害
    //        800.0 // 速度
    //    );
    //}
    // 发射子弹
    if (GetKeyDown(VK_SPACE))
    {
        // 控制子弹发射间隔
        if (player->attributes.bulletCd <= 0.0)
        {
            // 创建子弹，子弹从飞机顶部中央位置发射
            CreateBullet(
                player->position.x + player->width / 2.0,
                player->position.y,
                1, // 伤害
                800.0 // 速度
            );
            // 0.3秒发射一次
            player->attributes.bulletCd = player->attributes.maxBulletCd;
        }
        else
        {
            player->attributes.bulletCd -= deltaTime;
        }
    }
}

void RenderPlayer(HDC hdc_memBuffer, HDC hdc_loadBmp)
{
    // TODO: 绘制玩家
    SelectObject(hdc_loadBmp, bmp_Player);
    const int frameRowIndex = frameIndex / bmp_ColSize;
    const int frameColIndex = frameIndex % bmp_ColSize;
    TransparentBlt(
        hdc_memBuffer, (int)player->position.x, (int)player->position.y,
        player->width, player->height,
        hdc_loadBmp, frameColIndex * bmp_CellWidth, frameRowIndex *
        bmp_CellHeight, bmp_CellWidth, bmp_CellHeight,
        RGB(255, 255, 255));
}
