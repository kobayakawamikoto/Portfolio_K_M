#include "SampleScene.h"
#include "../../SystemTypes.h"
#include "../../GameObject/SampleObject/SampleObject.h"
#include "../../GameObject/Enemy/Enemy_Purple.h"
#include "../../GameObject/Enemy/Enemy_Red.h"
#include "../../GameObject/Enemy/Enemy_Red_Shy.h"
#include "../../GameObject/Enemy/Enemy_Boss.h"
#include "../../GameObject/Ground/Ground.h"
#include "../../GameObject/Gimmick/Gimmick_GoalFlag.h"
#include "../../GameObject/Gimmick/Gimmick_FakeGoalFlag.h"
#include "../../GameObject/Gimmick/Gimmick_Save.h"
#include "../../GameObject/Gimmick/Gimmick_Apple.h"
#include "../../GameObject/Platform/Platform_Passable.h"
#include "DxLib.h"

// static変数の定義
SampleObject* SampleScene::sampleObject = nullptr;
Enemy_Purple* SampleScene::enemy_Purple = nullptr;
SampleScene* SampleScene::sampleScene = nullptr;

SampleScene::SampleScene()
{
}

void SampleScene::Initialize()
{
	// 親クラスのInitialize()
	__super::Initialize();

	sampleScene = this;

	// SampleObjectを生成
	CreateObject<Ground>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f));

	/* 0 < x < 640, 0 < y < 480*/
	sampleObject = CreateObject<SampleObject>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f));
	enemy_Purple = CreateObject<Enemy_Purple>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f));
	CreateObject<Enemy_Red>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f));
	CreateObject<Gimmick_Save>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f), Vector2D(500.f, 100.f));
	CreateObject<Gimmick_Apple>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f), Vector2D(200.f, 340.f), 0, 8, 2);
	CreateObject<Gimmick_Apple>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f), Vector2D(230.f, 340.f), 0, 10, 2);
	CreateObject<Gimmick_Apple>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f), Vector2D(260.f, 340.f), 0, 12, 2);
	CreateObject<Gimmick_Apple>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f), Vector2D(290.f, 340.f), -8, -12, 7);
	CreateObject<Gimmick_Apple>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f), Vector2D(320.f, 340.f), 0, -12, 8);
	CreateObject<Gimmick_Apple>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f), Vector2D(350.f, 340.f), 0, -12, 8);
	CreateObject<Gimmick_Apple>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f), Vector2D(460.f, 340.f), 12, 0, 6);

	/* 640 < x < 1280, 0 < y < 480*/
	CreateObject<Enemy_Red_Shy>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f));
	CreateObject<Gimmick_FakeGoalFlag>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f), Vector2D(1225.f, 70.f));

	/* -640 < x < 0, 0 < y < 480*/
	CreateObject<Gimmick_Save>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f), Vector2D(-550.f, 15.f));
	CreateObject<Gimmick_Apple>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f), Vector2D(-500.f, 440.f), 0, -8, 8);
	CreateObject<Gimmick_Apple>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f), Vector2D(-550.f, 440.f), 0, -8, 8);
	CreateObject<Gimmick_Apple>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f), Vector2D(-600.f, 440.f), 0, -8, 8);
	CreateObject<Gimmick_Apple>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f), Vector2D(-650.f, 440.f), 0, -8, 8);
	CreateObject<Gimmick_Apple>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f), Vector2D(-700.f, 440.f), 0, -8, 8);
	CreateObject<Gimmick_Apple>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f), Vector2D(-450.f, 440.f), 0, -8, 8);
	CreateObject<Gimmick_Apple>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f), Vector2D(-400.f, 440.f), 0, -8, 8);
	CreateObject<Gimmick_Apple>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f), Vector2D(-350.f, 440.f), 0, -8, 8);
	CreateObject<Gimmick_Apple>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f), Vector2D(-300.f, 440.f), 0, -8, 8);
	CreateObject<Gimmick_Apple>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f), Vector2D(-250.f, 440.f), 0, -8, 8);
	CreateObject<Gimmick_Apple>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f), Vector2D(-200.f, 440.f), 0, -8, 8);
	CreateObject<Gimmick_Apple>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f), Vector2D(-150.f, 440.f), 0, -8, 8);
	CreateObject<Gimmick_Apple>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f), Vector2D(-100.f, 440.f), 0, -8, 8);
	CreateObject<Gimmick_Apple>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f), Vector2D(-50.f, 440.f), 0, -8, 8);
	CreateObject<Gimmick_Apple>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f), Vector2D(-5.f, 440.f), 0, -8, 8);

	/* -640 < x < 0, -480 < y < 0*/
	CreateObject<Gimmick_Apple>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f), Vector2D(-330.f, 210.f), 0, -8, 8);
	CreateObject<Gimmick_Apple>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f), Vector2D(-360.f, 50.f), 0, -8, 8);
	CreateObject<Gimmick_Apple>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f), Vector2D(-360.f, 100.f), 0, -8, 8);
	CreateObject<Gimmick_Apple>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f), Vector2D(-360.f, 150.f), 0, -8, 8);
	CreateObject<Gimmick_Apple>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f), Vector2D(-620.f, 50.f), 0, -8, 8);
	CreateObject<Gimmick_Apple>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f), Vector2D(-620.f, 100.f), 0, -8, 8);
	CreateObject<Gimmick_Apple>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f), Vector2D(-620.f, 150.f), 0, -8, 8);
	CreateObject<Gimmick_Apple>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f), Vector2D(-620.f, 200.f), 0, -8, 8);
	CreateObject<Gimmick_Apple>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f), Vector2D(-620.f, 250.f), 0, -8, 8);
	CreateObject<Gimmick_Apple>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f), Vector2D(-620.f, 300.f), 0, -8, 8);
	CreateObject<Gimmick_Apple>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f), Vector2D(-620.f, 350.f), 0, -8, 8);
	CreateObject<Gimmick_Apple>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f), Vector2D(-620.f, 400.f), 0, -8, 8);
	CreateObject<Platform_Passable>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f), Vector2D(-620.f, 120.f), 1.f, 5.f, 0);
	CreateObject<Platform_Passable>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f), Vector2D(-600.f, 320.f), 2.f, 3.f, 0);
	CreateObject<Platform_Passable>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f), Vector2D(-640.f, 220.f), 6.f, 15.f, 0);
	CreateObject<Enemy_Boss>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f));

	/* 0 < x < 640, -480 < y < 0*/
	CreateObject<Gimmick_GoalFlag>(Vector2D(SCREEN_RESOLUTION_X / 2.0f, SCREEN_RESOLUTION_Y / 2.0f), Vector2D(318.f, -330.f));
}

SceneType SampleScene::Update(float delta_seconds)
{
	// 親クラスのUpdate()s
	return __super::Update(delta_seconds);
}

void SampleScene::Draw()
{
	// 親クラスのDraw()
	__super::Draw();
}

void SampleScene::Finalize()
{
	// 親クラスのFinalize()
	__super::Finalize();
}

// static関数
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

// static関数
SampleScene* SampleScene::GetSampleScene() {

	return sampleScene;
};