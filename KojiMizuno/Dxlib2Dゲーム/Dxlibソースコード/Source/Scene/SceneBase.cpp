#include "SceneBase.h"
#include "DxLib.h"

SceneBase::SceneBase()
	: screen_offset(Vector2D())
{
}

SceneBase::~SceneBase()
{
	Finalize();
}

void SceneBase::Initialize()
{

}

SceneType SceneBase::Update(float delta_seconds)
{
	for (auto iterator = objects.begin(); iterator != objects.end(); ++iterator)
	{
		(*iterator)->Update(delta_seconds);
	}

	// ここにコリジョンを書いてもいいかも
	
	return GetSceneType();
}

void SceneBase::Draw()
{
	for (auto iterator = objects.begin(); iterator != objects.end(); ++iterator)
	{
		// スクリーンオフセット
		if (objects[1]->GetPosition().x > 640)
		{
			Vector2D vec;
			vec.x = 640.f;
			(*iterator)->Draw(vec);
		}
		else if (objects[1]->GetPosition().x < 0)
		{
			Vector2D vec;
			vec.x = -640.f;
			(*iterator)->Draw(vec);
		}
		else
		{
			(*iterator)->Draw(screen_offset);
		}
		//DrawFormatString(0, 200, 100, "chara_x: %f ", objects[1]->GetPosition().x);
	}
}

void SceneBase::Finalize()
{
	// 全てのオブジェクトを破棄
	DestroyAllObjects();
}

void SceneBase::DestroyObject(GameObject* object)
{
	if (object == nullptr)
	{
		return;
	}

	// 削除対象の検索
	auto iterator = objects.begin();
	for (; iterator != objects.end(); ++iterator)
	{
		if ((*iterator) == object)
		{
			break;
		}
	}

	// 見つからなかった場合は抜ける
	if (iterator == objects.end())
	{
		return;
	}

	// 削除
	(*iterator)->Finalize();
	objects.erase(iterator);
	delete object;
}

void SceneBase::DestroyAllObjects()
{
	for (auto iterator = objects.begin(); iterator != objects.end(); ++iterator)
	{
		(*iterator)->Finalize();
		delete (*iterator);
	}
	objects.clear();
}
