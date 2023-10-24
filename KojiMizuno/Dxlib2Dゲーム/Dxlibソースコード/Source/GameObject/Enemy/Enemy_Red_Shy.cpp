#include "Enemy_Red_Shy.h"
#include "../Ground/Ground.h"
#include "../Source/Utility/BoxCollisionParams.h"
#include "../SampleObject/SampleObject.h"
#include "../Source/Scene/SampleScene/SampleScene.h"
#include <algorithm>
#define NOMINMAX
#include "DxLib.h"

Enemy_Red_Shy::Enemy_Red_Shy()
	: loaded_sprite_handle(0), chara_act(), enemyState(), ground(), bcp(), sampleObject_()
{
}

Enemy_Red_Shy::~Enemy_Red_Shy()
{
	Finalize();
}

void Enemy_Red_Shy::Initialize()
{
	__super::Initialize();

	// �摜�ǂݍ���
	loaded_sprite_handle = LoadDivGraph(_T("Resources/Images/akasuke.bmp"), 8, 4, 2, 40, 48, chara_act);
	loaded_parent_handle = LoadGraph(_T("Resources/Images/akasuke_wait.bmp"));
	// �T�E���h�ǂݍ���
	hitSound = LoadSoundMem("Resources/Sounds/hit.wav");
	deadSound = LoadSoundMem("Resources/Sounds/pound.wav");
	parentSound = LoadSoundMem("Resources/Sounds/splatter.wav");

	ground = new Ground();
	bcp = new BoxCollisionParams();

	gravity.y = .2f;

	// �������W�ݒ�
	Vector2D initialVec;
	initialVec.x = 1150.f;
	initialVec.y = 270.f;
	SetPosition(initialVec);

	// �T���v���V�[������T���v���I�u�W�F�N�g������Ă���
	sampleObject_ = SampleScene::GetSampleObject();
	prevPosition_x = position.x;
	prevPosition_y = position.y;
}

void Enemy_Red_Shy::Update(float delta_seconds)
{
	__super::Update(delta_seconds);

	EnemyState currentState = enemyState;

	// �O��̈ʒu���o���Ă���
	prev_x = GetPosition().x;
	prev_y = GetPosition().y;

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

void Enemy_Red_Shy::Draw(const Vector2D& screen_offset)
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

	// �L�����`��
	if (isDead)
	{
		DrawExtendGraph(x - screen_offset.x - 300, y - screen_offset.y - 400, x - screen_offset.x + 100, y - screen_offset.y, chara_act[motion_index], true);
	}
	else
	{
		DrawRotaGraph(x - screen_offset.x, y - screen_offset.y, 1, 0, chara_act[motion_index], true, !isTurnLeft);
		DrawExtendGraph(1200 - screen_offset.x - change_parent_pos_x, 200 - screen_offset.y - change_parent_pos_y, 1500 - screen_offset.x - change_parent_pos_x, 500 - screen_offset.y - change_parent_pos_y, loaded_parent_handle, true);
	}
}

void Enemy_Red_Shy::Finalize()
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
void Enemy_Red_Shy::Chara_AnimFrame()
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
void Enemy_Red_Shy::Patrol(float delta_seconds)
{
	Vector2D input_dir;
	// �v���C���[����������ǂ�������
	if (isFindPlayer)
	{
		if (!isJumpOnce)
		{
			jumpTime++;

			if (jumpTime < 10.f)
			{
				Vector2D vec;
				vec.x = GetPosition().x;
				vec.y += GetPosition().y + -5.f;
				SetPosition(vec);
			}
			else
			{
				isJumpOnce = true;
			}
		}
		else
		{
			if (position.x <= prevPosition_x + 80.f)
			{
				MOVEMENT_SPEED = 120.f;
				input_dir.x += 1;
				isTurnLeft = false;
			}
		}
	}

	// ��������s��
	if (!isFindPlayer)
	{
		if (position.x >= prevPosition_x - 8.f && isLeftDir)
		{
			input_dir.x += -1;
			isTurnLeft = true;
			if (isGround == true && !isDamaged)
			{
				enemyState = EnemyState::RUN;
			}
		}
		else if (position.x <= prevPosition_x + 8.f)
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
void Enemy_Red_Shy::DetectPlayer()
{
	if (isTurnLeft)
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
	}
}

/*
*	�v���C���[�ƐڐG������
*/
void Enemy_Red_Shy::HitPlayer()
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
void Enemy_Red_Shy::HitBullet()
{
	// �e�ƃL�������d�Ȃ�����HP�����炷s
	for (int i = 0; i < 20; i++)
	{
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
				isDead = true;
				enemyState = EnemyState::ATTACK;
				ChangeVolumeSoundMem(255 * 30 / 100, parentSound);
				sampleObject_->Dead();
			}
			else
			{
				ChangeVolumeSoundMem(255 * 30 / 100, hitSound);
				PlaySoundMem(hitSound, DX_PLAYTYPE_BACK);
				change_parent_pos_x += 30;
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
void Enemy_Red_Shy::DetectMapchip()
{
	for (int j = 0; j < ground->GetNUM_MAP_Y(); j++) // �s������
	{
		for (int i = 0; i < ground->GetNUM_MAP_X(); i++) { // �񐔕���
			int no = ground->data[i + j * ground->GetNUM_MAP_X()];
			if (no != 7 && no != 11 && no != 15 && no != 19)
			{
				// �L�����ƃ}�b�v�`�b�v�ɑ΂���R���W��������
				if (bcp->CheckHit(i * 32 - ground->GetMAP_OFFSET_X(), (j * 40) - ground->GetMAP_OFFSET_Y(), i * 32 + 32 - ground->GetMAP_OFFSET_X(), (j * 40 + 40) - ground->GetMAP_OFFSET_Y(), static_cast<int>(GetPosition().x - 14.f), static_cast<int>(GetPosition().y - 10.f), static_cast<int>(GetPosition().x - 14.f + 27), static_cast<int>(GetPosition().y - 10.f + 32)))
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
						if ((j + 1) * 40.f <= prev_y)
						{
							Vector2D vec;
							vec.x = prev_x;
							prev_y = ((j + 1) - ground->GetMAP_OFFSET_Y()) * 40.f;
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
