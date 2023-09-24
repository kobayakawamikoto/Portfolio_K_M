#include "Gimmick_Apple.h"
#include "../Ground/Ground.h"
#include "../Source/Utility/BoxCollisionParams.h"
#include "../SampleObject/SampleObject.h"
#include "../Source/Scene/SampleScene/SampleScene.h"
#include <algorithm> // std::maxを使用するため
#define NOMINMAX
#include "DxLib.h"

Gimmick_Apple::Gimmick_Apple()
{

}

Gimmick_Apple::~Gimmick_Apple()
{
	Finalize();
}

void Gimmick_Apple::Initialize(Vector2D initialVec, float add_x, float add_y, int direction)
{
	__super::Initialize();

	sampleObject_ = SampleScene::GetSampleObject();
	bcp = new BoxCollisionParams();

	// 初期設定
	//Vector2D initialVec;
	//initialVec.x = 200.f;
	//initialVec.y = 340.f;
	SetPosition(initialVec);
	add_x_ = add_x;
	add_y_ = add_y;
	direction_ = direction;

	// 画像の読み込み
	loaded_apple = LoadGraph("Resources/Images/apple.png");
	// サウンド読み込み
	fallSound = LoadSoundMem("Resources/Sounds/fall.wav");
}

void Gimmick_Apple::Update(float delta_seconds)
{
	__super::Update(delta_seconds);

	// 下に落下
	if (isPlayerChecked)
	{
		position.x += add_x_;
		position.y += add_y_;
	}

	Vector2D input_dir;

	// プレイヤーを検知
	// directionはテンキーに対応 例:2は下 6は右
	if (direction_ == 1)
	{
		DetectPlayer_1();
	}
	if (direction_ == 2)
	{
		DetectPlayer_2();
	}
	if (direction_ == 3)
	{
		DetectPlayer_3();
	}
	if (direction_ == 4)
	{
		DetectPlayer_4();
	}
	if (direction_ == 6)
	{
		DetectPlayer_6();
	}
	if (direction_ == 7)
	{
		DetectPlayer_7();
	}
	if (direction_ == 8)
	{
		DetectPlayer_8();
	}
	if (direction_ == 9)
	{
		DetectPlayer_9();
	}

	// プレイヤーと接触したら
	if (bcp->CheckHit(static_cast<int>(sampleObject_->GetPosition().x - 14.f), static_cast<int>(sampleObject_->GetPosition().y + 2.f), static_cast<int>(sampleObject_->GetPosition().x - 14.f + 27), static_cast<int>(sampleObject_->GetPosition().y + 2.f + 38),
		static_cast<int>(GetPosition().x - 14.f), static_cast<int>(GetPosition().y - 10.f), static_cast<int>(GetPosition().x - 14.f + 20), static_cast<int>(GetPosition().y - 10.f + 20)))
	{
		sampleObject_->Dead();
	}
}

void Gimmick_Apple::Draw(const Vector2D& screen_offset)
{
	__super::Draw(screen_offset);

	// 画像の描画
	int x, y;
	GetPosition().ToInt(x, y);
	// 画像描画
	DrawGraph(static_cast<int>(GetPosition().x - 14.f - screen_offset.x), static_cast<int>(GetPosition().y - 10.f - screen_offset.y), loaded_apple, true);
	//DrawBox(static_cast<int>(GetPosition().x - 14.f - screen_offset.x), static_cast<int>(GetPosition().y - 10.f - screen_offset.y),
		//static_cast<int>(GetPosition().x - 14.f + 20 - screen_offset.x), static_cast<int>(GetPosition().y - 10.f + 20 - screen_offset.y), 100, false);
	//DrawBox(static_cast<int>(GetPosition().x - 14.f - screen_offset.x), static_cast<int>(GetPosition().y - 10.f - 100 - screen_offset.y),
		//static_cast<int>(GetPosition().x - 14.f + 20 - screen_offset.x), static_cast<int>(GetPosition().y - 10.f -100 + 200 - screen_offset.y), 100, false);
}

void Gimmick_Apple::Finalize()
{
	__super::Finalize();
}

void Gimmick_Apple::DetectPlayer_1()
{
}

void Gimmick_Apple::DetectPlayer_2()
{
	if (bcp->CheckHit(static_cast<int>(sampleObject_->GetPosition().x - 14.f), static_cast<int>(sampleObject_->GetPosition().y + 2.f), static_cast<int>(sampleObject_->GetPosition().x - 14.f + 27), static_cast<int>(sampleObject_->GetPosition().y + 2.f + 38),
		static_cast<int>(GetPosition().x - 14.f), static_cast<int>(GetPosition().y - 10.f), static_cast<int>(GetPosition().x - 14.f + 20), static_cast<int>(GetPosition().y - 10.f + 100)))
	{
		if (!isPlayerChecked)
		{
			ChangeVolumeSoundMem(255 * 30 / 100, fallSound);
			PlaySoundMem(fallSound, DX_PLAYTYPE_BACK);
		}

		isPlayerChecked = true;
	}
}

void Gimmick_Apple::DetectPlayer_3()
{
}

void Gimmick_Apple::DetectPlayer_4()
{
}

void Gimmick_Apple::DetectPlayer_6()
{
	if (bcp->CheckHit(static_cast<int>(sampleObject_->GetPosition().x - 14.f), static_cast<int>(sampleObject_->GetPosition().y + 2.f), static_cast<int>(sampleObject_->GetPosition().x - 14.f + 27), static_cast<int>(sampleObject_->GetPosition().y + 2.f + 38),
		static_cast<int>(GetPosition().x - 14.f), static_cast<int>(GetPosition().y - 10.f), static_cast<int>(GetPosition().x - 14.f + 150), static_cast<int>(GetPosition().y - 10.f + 40)))
	{
		if (!isPlayerChecked)
		{
			ChangeVolumeSoundMem(255 * 30 / 100, fallSound);
			PlaySoundMem(fallSound, DX_PLAYTYPE_BACK);
		}

		isPlayerChecked = true;
	}
}

void Gimmick_Apple::DetectPlayer_7()
{
	if (bcp->CheckHit(static_cast<int>(sampleObject_->GetPosition().x - 14.f), static_cast<int>(sampleObject_->GetPosition().y + 2.f), static_cast<int>(sampleObject_->GetPosition().x - 14.f + 27), static_cast<int>(sampleObject_->GetPosition().y + 2.f + 38),
		static_cast<int>(GetPosition().x - 14.f - 60), static_cast<int>(GetPosition().y - 10.f - 100), static_cast<int>(GetPosition().x - 14.f - 60 + 20), static_cast<int>(GetPosition().y - 10.f - 100 + 100)))
	{
		if (!isPlayerChecked)
		{
			ChangeVolumeSoundMem(255 * 30 / 100, fallSound);
			PlaySoundMem(fallSound, DX_PLAYTYPE_BACK);
		}

		isPlayerChecked = true;
	}
}

void Gimmick_Apple::DetectPlayer_8()
{
	if (bcp->CheckHit(static_cast<int>(sampleObject_->GetPosition().x - 14.f), static_cast<int>(sampleObject_->GetPosition().y + 2.f), static_cast<int>(sampleObject_->GetPosition().x - 14.f + 27), static_cast<int>(sampleObject_->GetPosition().y + 2.f + 38),
		static_cast<int>(GetPosition().x - 14.f), static_cast<int>(GetPosition().y - 10.f - 100), static_cast<int>(GetPosition().x - 14.f + 20), static_cast<int>(GetPosition().y - 10.f - 100 + 100)))
	{
		if (!isPlayerChecked)
		{
			ChangeVolumeSoundMem(255 * 30 / 100, fallSound);
			PlaySoundMem(fallSound, DX_PLAYTYPE_BACK);
		}

		isPlayerChecked = true;
	}
}

void Gimmick_Apple::DetectPlayer_9()
{
}
