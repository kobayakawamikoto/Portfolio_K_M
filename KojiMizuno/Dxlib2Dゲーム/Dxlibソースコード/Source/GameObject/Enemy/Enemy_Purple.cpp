#include "Enemy_Purple.h"
#include "../Ground/Ground.h"
#include "../Source/Utility/BoxCollisionParams.h"
#include "../SampleObject/SampleObject.h"
#include "../Source/Scene/SampleScene/SampleScene.h"
#include <algorithm> // std::max���g�p���邽��
#define NOMINMAX
#include "DxLib.h"

Enemy_Purple::Enemy_Purple()
	: loaded_sprite_handle(0), chara_act(), enemyState(), ground(), bcp(), sampleObject_()
{
}

Enemy_Purple::~Enemy_Purple()
{
	Finalize();
}

void Enemy_Purple::Initialize()
{
	__super::Initialize();

	// �摜�̓ǂݍ���
	loaded_sprite_handle = LoadDivGraph(_T("Resources/Images/murasuke.bmp"), 12, 4, 3, 40, 48, chara_act);

	// �T�E���h�ǂݍ���
	hitSound = LoadSoundMem("Resources/Sounds/hit.wav");
	deadSound = LoadSoundMem("Resources/Sounds/pound.wav");

	ground = new Ground();
	bcp = new BoxCollisionParams();
	gravity.y = .2f;

	// �������W�ݒ�
	Vector2D initialVec;
	initialVec.x = 500.f;
	initialVec.y = 400.f;
	SetPosition(initialVec);

	// �T���v���V�[������T���v���I�u�W�F�N�g������Ă���
	sampleObject_ = SampleScene::GetSampleObject();

	// �����s���̂��ߍŏ��̈ʒu������Ă���
	prevPosition_x = position.x;
	prevPosition_y = position.y;
	isLeftDir = true;
}

void Enemy_Purple::Update(float delta_seconds)
{
	__super::Update(delta_seconds);
	if (isDead)
	{
		return;
	}

	EnemyState currentState = enemyState;

	Vector2D input_dir;
	
	// ��1�t���[���O�̈ʒu���o���Ă����A�R���W�����p�Ɏg��
	prev_x = GetPosition().x;
	prev_y = GetPosition().y;
	
	// ���E�ɉ�������
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
	
	delta_position = input_dir.Normalize() * MOVEMENT_SPEED * delta_seconds;
	SetPosition(GetPosition() + delta_position + yadd);
	yadd.y += gravity.y + 0.1f;
	yadd.y = std::min(10.0f, std::max(0.0f, yadd.y));

	// �v���C���[�ƐڐG
	DetectPlayer();

	// �e�ۂƐڐG
	HitBullet();

	// �u���b�N�̓����蔻��
	
	


	if (enemyState != currentState) // �A�j���[�V�����t���[�������ꂼ��Ⴄ�̂ł����ŏ�����
	{
		act_index = 0;
	}
}

void Enemy_Purple::Draw(const Vector2D& screen_offset)
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
	DrawRotaGraph(x + screen_offset.x, y + screen_offset.y, 1, 0, chara_act[motion_index], true, !isTurnLeft);

	//DrawFormatString(0, 0, 100, "�����̓V�C�� %f �ł�", prev_x);
	//DrawBox(static_cast<int>(GetPosition().x - 14.f), static_cast<int>(GetPosition().y - 10.f), static_cast<int>(GetPosition().x - 14.f + 27), static_cast<int>(GetPosition().y - 10.f + 32), 100, false);
	//DrawBox(SampleObject::PPosition_x - 14.f, SampleObject::PPosition_y + 2.f, SampleObject::PPosition_x - 14.f + 27, SampleObject::PPosition_y + 2.f + 38, 200, true);
	//DrawString(100, 150, hitTop.c_str(), GetColor(255, 255, 255));
	//DrawString(100, 200, hitSide.c_str(), GetColor(255, 255, 255));
}

void Enemy_Purple::Finalize()
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

void Enemy_Purple::Chara_AnimFrame()
{
	if (--act_wait <= 0)
	{
		act_index++;
		act_wait = ACT_SPEED; // �A�j���[�V�����X�s�[�h
		act_index %= max_motion_index; // 0�ɖ߂�
	}
}

void Enemy_Purple::DetectPlayer()
{
	int chara_x = static_cast<int>(sampleObject_->GetPosition().x - 14.f);
	int chara_y = static_cast<int>(sampleObject_->GetPosition().y + 2.f);
	int chara_width = static_cast<int>(sampleObject_->GetPosition().x - 14.f + 27);
	int chara_height = static_cast<int>(sampleObject_->GetPosition().y + 2.f + 38);
	int enemy_x = static_cast<int>(GetPosition().x - 14.f);
	int enemy_y = static_cast<int>(GetPosition().y - 10.f);
	int enemy_width = static_cast<int>(GetPosition().x - 14.f + 27);
	int enemy_height = static_cast<int>(GetPosition().y - 10.f + 32);
	if (bcp->CheckHit(chara_x, chara_y, chara_width, chara_height,
		enemy_x, enemy_y, enemy_width, enemy_height))
	{
		sampleObject_->Dead();
	}
}

void Enemy_Purple::HitBullet()
{
	for (int i = 0; i < 20; i++)
	{
		int bullet_x = static_cast<int>(sampleObject_->GetBullet(i).x);
		int bullet_y = static_cast<int>(sampleObject_->GetBullet(i).y);
		int bullet_width = static_cast<int>(sampleObject_->GetBullet(i).x + 1);
		int bullet_height = static_cast<int>(sampleObject_->GetBullet(i).y + 1);
		int enemy_x = static_cast<int>(GetPosition().x - 14.f);
		int enemy_y = static_cast<int>(GetPosition().y - 10.f);
		int enemy_width = static_cast<int>(GetPosition().x - 14.f + 27);
		int enemy_height = static_cast<int>(GetPosition().y - 10.f + 32);
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

void Enemy_Purple::DetectMapchip()
{
	for (int j = 0; j < ground->GetNUM_MAP_Y(); j++) // �s��
	{
		for (int i = 0; i < ground->GetNUM_MAP_X(); i++) { // ��
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
				if (bcp->CheckHit(mapchip_x, mapchip_y, mapchip_width, mapchip_height,
					enemy_x, enemy_y, enemy_width, enemy_height))
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
							canJump = true;
							isJumping = false;
							isJumpKeyLeave = false;
							isJumpOnce = false;
							isGround = true;
							jumpTime = 0;
							jump.y = 0;
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
}
