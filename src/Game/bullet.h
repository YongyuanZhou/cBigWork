/**
 * 这个文件是子弹对象的头文件
 * 子弹对象的逻辑实现在这
 */

#pragma once

struct Enemy; // forward declare

struct Bullet
{
	Vector2 position;
	int radius;
	int damage;
	double speed;
	// 运动方向（单位向量）
	Vector2 direction;
	// 是否为敌方子弹
	bool isEnemy;
	// 发射者（可为空），用于在子弹销毁时通知敌机重置 hasBullet
	Enemy* owner;
};// 子弹结构体，包含位置、半径、伤害和速度等属性

// 创建子弹
// 兼容旧调用：默认 direction = {0,-1}（向上），isEnemy=false, owner=nullptr
void CreateBullet(double x, double y, int damage, double speed, Vector2 direction = {0,-1}, bool isEnemy = false, Enemy* owner = nullptr);// 创建一个新的子弹对象
// 摧毁子弹
void DestroyBullet(Bullet* bullet);// 摧毁指定的子弹对象
void DestroyBullets();// 摧毁所有子弹对象
// 获取所有子弹
std::vector<Bullet*> GetBullets();// 获取当前所有子弹对象的列表
// 更新所有子弹
void UpdateBullets(double deltaTime);// 更新所有子弹的位置和状态
// 渲染所有子弹
void RenderBullets(HDC hdc_memBuffer, HDC hdc_loadBmp);// 在屏幕上绘制所有子弹对象
