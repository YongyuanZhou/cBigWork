/**
 * 这个文件是游戏类型的头文件
 * 基本的共有类型可以定义在这里
 */

#pragma once

// 二维向量
struct Vector2
{
    double x;
    double y;
};

// 矩形
struct Rect
{
    double left;
    double top;
    double right;
    double bottom;
};

// 圆
struct Circle
{
    Vector2 center;
    double radius;
};

// 状态
struct Attributes
{
    int health;         // 生命值
    int score;          // 积分
    double speed;       // 速度
    double maxBulletCd; // 最大子弹冷却时间
    double bulletCd;    // 子弹冷却时间

    // TODO: 其他相关的状态数值
};
// 现在玩家和敌人共用一个状态结构体，需要的话可以拆成两个
