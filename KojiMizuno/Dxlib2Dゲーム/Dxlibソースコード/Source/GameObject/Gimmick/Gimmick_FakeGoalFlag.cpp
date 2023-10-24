#include "Gimmick_FakeGoalFlag.h"
#include "../Ground/Ground.h"
#include "../Source/Utility/BoxCollisionParams.h"
#include "../SampleObject/SampleObject.h"
#include "../Source/Scene/SampleScene/SampleScene.h"
#include <algorithm> // std::maxを使用するため
#define NOMINMAX
#include "DxLib.h"

Gimmick_FakeGoalFlag::Gimmick_FakeGoalFlag()
{

}

Gimmick_FakeGoalFlag::~Gimmick_FakeGoalFlag()
{
	Finalize();
}

void Gimmick_FakeGoalFlag::Initialize(Vector2D initialVec)
{
	__super::Initialize();
	sampleObject_ = SampleScene::GetSampleObject();
	bcp = new BoxCollisionParams();

	// 初期座標設定
	SetPosition(initialVec);

	// 画像の読み込み
	loaded_goal_flag = LoadGraph("Resources/Images/goal_flag.bmp");
	loaded_windows = LoadGraph("Resources/Images/windows.png");
}

void Gimmick_FakeGoalFlag::Update(float delta_seconds)
{
	__super::Update(delta_seconds);

	Vector2D input_dir;

	// プレイヤーと接触時isFakeClearedをtrueに
	if (bcp->CheckHit(static_cast<int>(sampleObject_->GetPosition().x - 14.f), static_cast<int>(sampleObject_->GetPosition().y + 2.f), static_cast<int>(sampleObject_->GetPosition().x - 14.f + 27), static_cast<int>(sampleObject_->GetPosition().y + 2.f + 38),
		static_cast<int>(GetPosition().x - 14.f), static_cast<int>(GetPosition().y - 10.f), static_cast<int>(GetPosition().x - 14.f + 27), static_cast<int>(GetPosition().y - 10.f + 80)))
	{
		sampleObject_->FakeClear();
		isFakeCleared = true;
		Vector2D vec;
		vec.x = 600.f;
		vec.y = 50.f;
		SetPosition(vec);
		
	}
	
	// windowsと接触時ゲームオーバー
	if (isFakeCleared)
	{
		Vector2D input_dir;
		input_dir.x = 1;
		delta_position = input_dir.Normalize() * delta_seconds * 20;
		SetPosition(GetPosition() + delta_position);
		if (bcp->CheckHit(static_cast<int>(sampleObject_->GetPosition().x - 14.f), static_cast<int>(sampleObject_->GetPosition().y + 2.f), static_cast<int>(sampleObject_->GetPosition().x - 14.f + 27), static_cast<int>(sampleObject_->GetPosition().y + 2.f + 38),
			static_cast<int>(GetPosition().x - 14.f), static_cast<int>(GetPosition().y - 10.f), static_cast<int>(GetPosition().x - 14.f + 420), static_cast<int>(GetPosition().y - 10.f + 300)))
		{
			sampleObject_->SetIsCleared(false);
			sampleObject_->Dead();

		}
	}
}

void Gimmick_FakeGoalFlag::Draw(const Vector2D& screen_offset)
{
	__super::Draw(screen_offset);

	// 画像の描画
	int x, y;
	GetPosition().ToInt(x, y);
	DrawGraph(static_cast<int>(GetPosition().x - 14.f - screen_offset.x), static_cast<int>(GetPosition().y - 10.f - screen_offset.y), loaded_goal_flag, true);
	if (isFakeCleared)
	{
		DrawGraph(static_cast<int>(GetPosition().x - 14.f - screen_offset.x), static_cast<int>(GetPosition().y - 10.f - screen_offset.y), loaded_windows, true);
	}
	//DrawBox(static_cast<int>(GetPosition().x - 14.f) - screen_offset.x, static_cast<int>(GetPosition().y - 10.f) - screen_offset.y, static_cast<int>(GetPosition().x - 14.f + 500) - screen_offset.x, static_cast<int>(GetPosition().y - 10.f + 300), 100, true) - screen_offset.y;
}

void Gimmick_FakeGoalFlag::Finalize()
{
	__super::Finalize();
}