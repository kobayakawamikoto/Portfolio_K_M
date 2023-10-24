#include "Enemy_Red.h"
#include "../Ground/Ground.h"
#include "../Source/Utility/BoxCollisionParams.h"
#include "../SampleObject/SampleObject.h"
#include "../Source/Scene/SampleScene/SampleScene.h"
#include <algorithm>
#define NOMINMAX
#include "DxLib.h"

Enemy_Red::Enemy_Red()
	: loaded_sprite_handle(0), enemyState(), ground(), bcp()
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

	// �������W�ݒ�
	Vector2D initialVec;
	initialVec.x = 150.f;
	initialVec.y = 270.f;
	SetPosition(initialVec);

	// �T���v���V�[������T���v���I�u�W�F�N�g������Ă���
	sampleObject_ = SampleScene::GetSampleObject();
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

	EnemyState currentState = enemyState;

	// �O��̈ʒu���o���Ă���
	prev_x = GetPosition().x;
	prev_y = GetPosition().y;

	// ���[�e�B���s��
	Patrol(delta_seconds);

	// �v���C���[�����m
	DetectPlayer();

	// �v���C���[�ƐڐG
	HitPlayer();

	// �e�ۂƐڐG
	HitBullet();

	// �u���b�N�̓����蔻��
	DetectMapchip();

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

	// �L�����`��
	DrawRotaGraph(x - screen_offset.x, y - screen_offset.y, 1, 0, chara_act[motion_index], true, !isTurnLeft);
}

void Enemy_Red::Finalize()
{
	__super::Finalize();

	// �摜�̔j��
	for (int i = 0; i < 10; i++)
	{
		DeleteGraph(chara_act[i]);
	}
}

/*
*	�A�j���[�V�����t���[���X�s�[�h�̐ݒ�
*/
void Enemy_Red::Chara_AnimFrame()
{
	if (--act_wait <= 0)
	{
		act_index++;
		act_wait = ACT_SPEED; // �A�j���[�V�����X�s�[�h
		act_index %= max_motion_index; // 0�ɖ߂�
	}
}

/*
*	���E�ɉ������鏈��
*/
void Enemy_Red::Patrol(float delta_seconds)
{
	Vector2D input_dir;
	// �v���C���[����������ǂ�������
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
			enemyState = EnemyState::ATTACK;
			addSpeed = 5.8f;
		}
		else
		{
			enemyState = EnemyState::RUN;
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
	}

	// �|�W�V�����̐ݒ�
	delta_position = input_dir.Normalize() * MOVEMENT_SPEED * delta_seconds;
	SetPosition(GetPosition() + delta_position + yadd);
	yadd.y += gravity.y + 0.1f; // �d�͂̐ݒ�
	yadd.y = std::min(10.0f, std::max(0.0f, yadd.y));
}

/*
*	�v���C���[�������e��ϐ���ύX����
*/
void Enemy_Red::DetectPlayer()
{
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
}

/*
*	�v���C���[�ƐڐG������
*/
void Enemy_Red::HitPlayer()
{
	// �G�̃R���W�����͈͂Əd�Ȃ�΃v���[���[������
	if (bcp->CheckHit(static_cast<int>(sampleObject_->GetPosition().x - 14.f), static_cast<int>(sampleObject_->GetPosition().y + 2.f), static_cast<int>(sampleObject_->GetPosition().x - 14.f + 27), static_cast<int>(sampleObject_->GetPosition().y + 2.f + 38),
		static_cast<int>(GetPosition().x - 14.f), static_cast<int>(GetPosition().y - 10.f), static_cast<int>(GetPosition().x - 14.f + 27), static_cast<int>(GetPosition().y - 10.f + 32)))
	{
		sampleObject_->Dead();
	}
}

/*
*	�e�ɔ�e������
*/
void Enemy_Red::HitBullet()
{
	for (int i = 0; i < 20; i++)
	{
		// �e�ƃL�������d�Ȃ�����HP�����炷
		if (bcp->CheckHit(static_cast<int>(sampleObject_->GetBullet(i).x), static_cast<int>(sampleObject_->GetBullet(i).y), static_cast<int>(sampleObject_->GetBullet(i).x + 1), static_cast<int>(sampleObject_->GetBullet(i).y + 1),
			static_cast<int>(GetPosition().x - 14.f), static_cast<int>(GetPosition().y - 10.f), static_cast<int>(GetPosition().x - 14.f + 27), static_cast<int>(GetPosition().y - 10.f + 32)))
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
}

/*
*	�}�b�v�`�b�v�ɑ΂���R���W��������
*/
void Enemy_Red::DetectMapchip()
{
	for (int j = 0; j < ground->GetNUM_MAP_Y(); j++) // �s������
	{
		for (int i = 0; i < ground->GetNUM_MAP_X(); i++) { // �񐔕���
			int no = ground->data[i + j * ground->GetNUM_MAP_X()];
			if (no != 7 && no != 11 && no != 15 && no != 19)
			{
				// �L�����ƃ}�b�v�`�b�v�ɑ΂���R���W��������
				if (bcp->CheckHit(i * 32 - ground->GetMAP_OFFSET_X(), (j * 40) - ground->GetMAP_OFFSET_Y(), i * 32 + 32 - ground->GetMAP_OFFSET_X(), (j * 40 + 40) - ground->GetMAP_OFFSET_Y(),
					static_cast<int>(GetPosition().x - 14.f), static_cast<int>(GetPosition().y - 10.f), static_cast<int>(GetPosition().x - 14.f + 27), static_cast<int>(GetPosition().y - 10.f + 32)))
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
							yadd.y = 0.0f;
							isGround = true;
						}
						// �}�b�v�`�b�v�ɏォ�炩�����瓖�����Ă���ꍇ�̃|�W�V�����ݒ�
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
						if ((j + 1) * 40.f <= prev_y)
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
