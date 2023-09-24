#include "Gimmick_GoalFlag.h"
#include "../Ground/Ground.h"
#include "../Source/Utility/BoxCollisionParams.h"
#include "../SampleObject/SampleObject.h"
#include "../Source/Scene/SampleScene/SampleScene.h"
#include <algorithm> // std::max���g�p���邽��
#define NOMINMAX
#include "DxLib.h"

Gimmick_GoalFlag::Gimmick_GoalFlag()
{

}

Gimmick_GoalFlag::~Gimmick_GoalFlag()
{
	Finalize();
}

void Gimmick_GoalFlag::Initialize()
{
	__super::Initialize();
	sampleObject_ = SampleScene::GetSampleObject();
	bcp = new BoxCollisionParams();

	// �������W�ݒ�
	Vector2D initialVec;
	initialVec.x = 1220.f;
	initialVec.y = 70.f;
	SetPosition(initialVec);

	// �摜�̓ǂݍ���
	loaded_goal_flag = LoadGraph("Resources/Images/goal_flag.bmp");
}

void Gimmick_GoalFlag::Update(float delta_seconds)
{
	__super::Update(delta_seconds);

	// ������
	Vector2D input_dir;

	// �v���C���[�ƐڐG
	if (bcp->CheckHit(static_cast<int>(sampleObject_->GetPosition().x - 14.f), static_cast<int>(sampleObject_->GetPosition().y + 2.f), static_cast<int>(sampleObject_->GetPosition().x - 14.f + 27), static_cast<int>(sampleObject_->GetPosition().y + 2.f + 38),
		static_cast<int>(GetPosition().x - 14.f), static_cast<int>(GetPosition().y - 10.f), static_cast<int>(GetPosition().x - 14.f + 27), static_cast<int>(GetPosition().y - 10.f + 80)))
	{
		sampleObject_->Clear();
	}
}

void Gimmick_GoalFlag::Draw(const Vector2D& screen_offset)
{
	__super::Draw(screen_offset);

	// �摜�̕`��
	int x, y;
	GetPosition().ToInt(x, y);
	DrawGraph(static_cast<int>(GetPosition().x - 14.f - screen_offset.x), static_cast<int>(GetPosition().y - 10.f - screen_offset.y), loaded_goal_flag, true);
	//DrawBox(static_cast<int>(GetPosition().x - 14.f - screen_offset.x), static_cast<int>(GetPosition().y - 10.f - screen_offset.y),
	//	static_cast<int>(GetPosition().x - 14.f + 27 - screen_offset.x), static_cast<int>(GetPosition().y - 10.f + 60 - screen_offset.y), 100, false);
}

void Gimmick_GoalFlag::Finalize()
{
	__super::Finalize();


}