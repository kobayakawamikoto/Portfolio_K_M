#include "Enemy_Red.h"
#include "../Ground/Ground.h"
#include "../Source/Utility/BoxCollisionParams.h"
#include "../SampleObject/SampleObject.h"
#include "../Source/Scene/SampleScene/SampleScene.h"
#include <algorithm> // std::max���g�p���邽��
#define NOMINMAX
#include "DxLib.h"

Enemy_Red::Enemy_Red()
	: loaded_sprite_handle(0), chara_act(), isTurnLeft(false), motion_index(0), enemyState(), isAttack(false), ground(), bcp(), prev_x(), prev_y(), canJump(false), jumpTime(0), isJumpOnce(false), isJumping(false), isGround(false),
	isJumpKeyLeave(false), jumpCount(0), maxJumpCount(0), remainingJumps(0), sampleObject_(), addSpeed(3.f)
{
}

Enemy_Red::~Enemy_Red()
{
	Finalize();
}

void Enemy_Red::Initialize()
{
	__super::Initialize();

	// �摜�̓ǂݍ���
	loaded_sprite_handle = LoadDivGraph(_T("Resources/Images/akasuke.bmp"), 8, 4, 2, 40, 48, chara_act);
	// �T�E���h�̓ǂݍ���
	hitSound = LoadSoundMem("Resources/Sounds/hit.wav");
	deadSound = LoadSoundMem("Resources/Sounds/pound.wav");

	ground = new Ground();
	bcp = new BoxCollisionParams();

	gravity.y = .2f;
	maxJumpCount = 2;

	// �������W�ݒ�
	Vector2D initialVec;
	initialVec.x = 150.f;
	initialVec.y = 270.f;
	SetPosition(initialVec);

	// �T���v���V�[������T���v���I�u�W�F�N�g������Ă���
	sampleObject_ = SampleScene::GetSampleObject();
	hp = 3;
	prevPosition_x = position.x;
	prevPosition_y = position.y;
	isLeftDir = true;
}

void Enemy_Red::Update(float delta_seconds)
{
	__super::Update(delta_seconds);
	if (isDead)
	{
		return;
	}

	EnemyState2 currentState = enemyState;

	Vector2D input_dir;

	// �O��̈ʒu���o���Ă���
	prev_x = GetPosition().x;
	prev_y = GetPosition().y;

	// �v���C���[����������
	if (isFindPlayer)
	{
		if (GetPosition().x < sampleObject_->GetPosition().x)
		{
			Vector2D vec;
			vec.y = GetPosition().y;
			vec.x = GetPosition().x;
			vec.x += 1 * addSpeed;
			SetPosition(vec);
			isTurnLeft = false;
		}
		else
		{
			Vector2D vec;
			vec.y = GetPosition().y;
			vec.x = GetPosition().x;
			vec.x -= 1 * addSpeed;
			SetPosition(vec);
			isTurnLeft = true;
		}

		if (canAttack)
		{
			enemyState = EnemyState2::ATTACK;
			addSpeed = 5.8f;
		}
		else
		{
			enemyState = EnemyState2::RUN;
			addSpeed = 3.f;

		}
	}

	// ��������s��
	if (!isFindPlayer)
	{
		if (position.x >= prevPosition_x - 50.f && isLeftDir)
		{
			input_dir.x += -1;
			isTurnLeft = true;
			if (isGround == true && !isDamaged)
			{
				enemyState = EnemyState2::RUN;
			}
		}
		else if (position.x <= prevPosition_x + 50.f)
		{
			input_dir.x += 1;
			isTurnLeft = false;
			if (isGround == true && !isDamaged)
			{
				enemyState = EnemyState2::RUN;
			}
			isLeftDir = false;
		}
		else
		{
			isLeftDir = true;
		}
	}
	
	const float MOVEMENT_SPEED = 30.0f;
	delta_position = input_dir.Normalize() * MOVEMENT_SPEED * delta_seconds;
	SetPosition(GetPosition() + delta_position + yadd);
	yadd.y += gravity.y + 0.1f;
	yadd.y = std::min(10.0f, std::max(0.0f, yadd.y));

	// �v���C���[�����m
	if (isLeftDir)
	{
		if (bcp->CheckHit(static_cast<int>(sampleObject_->GetPosition().x - 14.f), static_cast<int>(sampleObject_->GetPosition().y + 2.f), static_cast<int>(sampleObject_->GetPosition().x - 14.f + 27), static_cast<int>(sampleObject_->GetPosition().y + 2.f + 38),
			static_cast<int>(GetPosition().x - 340.f), static_cast<int>(GetPosition().y - 10.f), static_cast<int>(GetPosition().x - 140.f + 300), static_cast<int>(GetPosition().y - 10.f + 32)))
		{
			isFindPlayer = true;
			if (bcp->CheckHit(static_cast<int>(sampleObject_->GetPosition().x - 14.f), static_cast<int>(sampleObject_->GetPosition().y + 2.f), static_cast<int>(sampleObject_->GetPosition().x - 14.f + 27), static_cast<int>(sampleObject_->GetPosition().y + 2.f + 38),
				static_cast<int>(GetPosition().x - 140.f), static_cast<int>(GetPosition().y - 10.f), static_cast<int>(GetPosition().x - 140.f + 120), static_cast<int>(GetPosition().y - 10.f + 32)))
			{
				canAttack = true;
			}
			else
			{
				canAttack = false;
			}
		}
		else
		{
			isFindPlayer = false;
		}
	}
	else
	{
		if (bcp->CheckHit(static_cast<int>(sampleObject_->GetPosition().x - 14.f), static_cast<int>(sampleObject_->GetPosition().y + 2.f), static_cast<int>(sampleObject_->GetPosition().x - 14.f + 27), static_cast<int>(sampleObject_->GetPosition().y + 2.f + 38),
			static_cast<int>(GetPosition().x - 14.f), static_cast<int>(GetPosition().y - 10.f), static_cast<int>(GetPosition().x - 14.f + 500), static_cast<int>(GetPosition().y - 10.f + 32)))
		{
			isFindPlayer = true;
			if (bcp->CheckHit(static_cast<int>(sampleObject_->GetPosition().x - 14.f), static_cast<int>(sampleObject_->GetPosition().y + 2.f), static_cast<int>(sampleObject_->GetPosition().x - 14.f + 27), static_cast<int>(sampleObject_->GetPosition().y + 2.f + 38),
				static_cast<int>(GetPosition().x - 14.f), static_cast<int>(GetPosition().y - 10.f), static_cast<int>(GetPosition().x - 14.f + 100), static_cast<int>(GetPosition().y - 10.f + 32)))
			{
				canAttack = true;
			}
			else
			{
				canAttack = false;
			}
		}
		else
		{
			isFindPlayer = false;
		}
	}
	

	// �v���C���[�ƐڐG
	if (bcp->CheckHit(static_cast<int>(sampleObject_->GetPosition().x - 14.f), static_cast<int>(sampleObject_->GetPosition().y + 2.f), static_cast<int>(sampleObject_->GetPosition().x - 14.f + 27), static_cast<int>(sampleObject_->GetPosition().y + 2.f + 38),
		static_cast<int>(GetPosition().x - 14.f), static_cast<int>(GetPosition().y - 10.f), static_cast<int>(GetPosition().x - 14.f + 27), static_cast<int>(GetPosition().y - 10.f + 32)))
	{
		sampleObject_->Dead();
	}

	// �e�ۂƐڐG
	for (int i = 0; i < 20; i++)
	{
		if (bcp->CheckHit(static_cast<int>(sampleObject_->GetBullet(i).x), static_cast<int>(sampleObject_->GetBullet(i).y), static_cast<int>(sampleObject_->GetBullet(i).x + 1), static_cast<int>(sampleObject_->GetBullet(i).y + 1),
			static_cast<int>(GetPosition().x - 14.f), static_cast<int>(GetPosition().y - 10.f), static_cast<int>(GetPosition().x - 14.f + 27), static_cast<int>(GetPosition().y - 10.f + 32)))
		{
			sampleObject_->SetBullet_x(i, 10000.f);
			sampleObject_->SetBullet_y(i, 10000.f);
			hp--;
			enemyState = EnemyState2::DAMAGE;
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

	// DAMAGE�X�e�[�g���b���ێ�����
	if (isDamaged)
	{
		damageTimer++;
		if (damageTimer > 5)
		{
			isDamaged = false;
			damageTimer = 0;
		}
	}

	// �u���b�N�̓����蔻��
	for (int j = 0; j < 12; j++) // �s��
	{
		for (int i = 0; i < ground->GetNUM_MAP_X(); i++) { // ��
			int no = ground->data[i + j * ground->GetNUM_MAP_X()];
			if (no != 7 && no != 11 && no != 15 && no != 19)
			{
				if (bcp->CheckHit(i * 32 - ground->GetMAP_OFFSET_X(), j * 40, i * 32 + 32 - ground->GetMAP_OFFSET_X(), j * 40 + 40, static_cast<int>(GetPosition().x - 14.f), static_cast<int>(GetPosition().y - 10.f), static_cast<int>(GetPosition().x - 14.f + 27), static_cast<int>(GetPosition().y - 10.f + 32)))
				{
					// �ォ�瓖�����Ă��邩�`�F�b�N
					if (GetPosition().y >= prev_y)
					{
						// �n�`�����ォ�瓖�����Ă��邩�`�F�b�N����
						// ���̏ꍇ�̂�Y��␳
						// �����łȂ��ꍇ�͉����瓖�����Ă���
						if (j * 40 - 21 >= prev_y)
						{
							// �C�n�`�ɏォ�瓖�����Ă���
							Vector2D vec;
							vec.x = prev_x;
							prev_y = static_cast<float>(j * 40 - 21);
							vec.y = prev_y;
							if (GetPosition().y != prev_y)
							{
								SetPosition(vec);
							}
							yadd.y = 0.0f;
							isGround = true;
						}
						else
						{
							// �D�n�`�̉����瓖�����Ă���
							// �}�X�N���邽��int���g�p���܂� (float���ƁA�����Ċ|����Ɠ����l�ɂȂ��Ă��܂�)
							Vector2D vec;
							vec.y = GetPosition().y;
							int _x = static_cast<int>(std::max(GetPosition().x, prev_x));
							_x /= 20;
							_x *= 20;
							vec.x = static_cast<float>(_x);
							if (GetPosition().x != _x) // (GetPosition().x == _x)�̏ꍇ�������ɂ��蔲���鏰�ɂȂ�
							{
								SetPosition(vec);
							}
						}
					}
					// �����瓖�����Ă��邩�`�F�b�N
					else if (GetPosition().y < prev_y)
					{
						// �n�`���������瓖�����Ă��邩�`�F�b�N����
						// ���̏ꍇ�̂�Y��␳
						// �����łȂ��ꍇ�͉����瓖�����Ă���
						if ((j + 1) * 40.f <= prev_y)
						{
							Vector2D vec;
							vec.x = prev_x;
							prev_y = (j + 1) * 40.f;
							vec.y = prev_y;
							if (GetPosition().y != prev_y)
							{
								SetPosition(vec);
							}
							yadd.y = 0.0f;
						}
						else
						{
							// �n�`�̉����瓖�����Ă���
							// �}�X�N���邽��int���g�p���܂� (float���ƁA�����Ċ|����Ɠ����l�ɂȂ��Ă��܂�)
							Vector2D vec;
							vec.y = GetPosition().y;
							int _x = static_cast<int>(std::max(GetPosition().x, prev_x));
							_x /= 20;
							_x *= 20;
							vec.x = static_cast<float>(_x);
							if (GetPosition().x != _x) // (GetPosition().x == _x)�̏ꍇ�������ɂ��蔲���鏰�ɂȂ�
							{
								SetPosition(vec);
							}
						}
					}
				}
			}
		}
	}


	if (enemyState != currentState) // �A�j���[�V�����t���[�������ꂼ��Ⴄ�̂ł����ŏ�����
	{
		act_index = 0;
	}
}

void Enemy_Red::Draw(const Vector2D& screen_offset)
{
	__super::Draw(screen_offset);

	int x, y;
	GetPosition().ToInt(x, y);
	Chara_AnimFrame();
	switch (enemyState)
	{
	case (EnemyState2::IDLE):
		motion_index = act_idle[act_index];
		max_motion_index = sizeof(act_idle) / sizeof(int);
		break;
	case (EnemyState2::RUN):
		motion_index = act_run[act_index];
		max_motion_index = sizeof(act_run) / sizeof(int);
		break;
	case (EnemyState2::ATTACK):
		motion_index = act_attack[act_index];
		max_motion_index = sizeof(act_attack) / sizeof(int);
		break;
	case (EnemyState2::JUMP):
		motion_index = act_jump[act_index];
		max_motion_index = sizeof(act_jump) / sizeof(int);
		break;
	case (EnemyState2::DAMAGE):
		motion_index = act_damage[act_index];
		max_motion_index = sizeof(act_damage) / sizeof(int);
		break;
	default:
		break;
	}

	// �L�����`��
	DrawRotaGraph(x - screen_offset.x, y - screen_offset.y, 1, 0, chara_act[motion_index], true, !isTurnLeft);

	/*
		if (isTurnLeft)
	{
		DrawBox(static_cast<int>(GetPosition().x - 340.f), static_cast<int>(GetPosition().y - 10.f), static_cast<int>(GetPosition().x - 140.f + 150), static_cast<int>(GetPosition().y - 10.f + 32), 100, false);
		if (canAttack)
		{
			DrawBox(static_cast<int>(GetPosition().x - 140.f), static_cast<int>(GetPosition().y - 10.f), static_cast<int>(GetPosition().x - 140.f + 120), static_cast<int>(GetPosition().y - 10.f + 32), 0, false);
		}
	}
	else
	{
		DrawBox(static_cast<int>(GetPosition().x - 14.f), static_cast<int>(GetPosition().y - 10.f), static_cast<int>(GetPosition().x - 14.f + 500), static_cast<int>(GetPosition().y - 10.f + 32), 100, false);
		if (canAttack)
		{
			DrawBox(static_cast<int>(GetPosition().x - 14.f), static_cast<int>(GetPosition().y - 10.f), static_cast<int>(GetPosition().x - 14.f + 100), static_cast<int>(GetPosition().y - 10.f + 32), 0, false);
		}
	}
	*/
	
	
	//DrawBox(static_cast<int>(GetPosition().x - 14.f), static_cast<int>(GetPosition().y - 10.f), static_cast<int>(GetPosition().x - 14.f + 27), static_cast<int>(GetPosition().y - 10.f + 32), 100, false);
	//DrawBox(SampleObject::PPosition_x - 14.f, SampleObject::PPosition_y + 2.f, SampleObject::PPosition_x - 14.f + 27, SampleObject::PPosition_y + 2.f + 38, 200, true);
	//DrawString(100, 150, hitTop.c_str(), GetColor(255, 255, 255));
	//DrawString(100, 200, hitSide.c_str(), GetColor(255, 255, 255));
}

void Enemy_Red::Finalize()
{
	__super::Finalize();

	// �摜�̔j��
	for (int i = 0; i < 10; i++)
	{
		DeleteGraph(chara_act[i]);
	}

	//SampleScene* sampleScene = SampleScene::GetSampleScene();
	//GetOwnerScene()->DestroyObject(this);
	//SampleScene* sampleScene = SampleScene::GetSampleScene();
	//sampleScene->DestroyObject(sampleScene->enemy_Purple);
	//delete(sampleScene->enemy_Purple);
}

void Enemy_Red::Chara_AnimFrame()
{
	if (--act_wait <= 0)
	{
		act_index++;
		act_wait = ACT_SPEED; // �A�j���[�V�����X�s�[�h
		act_index %= max_motion_index; // 0�ɖ߂�
	}
}