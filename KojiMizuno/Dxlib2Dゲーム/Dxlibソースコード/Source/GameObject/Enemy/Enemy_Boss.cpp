#include "Enemy_Boss.h"
#include "../Ground/Ground.h"
#include "../Source/Utility/BoxCollisionParams.h"
#include "../SampleObject/SampleObject.h"
#include "../Source/Scene/SampleScene/SampleScene.h"
#include <algorithm>
#include <random>
#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#define NOMINMAX
#include "DxLib.h"

Enemy_Boss::Enemy_Boss()
	: loaded_sprite_handle(0), chara_act(), enemyState(), ground(), bcp(), sampleObject_()
{
}

Enemy_Boss::~Enemy_Boss()
{
	Finalize();
}

void Enemy_Boss::Initialize()
{
	__super::Initialize();

	// �{�X�摜�̓ǂݍ���
	loaded_sprite_handle = LoadDivGraph(_T("Resources/Images/murasuke_boss.bmp"), 24, 4, 6, 40, 48, chara_act);

	// �T�E���h�ǂݍ���
	hitSound = LoadSoundMem("Resources/Sounds/hit.wav");
	deadSound = LoadSoundMem("Resources/Sounds/pound.wav");

	ground = new Ground();
	bcp = new BoxCollisionParams();

	// �C���X�^���X����
	for (int i = 0; i < apple_number; i++)
	{
		gimmick_apple[i] = new Gimmick_Apple();
		Vector2D vec;
		vec.x = -250.f;
		vec.y = -350.f;
		gimmick_apple[i]->Initialize(vec, -1.f, -1.f, 1);
		apple_pos[i] = vec;
	}

	// Attack1(�e��)�p�̃����_���ϐ�����
	for (int i = 0; i < apple_number + 1; i++)
	{
		std::random_device rd; // �n�[�h�E�F�A�̗����W�F�l���[�^���g�p���ăV�[�h�𐶐�
		std::mt19937 gen(rd()); // �����Z���k�E�c�C�X�^�@���g�p���闐��������
		std::uniform_real_distribution<float> distribution(-8.f, 8.f); // 1.0����10.0�܂ł�float�𐶐����镪�z

		random_number = distribution(gen); // �����_����float�𐶐�
		randomVal[i] = (random_number);
	}

	// �{�X���o�������S�e�̏������W�X�V
	Vector2D vec;
	vec.x = -150.f;
	vec.y = -350.f;
	//vec.x = GetPosition().x - 100.f;
	//vec.y = GetPosition().y - 250.f;
	gimmick_apple_2 = new Gimmick_Apple();
	gimmick_apple_2->Initialize(vec, -1.f, -1.f, 1);
	
	apple_pos_2 = vec;
	gimmick_apple_2->SetPosition(apple_pos_2);

	gravity.y = .2f;

	// �{�X�������W�ݒ�
	Vector2D initialVec;
	initialVec.x = -100.f;
	initialVec.y = -280.f;
	SetPosition(initialVec);

	// �T���v���V�[������T���v���I�u�W�F�N�g������Ă���
	sampleObject_ = SampleScene::GetSampleObject();

	// �����s���̂��ߍŏ��̈ʒu������Ă���
	prevPosition_x = position.x;
	prevPosition_y = position.y;
	isLeftDir = true;

	// �T�E���h�ǂݍ���
	fallSound = LoadSoundMem("Resources/Sounds/fall.wav");
}

void Enemy_Boss::Update(float delta_seconds)
{
	__super::Update(delta_seconds);
	if (isDead)
	{
		return;
	}

	EnemyState currentState = enemyState;

	// �������L���b�V��(Attack1�Ŏg�p)
	isPrevLeftDir = isLeftDir;

	// ��1�t���[���O�̈ʒu���o���Ă����A�R���W�����p�Ɏg��
	prev_x = GetPosition().x;
	prev_y = GetPosition().y;

	// �����ړ�
	Move(delta_seconds);

	// �v���C���[�ƐڐG
	HitPlayer();

	// �e�ۂƐڐG
	HitBullet();

	// �u���b�N�̓����蔻��
	DetectMapchip();

	// �U��
	if (sampleObject_->GetPosition().x < -15.f && sampleObject_->GetPosition().x > -600.f &&
		sampleObject_->GetPosition().y < -75.f && sampleObject_->GetPosition().y > -450.f)
	{
		if (hp > 15)
		{
			Attack1(delta_seconds);
		}
		else
		{
			Attack2(delta_seconds);
			apple_finalize();
		}
	}

	if (enemyState != currentState) // �A�j���[�V�����t���[�������ꂼ��Ⴄ�̂ł����ŏ�����
	{
		act_index = 0;
	}
}

void Enemy_Boss::Draw(const Vector2D& screen_offset)
{
	__super::Draw(screen_offset);

	// �摜�̕`��
	int x, y;
	GetPosition().ToInt(x, y);
	Chara_AnimFrame();
	switch (enemyState)
	{
	case (EnemyState::IDLE):
		motion_index = act_idle[act_index];
		max_motion_index = sizeof(act_idle) / sizeof(int);
		break;
	case (EnemyState::RUN):
		motion_index = act_run[act_index];
		max_motion_index = sizeof(act_run) / sizeof(int);
		break;
	case (EnemyState::ATTACK):
		motion_index = act_attack[act_index];
		max_motion_index = sizeof(act_attack) / sizeof(int);
		break;
	case (EnemyState::JUMP):
		motion_index = act_jump[act_index];
		max_motion_index = sizeof(act_jump) / sizeof(int);
		break;
	case (EnemyState::DAMAGE):
		motion_index = act_damage[act_index];
		max_motion_index = sizeof(act_damage) / sizeof(int);
		break;
	default:
		break;
	}

	// �{�X�L�����`��
	DrawExtendGraph(x - screen_offset.x - 300, y - screen_offset.y - 400, x - screen_offset.x + 150, y - screen_offset.y + 30, chara_act[motion_index], true);
	// �{�X�L�����̕`��͈͊m�F�p
	//DrawBox(x - screen_offset.x - 180, y - screen_offset.y - 200, x - screen_offset.x + 50, y - screen_offset.y + 30, 100, false);
	//DrawBox(x - screen_offset.x - 120, y - screen_offset.y - 280, x - screen_offset.x - 20, y - screen_offset.y - 200, 100, false);

	// �����S�e�`��
	for (int i = 0; i < apple_number; i++)
	{
		gimmick_apple[i]->Draw(screen_offset);
	}
	if (hp < 15)
	{
		gimmick_apple_2->Draw(screen_offset);
	}
}

/*
*	�I�����Ăяo�����
*/
void Enemy_Boss::Finalize()
{
	__super::Finalize();

	// �摜�̔j��
	for (int i = 0; i < apple_number; i++)
	{
		DeleteGraph(chara_act[i]);
	}
	gimmick_apple_2->Finalize();
}

/*
*	�A�j���[�V�����t���[���X�s�[�h�̐ݒ�
*/
void Enemy_Boss::Chara_AnimFrame()
{
	// �A�j���[�V�����t���[���̃X�s�[�h��ݒ肵�Ă��܂�
	if (--act_wait <= 0)
	{
		act_index++;
		act_wait = ACT_SPEED;
		act_index %= max_motion_index;
	}
}

/*
*	���������̑��i�K
*	�����_���e��
*/
void Enemy_Boss::Attack1(float delta_seconds)
{
	// �����̂��тɒe���U�������܂�
	if (isPrevLeftDir == isLeftDir)
	{
		for (int i = 0; i < apple_number; ++i) {
			apple_pos[i].x += randomVal[i];
			apple_pos[i].y += randomVal[i + 1];
			gimmick_apple[i]->SetPosition(apple_pos[i]);
			gimmick_apple[i]->Update(delta_seconds);
		}
	}
	else
	{
		ChangeVolumeSoundMem(255 * 30 / 100, fallSound);
		PlaySoundMem(fallSound, DX_PLAYTYPE_BACK);

		// �e�������ʒu�ɖ߂�
		for (int i = 0; i < apple_number; ++i)
		{
			Vector2D vec;
			vec.x = GetPosition().x - 100.f;
			vec.y = GetPosition().y - 250.f;
			apple_pos[i] = vec;
		}
		// �e���̐i�ޕ������U�����ƂɃ����_���Ō��߂�
		for (int i = 0; i < apple_number + 1; i++)
		{
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_real_distribution<float> distribution(-8.f, 8.f);

			random_number = distribution(gen);
			randomVal[i] = (random_number);
		}
	}
}

/*
*	���������̑��i�K
*	�ǔ��e
*/
void Enemy_Boss::Attack2(float delta_seconds)
{
	// �����S�ƃv���C���[�����Ԓ����𕽍s�ړ������āA���_����݂��������W�ֈړ�(atan2�֐����g�p���邽��)
	double dx = sampleObject_->GetPosition().x - apple_pos_2.x;
	double dy = sampleObject_->GetPosition().y - apple_pos_2.y;
	// atan2�֐����g�p���Ċp�x���v�Z����
	double angle = atan2(dy, dx);
	// sin/cos�֐����g�p���āA�p�x��-1~1�̐��l�ɕϊ����āA�|�W�V�����ɉ��Z���Ă���
	apple_pos_2.x += static_cast<int>(3 * cos(angle));
	apple_pos_2.y += static_cast<int>(3 * sin(angle));
	gimmick_apple_2->SetPosition(apple_pos_2);
	gimmick_apple_2->Update(delta_seconds);
}

/*
*	�����S�������ĉ��
*/
void Enemy_Boss::apple_finalize()
{
	for (int i = 0; i < apple_number; i++)
	{
		gimmick_apple[i]->Finalize();
	}
}

/*
*	�v���C���[�ƐڐG������
*/
void Enemy_Boss::HitPlayer()
{
	int chara_x = static_cast<int>(sampleObject_->GetPosition().x - 14.f);
	int chara_y = static_cast<int>(sampleObject_->GetPosition().y + 2.f);
	int chara_width = static_cast<int>(sampleObject_->GetPosition().x - 14.f + 27);
	int chara_height = static_cast<int>(sampleObject_->GetPosition().y + 2.f + 38);
	int enemy_x = static_cast<int>(GetPosition().x - 180.f);
	int enemy_y = static_cast<int>(GetPosition().y - 200.f);
	int enemy_width = static_cast<int>(GetPosition().x + 50);
	int enemy_height = static_cast<int>(GetPosition().y + 30);

	// ���̂̃R���W�����͈͂Əd�Ȃ�΃v���[���[������
	if (bcp->CheckHit(chara_x, chara_y, chara_width, chara_height,
		enemy_x, enemy_y, enemy_width, enemy_height))
	{
		sampleObject_->Dead();
	}
	// ���̃R���W�����͈͂Əd�Ȃ�΃v���[���[������
	enemy_x = static_cast<int>(GetPosition().x - 120.f);
	enemy_y = static_cast<int>(GetPosition().y - 280.f);
	enemy_width = static_cast<int>(GetPosition().x - 20);
	enemy_height = static_cast<int>(GetPosition().y - 200);
	if (bcp->CheckHit(chara_x, chara_y, chara_width, chara_height,
		enemy_x, enemy_y, enemy_width, enemy_height))
	{
		sampleObject_->Dead();
	}
}

/*
*	�e�ɔ�e������
*/
void Enemy_Boss::HitBullet()
{
	for (int i = 0; i < 20; i++)
	{
		int bullet_x = static_cast<int>(sampleObject_->GetBullet(i).x);
		int bullet_y = static_cast<int>(sampleObject_->GetBullet(i).y);
		int bullet_width = static_cast<int>(sampleObject_->GetBullet(i).x + 1);
		int bullet_height = static_cast<int>(sampleObject_->GetBullet(i).y + 1);
		int enemy_x = static_cast<int>(GetPosition().x - 120.f);
		int enemy_y = static_cast<int>(GetPosition().y - 280.f);
		int enemy_width = static_cast<int>(GetPosition().x - 20);
		int enemy_height = static_cast<int>(GetPosition().y - 200);

		// �e�ƃL�������d�Ȃ�����HP�����炷
		if (bcp->CheckHit(bullet_x, bullet_y, bullet_width, bullet_height,
			enemy_x, enemy_y, enemy_width, enemy_height))
		{
			sampleObject_->SetBullet_x(i, 10000.f);
			sampleObject_->SetBullet_y(i, 10000.f);
			hp--;
			enemyState = EnemyState::DAMAGE;
			isDamaged = true;
			if (hp <= 0)
			{
				ChangeVolumeSoundMem(255 * 30 / 100, deadSound);
				PlaySoundMem(deadSound, DX_PLAYTYPE_BACK);
				Finalize();
				isDead = true;
			}
			else
			{
				ChangeVolumeSoundMem(255 * 30 / 100, hitSound);
				PlaySoundMem(hitSound, DX_PLAYTYPE_BACK);
			}
		}
	}
	// ��莞��DAMAGE�X�e�[�g���ێ�����
	if (isDamaged)
	{
		damageTimer++;
		if (damageTimer > 5)
		{
			isDamaged = false;
			damageTimer = 0;
		}
	}
}

/*
*	�}�b�v�`�b�v�ɑ΂���R���W��������
*/
void Enemy_Boss::DetectMapchip()
{
	for (int j = 0; j < ground->GetNUM_MAP_Y(); j++) // �s������
	{
		for (int i = 0; i < ground->GetNUM_MAP_X(); i++) { // �񐔕���
			int no = ground->data[i + j * ground->GetNUM_MAP_X()];
			if (no != 7 && no != 11 && no != 15 && no != 19)
			{
				int mapchip_x = i * ground->GetPOS_MAP_X() - ground->GetMAP_OFFSET_X();
				int mapchip_y = j * ground->GetPOS_MAP_Y() - ground->GetMAP_OFFSET_Y();
				int mapchip_width = i * ground->GetPOS_MAP_X() + ground->GetPOS_MAP_WIDTH() - ground->GetMAP_OFFSET_X();
				int mapchip_height = j * ground->GetPOS_MAP_Y() + ground->GetPOS_MAP_HEIGHT() - ground->GetMAP_OFFSET_Y();
				int enemy_x = static_cast<int>(GetPosition().x - 14.f);
				int enemy_y = static_cast<int>(GetPosition().y - 10.f);
				int enemy_width = static_cast<int>(GetPosition().x - 14.f + 27);
				int enemy_height = static_cast<int>(GetPosition().y - 10.f + 32);

				// �L�����ƃ}�b�v�`�b�v�ɑ΂���R���W��������
				if (bcp->CheckHit(mapchip_x, mapchip_y, mapchip_width, mapchip_height,
					enemy_x, enemy_y, enemy_width, enemy_height))
				{
					// �ォ�瓖�����Ă��邩�`�F�b�N
					if (GetPosition().y >= prev_y)
					{
						/*
						*	�}�b�v�`�b�v�����ォ�瓖�����Ă��邩�`�F�b�N����
						*	���̏ꍇ�̂�y���W���Œ肷��
						*/
						// �}�b�v�`�b�v�ɏォ�瓖�����Ă���ꍇ�̃|�W�V�����ݒ�
						if ((j * 40 - 21) - ground->GetMAP_OFFSET_Y() >= prev_y)
						{
							Vector2D vec;
							vec.x = prev_x;
							prev_y = static_cast<float>((j * 40 - 21) - ground->GetMAP_OFFSET_Y());
							vec.y = prev_y;
							if (GetPosition().y != prev_y)
							{
								SetPosition(vec);
							}
							// �e��ϐ��̕ύX
							yadd.y = 0.0f; // �d��
							canJump = true; // �W�����v�\���ǂ���
							isJumping = false; // ���݃W�����v���Ă��邩
							isJumpOnce = false; // ��x�W�����v�������ǂ���
							isGround = true; // �n�ʂɐڒn���Ă��邩
							jumpTime = 0; // �W�����v���Ă��鎞��(���̕b���ŏ�ɏオ�邩���ɉ����邩�����߂Ă���)
							jump.y = 0; // y���W�̃x���V�e�B
						}
						// �}�b�v�`�b�v�ɏォ�炩�����瓖�����Ă���ꍇ�̃|�W�V�����ݒ�
						else
						{
							Vector2D vec;
							vec.y = GetPosition().y;
							int _x = static_cast<int>(std::max(GetPosition().x, prev_x));
							_x /= 20;
							_x *= 20;
							vec.x = static_cast<float>(_x);
							if (GetPosition().x != _x)
							{
								SetPosition(vec);
							}
						}
					}
					// �����瓖�����Ă��邩�`�F�b�N
					else if (GetPosition().y < prev_y)
					{
						/*
						*	�}�b�v�`�b�v���������瓖�����Ă��邩�`�F�b�N����
						*	���̏ꍇ�̂�y���W���Œ肷��
						*/
						// �}�b�v�`�b�v�ɉ����瓖�����Ă���ꍇ�̃|�W�V�����ݒ�
						if (((j + 1) * 40.f) - ground->GetMAP_OFFSET_Y() <= prev_y)
						{
							Vector2D vec;
							vec.x = prev_x;
							prev_y = ((j + 1) * 40.f) - ground->GetMAP_OFFSET_Y();
							vec.y = prev_y;
							if (GetPosition().y != prev_y)
							{
								SetPosition(vec);
							}
							yadd.y = 0.0f;
						}
						// �}�b�v�`�b�v�ɉ����炩�����瓖�����Ă���ꍇ�̃|�W�V�����ݒ�
						else
						{
							Vector2D vec;
							vec.y = GetPosition().y;
							int _x = static_cast<int>(std::max(GetPosition().x, prev_x));
							_x /= 20;
							_x *= 20;
							vec.x = static_cast<float>(_x);
							if (GetPosition().x != _x)
							{
								SetPosition(vec);
							}
						}
					}
				}
			}
		}
	}
}

/*
*	���E�ɉ������鏈��
*/
void Enemy_Boss::Move(float delta_seconds)
{
	Vector2D input_dir;
	if (position.x >= prevPosition_x - 50.f && isLeftDir)
	{
		input_dir.x += -1;
		isTurnLeft = true;
		if (isGround == true && !isDamaged)
		{
			enemyState = EnemyState::RUN;
		}
	}
	else if (position.x <= prevPosition_x + 50.f)
	{
		input_dir.x += 1;
		isTurnLeft = false;
		if (isGround == true && !isDamaged)
		{
			enemyState = EnemyState::RUN;
		}
		isLeftDir = false;
	}
	else
	{
		isLeftDir = true;
	}

	// �|�W�V�����̐ݒ�
	delta_position = input_dir.Normalize() * MOVEMENT_SPEED * delta_seconds;
	SetPosition(GetPosition() + delta_position + yadd);
	yadd.y += gravity.y + 0.1f; // �d�͂̐ݒ�
	yadd.y = std::min(10.0f, std::max(0.0f, yadd.y));
}
