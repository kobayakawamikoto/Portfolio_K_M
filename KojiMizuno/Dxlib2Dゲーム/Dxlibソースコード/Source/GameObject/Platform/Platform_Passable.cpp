#include "Platform_Passable.h"
#include "../Source/Utility/BoxCollisionParams.h"
#include "../SampleObject/SampleObject.h"
#include "../Source/Scene/SampleScene/SampleScene.h"
#include <algorithm> // std::maxを使用するため
#define NOMINMAX
#include "DxLib.h"

Platform_Passable::Platform_Passable()
	: loaded_sprite_handle(0), bcp()
{
}

Platform_Passable::~Platform_Passable()
{
	Finalize();
}

void Platform_Passable::Initialize(Vector2D initialVec, float radian_period_, float speed_, int direction_)
{
	__super::Initialize();

	// 画像の読み込み
	loaded_sprite_handle = LoadGraph(_T("Resources/Images/platform_passable2.bmp"));

	bcp = new BoxCollisionParams();

	// 初期座標設定
	SetPosition(initialVec);
	radian_period = radian_period_;
	speed = speed_;
	// サンプルシーンからサンプルオブジェクトを取ってくる
	sampleObject_ = SampleScene::GetSampleObject();
	prevPosition_x = position.x;
	prevPosition_y = position.y;
	isLeftDir = true;
}

void Platform_Passable::Update(float delta_seconds)
{
	__super::Update(delta_seconds);

	// 前回の位置を覚えておく
	prev_x = GetPosition().x;
	prev_y = GetPosition().y;

	// 往復移動
	Move(delta_seconds);

	// プレイヤーと接触
	HitPlayer();
}

void Platform_Passable::Draw(const Vector2D& screen_offset)
{
	__super::Draw(screen_offset);

	int x, y;
	GetPosition().ToInt(x, y);
	Chara_AnimFrame();


	// キャラ描画
	DrawGraph(x - screen_offset.x, y - screen_offset.y, loaded_sprite_handle, true);

	//DrawBox(static_cast<int>(GetPosition().x), static_cast<int>(GetPosition().y - 5.f), static_cast<int>(GetPosition().x + 33.f), static_cast<int>(GetPosition().y + 10.f), 100, false);
	//DrawFormatString(0, 0, 100, "platform_pos_x = %f", GetPosition().x);
}

void Platform_Passable::Finalize()
{
	__super::Finalize();

	// 画像の破棄
	DeleteGraph(loaded_sprite_handle);
}

/*
*	アニメーションフレームスピードの設定
*/
void Platform_Passable::Chara_AnimFrame()
{
	if (--act_wait <= 0)
	{
		act_index++;
		act_wait = ACT_SPEED; // アニメーションスピード
	}
}

/*
*	サイン関数を使って足場を左右に動かす
*/
void Platform_Passable::Move(float delta_seconds)
{
	Vector2D input_dir;

	// radian_periodを加算していく
	radian_x += radian_period;
	// sin関数でx座標を計算
	input_dir.x = speed * sin(radian_x * 0.01);
	
	delta_position = input_dir * MOVEMENT_SPEED * delta_seconds;
	// x座標を変更
	SetPosition(GetPosition() + delta_position);
}

/*
*	プレイヤーと接触した時
*/
void Platform_Passable::HitPlayer()
{
	// プレイヤーと接触時true
	if (bcp->CheckHit(static_cast<int>(sampleObject_->GetPosition().x - 14.f), static_cast<int>(sampleObject_->GetPosition().y + 2.f), static_cast<int>(sampleObject_->GetPosition().x - 14.f + 27), static_cast<int>(sampleObject_->GetPosition().y + 2.f + 38),
		static_cast<int>(GetPosition().x), static_cast<int>(GetPosition().y - 5.f), static_cast<int>(GetPosition().x + 33.f), static_cast<int>(GetPosition().y + 10.f)))
	{
		// キャラが上から乗ってきたら足場として機能する
		if (static_cast<int>(GetPosition().y + 10.f + 5.f) >= static_cast<int>(sampleObject_->GetPosition().y + 2.f + 38))
		{
			Vector2D vec;
			vec.x = sampleObject_->GetPosition().x + delta_position.x;
			vec.y = sampleObject_->GetPosition().y;
			Vector2D vec2;
			vec2.y = 0.f;
			sampleObject_->SetPosition(vec);
			sampleObject_->SetYAdd(vec2);
			sampleObject_->GroundBool();
		}
	}
}
