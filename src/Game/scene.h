/**
 * 这个文件是场景管理的头文件
 * 需要添加新的游戏场景时，修改这个文件，请同时修改宏函数部分
 */

#pragma once

// 游戏场景
enum SceneId
{
    None = 0,       // 没有场景
    StartScene = 1, // 开始场景
    GameScene = 2   // 游戏场景
};

struct Scene
{
    SceneId sceneId; // 游戏场景的编号

    // TODO: 如果场景需要保存更多信息，添加在这里
};

// 场景循环
void SceneLoop();

// 渲染场景
void RenderScene(HDC hdc_memBuffer, HDC hdc_loadBmp);

// 获取当前场景
Scene *GetCurrentScene();

// 切换场景
void ChangeScene(SceneId newSceneId);

// 宏函数 - 路由场景函数调用，如果有新的场景需要添加，在这里添加对应的 case 分支
#define ROUTE_SCENE_FUNCTION(FUNCTION_NAME, ...) \
    switch (GetCurrentScene()->sceneId)          \
    {                                            \
    case None:                                   \
        break;                                   \
    case StartScene:                             \
        FUNCTION_NAME##_StartScene(__VA_ARGS__); \
        break;                                   \
    case GameScene:                              \
        FUNCTION_NAME##_GameScene(__VA_ARGS__);  \
        break;                                   \
    default:                                     \
        break;                                   \
    }
