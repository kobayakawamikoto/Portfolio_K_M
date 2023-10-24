#include "Platform_Passable.h"
#include "../Source/Utility/BoxCollisionParams.h"
#include "../SampleObject/SampleObject.h"
#include "../Source/Scene/SampleScene/SampleScene.h"
#include <algorithm> // std::max���g�p���邽��
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

	// �摜�̓ǂݍ���
	loaded_sprite_handle = LoadGraph(_T("Resources/Images/platform_passable2.bmp"));

	bcp = new BoxCollisionParams();

	// �������W�ݒ�
	SetPosition(initialVec);
	radian_period = radian_period_;
	speed = speed_;
	// �T���v���V�[������T���v���I�u�W�F�N�g������Ă���
	sampleObject_ = SampleScene::GetSampleObject();
	prevPosition_x = position.x;
	prevPosition_y = position.y;
	isLeftDir = true;
}

void Platform_Passable::Update(float delta_seconds)
{
	__super::Update(delta_seconds);

	// �O��̈ʒu���o���Ă���
	prev_x = GetPosition().x;
	prev_y = GetPosition().y;

	// �����ړ�
	Move(delta_seconds);

	// �v���C���[�ƐڐG
	HitPlayer();
}

void Platform_Passable::Draw(const Vector2D& screen_offset)
{
	__super::Draw(screen_offset);

	int x, y;
	GetPosition().ToInt(x, y);
	Chara_AnimFrame();


	// �L�����`��
	DrawGraph(x - screen_offset.x, y - screen_offset.y, loaded_sprite_handle, true);

	//DrawBox(static_cast<int>(GetPosition().x), static_cast<int>(GetPosition().y - 5.f), static_cast<int>(GetPosition().x + 33.f), static_cast<int>(GetPosition().y + 10.f), 100, false);
	//DrawFormatString(0, 0, 100, "platform_pos_x = %f", GetPosition().x);
}

void Platform_Passable::Finalize()
{
	__super::Finalize();

	// �摜�̔j��
	DeleteGraph(loaded_sprite_handle);
}

/*
*	�A�j���[�V�����t���[���X�s�[�h�̐ݒ�
*/
void Platform_Passable::Chara_AnimFrame()
{
	if (--act_wait <= 0)
	{
		act_index++;
		act_wait = ACT_SPEED; // �A�j���[�V�����X�s�[�h
	}
}

/*
*	�T�C���֐����g���đ�������E�ɓ�����
*/
void Platform_Passable::Move(float delta_seconds)
{
	Vector2D input_dir;

	// radian_period�����Z���Ă���
	radian_x += radian_period;
	// sin�֐���x���W���v�Z
	input_dir.x = speed * sin(radian_x * 0.01);
	
	delta_position = input_dir * MOVEMENT_SPEED * delta_seconds;
	// x���W��ύX
	SetPosition(GetPosition() + delta_position);
}

/*
*	�v���C���[�ƐڐG������
*/
void Platform_Passable::HitPlayer()
{
	// �v���C���[�ƐڐG��true
	if (bcp->CheckHit(static_cast<int>(sampleObject_->GetPosition().x - 14.f), static_cast<int>(sampleObject_->GetPosition().y + 2.f), static_cast<int>(sampleObject_->GetPosition().x - 14.f + 27), static_cast<int>(sampleObject_->GetPosition().y + 2.f + 38),
		static_cast<int>(GetPosition().x), static_cast<int>(GetPosition().y - 5.f), static_cast<int>(GetPosition().x + 33.f), static_cast<int>(GetPosition().y + 10.f)))
	{
		// �L�������ォ�����Ă����瑫��Ƃ��ċ@�\����
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
