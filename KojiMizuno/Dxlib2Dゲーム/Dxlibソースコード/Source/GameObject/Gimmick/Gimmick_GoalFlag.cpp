#include "Gimmick_GoalFlag.h"
#include "../Ground/Ground.h"
#include "../Source/Utility/BoxCollisionParams.h"
#include "../SampleObject/SampleObject.h"
#include "../Source/Scene/SampleScene/SampleScene.h"
#include <algorithm> // std::maxを使用するため
#define NOMINMAX
#include "DxLib.h"

Gimmick_GoalFlag::Gimmick_GoalFlag()
{

}

Gimmick_GoalFlag::~Gimmick_GoalFlag()
{
	Finalize();
}

void Gimmick_GoalFlag::Initialize(Vector2D initialVec)
{
	__super::Initialize();
	sampleObject_ = SampleScene::GetSampleObject();
	bcp = new BoxCollisionParams();

	// 初期座標設定
	SetPosition(initialVec);

	// 画像の読み込み
	loaded_goal_flag_handle = LoadGraph("Resources/Images/goal_flag.bmp");
}

void Gimmick_GoalFlag::Update(float delta_seconds)
{
	__super::Update(delta_seconds);

	Vector2D input_dir;

	// プレイヤーと接触時ゲームクリア
	if (bcp->CheckHit(static_cast<int>(sampleObject_->GetPosition().x - 14.f), static_cast<int>(sampleObject_->GetPosition().y + 2.f), static_cast<int>(sampleObject_->GetPosition().x - 14.f + 27), static_cast<int>(sampleObject_->GetPosition().y + 2.f + 38),
		static_cast<int>(GetPosition().x - 14.f), static_cast<int>(GetPosition().y - 10.f), static_cast<int>(GetPosition().x - 14.f + 27), static_cast<int>(GetPosition().y - 10.f + 80)))
	{
		sampleObject_->Clear();
	}
}

void Gimmick_GoalFlag::Draw(const Vector2D& screen_offset)
{
	__super::Draw(screen_offset);

	// 画像の描画
	int x, y;
	GetPosition().ToInt(x, y);
	DrawGraph(static_cast<int>(GetPosition().x - 14.f - screen_offset.x), static_cast<int>(GetPosition().y - 10.f - screen_offset.y), loaded_goal_flag_handle, true);
}

void Gimmick_GoalFlag::Finalize()
{
	__super::Finalize();
	// 画像の破棄
	DeleteGraph(loaded_goal_flag_handle);
	loaded_goal_flag_handle = 0;
}