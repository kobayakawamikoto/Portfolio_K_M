#include "SampleScene.h"
#include "../../SystemTypes.h"
#include "../../GameObject/SampleObject/SampleObject.h"
#include "../../GameObject/Enemy/Enemy_Purple.h"
#include "../../GameObject/Enemy/Enemy_Red.h"
#include "../../GameObject/Enemy/Enemy_Red_Shy.h"
#include "../../GameObject/Ground/Ground.h"
#include "../../GameObject/Gimmick/Gimmick_GoalFlag.h"
#include "../../GameObject/Gimmick/Gimmick_Save.h"
#include "../../GameObject/Gimmick/Gimmick_Apple.h"
#include "DxLib.h"

// static�ϐ��̒�`
SampleObject* SampleScene::sampleObject = nullptr;
Enemy_Purple* SampleScene::enemy_Purple = nullptr;
SampleScene* SampleScene::sampleScene = nullptr;

SampleScene::SampleScene()
{
}

void SampleScene::Initialize()
{
	// �e�N���X��Initialize()
	__super::Initialize();

	sampleScene = this;

	// SampleObject�𐶐�
	CreateObject<Ground>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f));
	
	sampleObject = CreateObject<SampleObject>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f));
	enemy_Purple = CreateObject<Enemy_Purple>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f));
	CreateObject<Enemy_Red>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f));
	CreateObject<Enemy_Red_Shy>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f));
	CreateObject<Gimmick_GoalFlag>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f));
	CreateObject<Gimmick_Save>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f));
	CreateObject<Gimmick_Apple>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f), Vector2D(200.f, 340.f), 0, 8, 2);
	CreateObject<Gimmick_Apple>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f), Vector2D(230.f, 340.f), 0, 10, 2);
	CreateObject<Gimmick_Apple>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f), Vector2D(260.f, 340.f), 0, 12, 2);
	CreateObject<Gimmick_Apple>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f), Vector2D(290.f, 340.f), -8, -12, 7);
	CreateObject<Gimmick_Apple>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f), Vector2D(320.f, 340.f), 0, -12, 8);
	CreateObject<Gimmick_Apple>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f), Vector2D(350.f, 340.f), 0, -12, 8);
	CreateObject<Gimmick_Apple>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f), Vector2D(460.f, 340.f), 12, 0, 6);
}

SceneType SampleScene::Update(float delta_seconds)
{
	// �e�N���X��Update()s
	return __super::Update(delta_seconds);
}

void SampleScene::Draw()
{
	// �e�N���X��Draw()
	__super::Draw();
}

void SampleScene::Finalize()
{
	// �e�N���X��Finalize()
	__super::Finalize();
}

// static�֐�
SampleObject* SampleScene::GetSampleObject()
{
	if (sampleObject != nullptr)
	{
		return sampleObject;
	}
	else
	{
		return nullptr;
	}
}

// static�֐�
SampleScene* SampleScene::GetSampleScene() {
	
	return sampleScene;
};