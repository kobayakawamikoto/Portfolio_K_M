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

	// �����ɃR���W�����������Ă���������
	
	return GetSceneType();
}

void SceneBase::Draw()
{
	for (auto iterator = objects.begin(); iterator != objects.end(); ++iterator)
	{
		// �X�N���[���I�t�Z�b�g
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
	// �S�ẴI�u�W�F�N�g��j��
	DestroyAllObjects();
}

void SceneBase::DestroyObject(GameObject* object)
{
	if (object == nullptr)
	{
		return;
	}

	// �폜�Ώۂ̌���
	auto iterator = objects.begin();
	for (; iterator != objects.end(); ++iterator)
	{
		if ((*iterator) == object)
		{
			break;
		}
	}

	// ������Ȃ������ꍇ�͔�����
	if (iterator == objects.end())
	{
		return;
	}

	// �폜
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
