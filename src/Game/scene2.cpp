/**
 * 这个文件是场景2-游戏场景的源文件
 * 场景的逻辑实现在这里
 * 务必定义并实现所有的场景核心函数
 */

#include "stdafx.h"

#include "scene.h"
#include "scene2.h"

#include "player.h"
#include "enemy.h"
#include "bullet.h"

#pragma region 碰撞检测
// 检查角色和敌人的碰撞
static void CheckCollision_GameScene_Player_Enemies();
// 检查敌人和子弹的碰撞
static void CheckCollision_GameScene_Enemies_Bullets();
#pragma endregion

void LoadScene_GameScene()
{
    /* UI组件创建 */
    // 游戏场景暂时没有UI组件需要创建
    CreatePlayer();
    /* 游戏对象创建 */
    // TODO: 创建玩家对象
    // 敌人将在游戏过程中动态创建
    // 子弹将在游戏过程中动态创建
    // TODO: 游戏场景中需要创建的游戏对象
}

void UnloadScene_GameScene()
{
    /* UI组件销毁 */
    // 游戏场景暂时没有UI组件需要销毁
    /* 游戏对象销毁 */
    // TODO: 清空角色对象
    DestroyPlayer();
    // TODO: 清空敌人对象
    DestroyEnemies();
    // TODO: 清空子弹对象
    DestroyBullets();
    // TODO: 游戏场景卸载时需要销毁的游戏对象
}

void ProcessUiInput_GameScene()
{
    // 游戏场景暂时没有UI输入需要处理
}

void CheckCollision_GameScene()
{
    // 玩家和敌人的碰撞
    CheckCollision_GameScene_Player_Enemies();
    // 敌人和子弹的碰撞
    CheckCollision_GameScene_Enemies_Bullets();

    // TODO: 更多的碰撞逻辑
}

void UpdateScene_GameScene(double deltaTime)
{
    /* UI组件更新 */
    // 游戏场景暂时没有UI组件需要更新

    /* 游戏对象更新 */
    // 更新角色对象
    UpdatePlayer(deltaTime);
    // 更新敌人对象
    UpdateEnemies(deltaTime);
    // 更新子弹对象
    UpdateBullets(deltaTime);
    // TODO: 游戏场景中需要更新的游戏对象
}

void RenderScene_GameScene(HDC hdc_memBuffer, HDC hdc_loadBmp)
{
    /*
     * 注意绘制顺序，后绘制的会覆盖先绘制的
     * 所以先绘制游戏对象，再绘制UI组件
     */

    /* 游戏对象绘制 */
    // 绘制角色对象
    RenderPlayer(hdc_memBuffer, hdc_loadBmp);
    // 绘制敌人对象
    RenderEnemies(hdc_memBuffer, hdc_loadBmp);
    // 绘制子弹对象
    RenderBullets(hdc_memBuffer, hdc_loadBmp);
    // TODO: 游戏场景中需要渲染的游戏对象

    /* UI组件绘制 */
    // 绘制一个边框表示游戏区域
    // 选择画笔颜色（边框颜色）
    HPEN hPen = CreatePen(PS_SOLID, GAME_BOARDER, RGB(0, 0, 0));
    HGDIOBJ oldPen = SelectObject(hdc_memBuffer, hPen);

    // 使用透明画刷防止填充
    HGDIOBJ oldBrush = SelectObject(hdc_memBuffer, GetStockObject(NULL_BRUSH));

    // 绘制矩形（仅边框）
    Rectangle(hdc_memBuffer, GAME_X, GAME_Y, GAME_WIDTH, GAME_HEIGHT);

    // 还原 GDI 对象
    SelectObject(hdc_memBuffer, oldBrush);
    SelectObject(hdc_memBuffer, oldPen);
    DeleteObject(hPen);

    // 绘制玩家的属性信息
    HFONT hFont = CreateFont(
        30,                       // 字体高度
        0,                        // 字体宽度（0 表示自动计算）
        0,                        // 字体的倾斜角度
        0,                        // 字体的基线倾斜角度
        FW_NORMAL,                // 字体的粗细
        FALSE,                    // 是否斜体
        FALSE,                    // 是否下划线
        FALSE,                    // 是否删除线
        DEFAULT_CHARSET,          // 字符集
        OUT_DEFAULT_PRECIS,       // 输出精度
        CLIP_DEFAULT_PRECIS,      // 剪裁精度
        DEFAULT_QUALITY,          // 输出质量
        DEFAULT_PITCH | FF_SWISS, // 字体家族和间距
        TEXT("微软雅黑")          // 字体名称
    );
    // 选择自定义字体到设备上下文
    HFONT hOldFont = (HFONT)SelectObject(hdc_memBuffer, hFont);
    // 设置字体区域
    const int width = 300;
    const int height = 200;
    const int left = GAME_WIDTH;
    const int top = 80;
    const int right = left + width;
    const int bottom = top + height;
    RECT rect = {left, top, right, bottom};
    // 绘制
    TCHAR buffer[128];
    swprintf_s(buffer, sizeof(buffer) / sizeof(TCHAR),
               TEXT("第一关\n\n\n生命值: %d\n\n积分: %d"),
               GetPlayer()->attributes.health,
               GetPlayer()->attributes.score );
	DrawText(hdc_memBuffer, buffer, -1, &rect, DT_CENTER);// 居中对齐
    // 恢复原来的字体
    SelectObject(hdc_memBuffer, hOldFont);
    // 删除自定义字体以释放资源
    DeleteObject(hFont);

    // TODO: 游戏场景其他需要绘制的UI组件
}

#pragma region 碰撞检测

// 检查角色和敌人的碰撞
void CheckCollision_GameScene_Player_Enemies()
{
    // 玩家用简单矩形表示
    Player* player = GetPlayer();
    Rect rect1{};
    rect1.left = player->position.x;
    rect1.right = player->position.x + player->width;
    rect1.top = player->position.y;
    rect1.bottom = player->position.y + player->height;
	// 敌人用简单矩形表示
    std::vector<Enemy *> enemies = GetEnemies();
    Rect rect2{};
    for (Enemy* enemy : enemies)
    {
        rect2.left = enemy->position.x;
        rect2.right = enemy->position.x + enemy->width;
        rect2.top = enemy->position.y;
        rect2.bottom = enemy->position.y + enemy->height;
        if (IsRectRectCollision(rect1, rect2))
        {
            // 碰撞后扣血、加分摧毁敌人
            player->attributes.health--;
            player->attributes.score += enemy->attributes.score;
            DestroyEnemy(enemy);
            if (player->attributes.health <= 0)
            {
                Log(1, TEXT("游戏结束！"));
                ChangeScene(SceneId::StartScene);
            }
        }
    }
    // TODO: 实现玩家和敌人的碰撞检测
}

// 检查敌人和子弹的碰撞
void CheckCollision_GameScene_Enemies_Bullets()
{
	// 敌人用简单矩形表示
    std::vector<Enemy *> enemies = GetEnemies();
    Rect rect{};
    // 子弹用简单圆形表示
    std::vector<Bullet *> bullets = GetBullets();
    Circle circle{};
	// TODO: 实现敌人和子弹的碰撞检测
    for (Enemy* enemy : enemies)
    {
        rect.left = enemy->position.x;
        rect.right = enemy->position.x + enemy->width;
        rect.top = enemy->position.y;
        rect.bottom = enemy->position.y + enemy->height;
        for (Bullet* bullet : bullets)
        {
            circle.center = bullet->position;
            circle.radius = bullet->radius;
            if (IsRectCircleCollision(rect, circle))
            {
                // 碰撞后扣血、加分摧毁敌人和子弹
                enemy->attributes.health -= bullet->damage;
                if (enemy->attributes.health <= 0)
                {
                    GetPlayer()->attributes.score += enemy->attributes.score;
                    DestroyEnemy(enemy);
                }
                DestroyBullet(bullet);
            }
        }
    }
}

#pragma endregion
