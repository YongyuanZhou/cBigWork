/**
 * 这个文件是子弹对象的头文件
 * 子弹对象的逻辑实现在这
 */

#pragma once

struct Bullet
{
	Vector2 position;
	int radius;
	int damage;
	double speed;
};// 子弹结构体，包含位置、半径、伤害和速度等属性

// 创建子弹
void CreateBullet(double x, double y, int damage, double speed);// 创建一个新的子弹对象
// 摧毁子弹
void DestroyBullet(Bullet* bullet);// 摧毁指定的子弹对象
void DestroyBullets();// 摧毁所有子弹对象
// 获取所有子弹
std::vector<Bullet*> GetBullets();// 获取当前所有子弹对象的列表
// 更新所有子弹
void UpdateBullets(double deltaTime);// 更新所有子弹的位置和状态
// 渲染所有子弹
void RenderBullets(HDC hdc_memBuffer, HDC hdc_loadBmp);// 在屏幕上绘制所有子弹对象
