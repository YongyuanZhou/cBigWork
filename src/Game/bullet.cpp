/**
 * 这个文件是子弹对象的源文件
 * 子弹对象的逻辑实现在这
 */

#include "stdafx.h"// 预编译头文件,作用是加快编译速度
#include "bullet.h"// 包含子弹对象的头文件

static std::set<Bullet*> bullets;// 存储所有子弹对象的集合

void CreateBullet(double x, double y, int damage, double speed)
{
	Bullet* bullet = new Bullet();// 创建一个新的子弹对象
	bullet->position.x = x;// 设置子弹的位置
	bullet->position.y = y;// 设置子弹的位置
	bullet->radius = BULLET_RADIUS;// 设置子弹的半径
	bullet->damage = damage;// 设置子弹的伤害
	bullet->speed = speed;// 设置子弹的速度
	bullets.insert(bullet);// 将子弹对象添加到集合中
}

void DestroyBullet(Bullet* bullet)// 摧毁指定的子弹对象
{
	bullets.erase(bullet);
	delete bullet;
}

void DestroyBullets()// 摧毁所有子弹对象
{
	for (Bullet *bullet : bullets)
	{
		delete bullet;
	}
	bullets.clear();
}

std::vector<Bullet*> GetBullets()// 获取当前所有子弹对象的列表
{
	// 返回的是子弹指针的副本列表 - 避免边遍历边删除时出错
	return CopyFromSet(bullets);
}

void UpdateBullets(double deltaTime)// 更新所有子弹的位置和状态
{
	// TODO: 子弹移动逻辑
	for (Bullet* bullet : GetBullets())
	{
		// 子弹向上移动
		bullet->position.y -= bullet->speed * deltaTime;
		// 超出屏幕的子弹删除
		if (bullet->position.y + bullet->radius < 0)
		{
			DestroyBullet(bullet);
		}
	}
}

void RenderBullets(HDC hdc_memBuffer, HDC hdc_loadBmp)// 在屏幕上绘制所有子弹对象
{
	// TODO: 绘制子弹
	// 创建红色实心画刷
	HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 0));
	HBRUSH oldBrush = (HBRUSH)SelectObject(hdc_memBuffer, hBrush);
	// 设置画笔为空，避免边框影响
	HPEN hPen = (HPEN)SelectObject(hdc_memBuffer, GetStockObject(NULL_PEN));
	// 绘制子弹
	for (Bullet* bullet : bullets)
	{
		Ellipse(
			hdc_memBuffer,
			(int)(bullet->position.x - bullet->radius),
			(int)(bullet->position.y - bullet->radius),
			(int)(bullet->position.x + bullet->radius),
			(int)(bullet->position.y + bullet->radius));
	}
	// 恢复 GDI 对象
	SelectObject(hdc_memBuffer, oldBrush);
	SelectObject(hdc_memBuffer, hPen);
	DeleteObject(hBrush);
}
