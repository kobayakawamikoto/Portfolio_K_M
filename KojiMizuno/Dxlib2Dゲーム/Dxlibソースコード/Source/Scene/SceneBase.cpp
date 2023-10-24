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
		Vector2D vec;
		// �X�N���[���I�t�Z�b�g
		if (objects[1]->GetPosition().x > 640 && objects[1]->GetPosition().y > 480) // 3
		{
			float offset_val = floor(static_cast<int>(objects[1]->GetPosition().x) / 640);
			vec.x = 640.f * offset_val;
			offset_val = floor(static_cast<int>(objects[1]->GetPosition().y) / 480);
			vec.y = 480.f * offset_val;
			(*iterator)->Draw(vec);
		}
		else if (objects[1]->GetPosition().x > 640 && objects[1]->GetPosition().y < 0) // 9
		{
			float offset_val = floor(static_cast<int>(objects[1]->GetPosition().x) / 640);
			vec.x = 640.f * offset_val;
			offset_val = ceil(static_cast<int>(objects[1]->GetPosition().y) / -480) + 1;
			vec.y = -480.f * offset_val;
			(*iterator)->Draw(vec);
		}
		else if (objects[1]->GetPosition().x < 0 && objects[1]->GetPosition().y > 480) // 1
		{
			float offset_val = ceil(static_cast<int>(objects[1]->GetPosition().x) / -640) + 1;
			vec.x = -640.f * offset_val;
			offset_val = floor(static_cast<int>(objects[1]->GetPosition().y) / 480);
			vec.y = 480.f * offset_val;
			(*iterator)->Draw(vec);
		}
		else if (objects[1]->GetPosition().x < 0 && objects[1]->GetPosition().y < 0) // 7
		{
			float offset_val = ceil(static_cast<int>(objects[1]->GetPosition().x) / -640) + 1;
			vec.x = -640.f * offset_val;
			offset_val = ceil(static_cast<int>(objects[1]->GetPosition().y) / -480) + 1;
			vec.y = -480.f * offset_val;
			(*iterator)->Draw(vec);
		}

		else if (objects[1]->GetPosition().x > 640 && objects[1]->GetPosition().y >= 0 && objects[1]->GetPosition().y <= 480) // 6
		{
			float offset_val = floor(static_cast<int>(objects[1]->GetPosition().x) / 640);
			vec.x = 640.f * offset_val;
			(*iterator)->Draw(vec);
		}
		else if (objects[1]->GetPosition().x < 0 && objects[1]->GetPosition().y >= 0 && objects[1]->GetPosition().y <= 480) // 4
		{
			float offset_val = ceil(static_cast<int>(objects[1]->GetPosition().x) / -640) + 1;
			vec.x = -640.f * offset_val;
			(*iterator)->Draw(vec);
		}
		else if (objects[1]->GetPosition().y > 480 && objects[1]->GetPosition().x >= 0 && objects[1]->GetPosition().x <= 640) // 2
		{
			float offset_val = floor(static_cast<int>(objects[1]->GetPosition().y) / 480);
			vec.y = 480.f * offset_val;
			(*iterator)->Draw(vec);
		}
		else if (objects[1]->GetPosition().y < 0 && objects[1]->GetPosition().x >= 0 && objects[1]->GetPosition().x <= 640) // 8
		{
			float offset_val = ceil(static_cast<int>(objects[1]->GetPosition().y) / -480) + 1;
			vec.y = -480.f * offset_val;
			(*iterator)->Draw(vec);
		}
		else // 5
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
