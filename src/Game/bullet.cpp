/**
 * 这个文件是子弹对象的源文件
 * 子弹对象的逻辑实现在这
 */

#include "stdafx.h"// 预编译头文件,作用是加快编译速度
#include "bullet.h"// 包含子弹对象的头文件
#include "audio.h"
#include "enemy.h" // to notify owner when bullet destroyed

static std::set<Bullet*> bullets;// 存储所有子弹对象的集合

// helper normalize
static Vector2 NormalizeSafe(Vector2 v)
{
    double len = sqrt(v.x * v.x + v.y * v.y);
    if (len <= 1e-8) return {0, -1};
    return { v.x / len, v.y / len };
}

void CreateBullet(double x, double y, int damage, double speed, Vector2 direction, bool isEnemy, Enemy* owner)
{
    // 归一化方向
    direction = NormalizeSafe(direction);

    Bullet* bullet = new Bullet();// 创建一个新的子弹对象
    bullet->position.x = x;// 设置子弹的位置
    bullet->position.y = y;// 设置子弹的位置
    bullet->radius = BULLET_RADIUS;// 设置子弹的半径
    bullet->damage = damage;// 设置子弹的伤害
    bullet->speed = speed;// 设置子弹的速度
    bullet->direction = direction;
    bullet->isEnemy = isEnemy;
    bullet->owner = owner;
    bullets.insert(bullet);// 将子弹对象添加到集合中

    // Diagnostic log to help debug runtime: will show when bullets are created
    Log(3, "CreateBullet isEnemy=%d pos=(%.1f,%.1f) dir=(%.2f,%.2f) speed=%.1f", isEnemy ? 1 : 0, x, y, direction.x, direction.y, speed);
}

void DestroyBullet(Bullet* bullet)// 摧毁指定的子弹对象
{
    // 如果子弹有 owner（敌机），通知敌机该子弹已被销毁，允许其再次发射
    if (bullet->owner)
    {
        Enemy* e = bullet->owner;
        // 检查该敌机仍存在于敌人集合中
        std::vector<Enemy*> es = GetEnemies();
        for (Enemy* en : es)
        {
            if (en == e)
            {
                en->hasBullet = false;
                break;
            }
        }
    }
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
    // 子弹移动逻辑
    const double OFFSCREEN_BUFFER = 50.0; // 允许的缓冲区
    for (Bullet* bullet : GetBullets())
    {
        bullet->position.x += bullet->direction.x * bullet->speed * deltaTime;
        bullet->position.y += bullet->direction.y * bullet->speed * deltaTime;
        // 超出屏幕的子弹删除（带缓冲区）
        if (bullet->position.y + bullet->radius < -OFFSCREEN_BUFFER || bullet->position.y - bullet->radius > GAME_HEIGHT + OFFSCREEN_BUFFER)
        {
            DestroyBullet(bullet);
        }
    }
}

void RenderBullets(HDC hdc_memBuffer, HDC hdc_loadBmp)// 在屏幕上绘制所有子弹对象
{
    // 绘制子弹，根据 isEnemy 使用不同颜色
    for (Bullet* bullet : GetBullets())
    {
        HBRUSH hBrush = CreateSolidBrush(bullet->isEnemy ? RGB(0, 120, 255) : RGB(255, 0, 0));
        HBRUSH oldBrush = (HBRUSH)SelectObject(hdc_memBuffer, hBrush);
        // 设置画笔为空，避免边框影响
        HPEN hPen = (HPEN)SelectObject(hdc_memBuffer, GetStockObject(NULL_PEN));
        Ellipse(
            hdc_memBuffer,
            (int)(bullet->position.x - bullet->radius),
            (int)(bullet->position.y - bullet->radius),
            (int)(bullet->position.x + bullet->radius),
            (int)(bullet->position.y + bullet->radius));
        // 恢复 GDI 对象
        SelectObject(hdc_memBuffer, oldBrush);
        SelectObject(hdc_memBuffer, hPen);
        DeleteObject(hBrush);
    }
}
