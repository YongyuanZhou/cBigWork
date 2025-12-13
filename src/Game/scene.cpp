/**
 * 这个文件是场景管理的源文件
 * 需要添加新的游戏场景时，include新场景的头文件
 * 除非你想要修改游戏场景相关的逻辑，否则不需要改这个文件中的其他内容
 */

#include "stdafx.h"

#include "scene.h"
#include "scene1.h"
#include "scene2.h"
#include "help_scene.h"
#include "setting_scene.h"

// 当前游戏场景
static Scene sceneInstance = {None,false};
static Scene *currentScene = &sceneInstance;

static SceneId _newSceneId = None;

// 场景循环
void SceneLoop()
{
	if (_newSceneId != None)
	{
		// 卸载当前场景
		ROUTE_SCENE_FUNCTION(UnloadScene);
		// 切换场景ID
		currentScene->sceneId = _newSceneId;
		_newSceneId = None;
		//修改：11.30 切换场景时重置暂停状态
		currentScene->isPaused = false;
		// 加载新场景
		ROUTE_SCENE_FUNCTION(LoadScene);
	}

	//修改：11.30 如果未暂停，按原流程走：处理UI输入、碰撞、更新
	if (!currentScene->isPaused)
	{
		// 先处理UI输入
		ROUTE_SCENE_FUNCTION(ProcessUiInput);

		// 再计算碰撞
		ROUTE_SCENE_FUNCTION(CheckCollision);

		// 然后运行游戏逻辑
		ROUTE_SCENE_FUNCTION(UpdateScene, GetDeltaTime());
	}
	else
	{
		ROUTE_SCENE_FUNCTION(ProcessUiInput);
	}
}

// 渲染场景
void RenderScene(HDC hdc_memBuffer, HDC hdc_loadBmp)
{
	ROUTE_SCENE_FUNCTION(RenderScene, hdc_memBuffer, hdc_loadBmp);
}

// 获取当前场景
Scene *GetCurrentScene()
{
	return currentScene;
}

// 切换场景
void ChangeScene(SceneId newSceneId)
{
	_newSceneId = newSceneId;
}
