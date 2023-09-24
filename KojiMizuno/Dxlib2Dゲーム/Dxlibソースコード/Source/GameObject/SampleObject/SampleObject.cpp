#include "SampleObject.h"
#include "../Ground/Ground.h"
#include "../Source/Utility/BoxCollisionParams.h"
#include "../Source/Utility/SavePosition.h"
#include "../Source/Scene/SampleScene/SampleScene.h"
#include <algorithm> // std::max���g�p���邽��
#include <random>
#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#define NOMINMAX
#include "DxLib.h"

// �R���W������t�������ꏊ���蓮�Őݒ� ��1����:x���W, ��Q����:y���W, ��R����:x���W+width, ��S����:y���W+height
int colPosition[][4] = { { 0, 440, 800, 480 }, {512, 200, 512+32, 200+160} };


SampleObject::SampleObject()
	: loaded_sprite_handle(), chara_act(), motion_index(0), playerState(), playerDirection(), ground(), bcp(),
	 jumpCount(0)
{
}

SampleObject::~SampleObject()
{
	Finalize();
}

void SampleObject::Initialize()
{
	__super::Initialize();

	// �L�����摜�̓ǂݍ���
	loaded_sprite_handle = LoadDivGraph(_T("Resources/Images/collon.bmp"), 27, 4, 7, 128, 128, chara_act);
	loaded_gameover_handle = LoadGraph("Resources/Images/gameover.png");
	// �����ʒu�ݒ�
	Vector2D initialVec;
	initialVec.x = 100.f;
	initialVec.y = 400.f;
	SetPosition(initialVec);
	// ���݂̍��W��SavePosition�N���X�Ɋi�[
	savePosition = SavePosition::GetSavePosition();
	if (savePosition->GetCachePosition().x != 0 && savePosition->GetCachePosition().y != 0)
	{
		SetPosition(savePosition->GetCachePosition());
	}

	ground = new Ground();
	bcp = new BoxCollisionParams();
	
	gravity.y = .2f; // �d��

	// �T�E���h�ǂݍ���
	gameOverSound = LoadSoundMem("Resources/Sounds/gameover.mp3");
	fireSound = LoadSoundMem("Resources/Sounds/fire2.wav");
	jump1Sound = LoadSoundMem("Resources/Sounds/jump1.wav");
	jump2Sound = LoadSoundMem("Resources/Sounds/jump2.wav");
	clearSound = LoadSoundMem("Resources/Sounds/clear.ogg");
	bgm1 = LoadSoundMem("Resources/Sounds/12.ogg");
	ChangeVolumeSoundMem(255 * 30 / 100, bgm1);
	PlaySoundMem(bgm1, DX_PLAYTYPE_LOOP);
	SetFontSize(20);

	// ���̉摜�ǂݍ���
	for (int i = 0; i <= 10; i++)
	{
		// �t�@�C�����𐶐�
		std::stringstream filename;
		filename << "Resources/Images/b_collon" << i + 1 << ".bmp";

		// �摜�����[�h���A�n���h����z��Ɋi�[
		loaded_bcollon_handle[i] = LoadGraph(filename.str().c_str());
	}
	// ���̉摜���̈ʒu�𐶐�
	for (int i = 0; i < blood_num + 1; i++)
	{
		std::random_device rd; // �n�[�h�E�F�A�̗����W�F�l���[�^���g�p���ăV�[�h�𐶐�
		std::mt19937 gen(rd()); // �����Z���k�E�c�C�X�^�@���g�p���闐��������
		std::uniform_real_distribution<float> distribution(-20.0, 20.0); // 1.0����10.0�܂ł�float�𐶐����镪�z

		random_number = distribution(gen); // �����_����float�𐶐�
		if (random_number <= .8f && random_number >= -.8f)
		{
			random_number = distribution(gen);
		}
		randomVal[i] = (random_number);
	}
}

void SampleObject::Update(float delta_seconds)
{
	__super::Update(delta_seconds);

	if (isCleared)
	{
		return;
	}

	PlayerState currentState = playerState;
	Vector2D input_dir;

	// �O��̈ʒu���o���Ă���
	prev_x = GetPosition().x;
	prev_y = GetPosition().y;


	int leftButtonState = GetMouseInput();
	// �{�^���������ꂽ�u�Ԃ����m
	if ((prevLeftButtonState & MOUSE_INPUT_LEFT) && !(leftButtonState & MOUSE_INPUT_LEFT)) {
		// �{�^���������ꂽ�Ƃ��̏����������ɋL�q����
		canShot = true;
	}
	prevLeftButtonState = leftButtonState; // ���݂̏�Ԃ�ۑ�


	if (CheckHitKey(KEY_INPUT_A) == 1) // �E�ړ�
	{
		input_dir.x = -1;
		isTurnLeft = true;
		if (isGround == true && !isAttacked)
		{
			playerState = PlayerState::RUN;
		}
		playerDirection = PlayerDirection::LEFT;
	}
	else if (CheckHitKey(KEY_INPUT_D) == 1) // ���ړ�
	{
		input_dir.x = 1;
		isTurnLeft = false;
		if (isGround == true && !isAttacked)
		{
			playerState = PlayerState::RUN;
		}
		playerDirection = PlayerDirection::RIGHT;
	}
	else
	{
		if (isGround == true && !isAttacked)
		{
			playerState = PlayerState::IDLE;
		}
	}

	// �ˌ�
	Attack(delta_seconds);
	
	// �Q�i�W�����v
	Jump();
	
	// �A�j���[�V�����t���[�������ꂼ��Ⴄ�̂ł����ŏ�����
	if (playerState != currentState)
	{
		act_index = 0;
	}

	const float MOVEMENT_SPEED = 300.0f;
	delta_position = input_dir.Normalize() * MOVEMENT_SPEED * delta_seconds;
	SetPosition(GetPosition() + delta_position + yadd); // �L�������W�̍X�V
	yadd.y += gravity.y + 10 * delta_seconds; // deltaSeconds�ŉσt���[���Ή�
	yadd.y = std::min(20.0f, std::max(0.0f, yadd.y));

	
	/*
	** �R���W��������^�C�v�P
	* �C�ӂ̏ꏊ�Ɏ蓮�ŃR���W������ݒ肷��
	*/
	//DetectManualMapchip();
	
	/*
	** �R���W��������^�C�v�Q
	* �}�b�v�`�b�v�̔ԍ��ŃR���W�������肷��
	*/
	DetectMapchip();

	// �����\��
	Bleed();
}

void SampleObject::Draw(const Vector2D& screen_offset)
{
	__super::Draw(screen_offset);
	int x, y;
	GetPosition().ToInt(x, y);
	// �A�j���[�V�����̑����ƃA�j���[�V�����̃t���[���ԍ��X�V
	Chara_AnimFrame();
	// �X�e�[�g�ŃA�j���[�V��������E�`��
	switch (playerState)
	{
	case (PlayerState::IDLE):
		motion_index = act_idle[act_index];
		max_motion_index = sizeof(act_idle) / sizeof(int);
		break;
	case (PlayerState::RUN):
		motion_index = act_run[act_index];
		max_motion_index = sizeof(act_run) / sizeof(int);
		break;
	case (PlayerState::ATTACK):
		motion_index = act_attack[act_index];
		max_motion_index = sizeof(act_attack) / sizeof(int);
		break;
	case (PlayerState::JUMP):
		motion_index = act_jump[act_index];
		max_motion_index = sizeof(act_jump) / sizeof(int);
		break;
	default:
		break;
	}

	// �L�����`��
	if (isDead)
	{
		// ���S������L�����`�悳���Ȃ�
		Finalize();
		// �ްѵ�ް�摜�\��
		DrawExtendGraph(0, 0, 640, 480, loaded_gameover_handle, true);
	}
	else
	{
		DrawRotaGraph(x - screen_offset.x, y - screen_offset.y, 1, 0, chara_act[motion_index], true, isTurnLeft);
	}

	// ���S�摜�`��
	if (isDead)
	{
		// ���̕`��
		for (int i = 0; i < blood_num; i++)
		{
			DrawCircle(bloodVec[i].x - screen_offset.x, bloodVec[i].y - screen_offset.y, 1, GetColor(255, 0, 0), TRUE);
		}
		// �o���o���̃R���������`��
		DrawGraph(bloodVec[10].x - screen_offset.x, bloodVec[10].y - screen_offset.y, loaded_bcollon_handle[0], true);
		DrawGraph(bloodVec[20].x - screen_offset.x, bloodVec[20].y - screen_offset.y, loaded_bcollon_handle[1], true);
		DrawGraph(bloodVec[30].x - screen_offset.x, bloodVec[30].y - screen_offset.y, loaded_bcollon_handle[2], true);
		DrawGraph(bloodVec[40].x - screen_offset.x, bloodVec[40].y - screen_offset.y, loaded_bcollon_handle[3], true);
		DrawGraph(bloodVec[50].x - screen_offset.x, bloodVec[50].y - screen_offset.y, loaded_bcollon_handle[4], true);
		DrawGraph(bloodVec[60].x - screen_offset.x, bloodVec[60].y - screen_offset.y, loaded_bcollon_handle[5], true);
		DrawGraph(bloodVec[70].x - screen_offset.x, bloodVec[70].y - screen_offset.y, loaded_bcollon_handle[6], true);
		DrawGraph(bloodVec[80].x - screen_offset.x, bloodVec[80].y - screen_offset.y, loaded_bcollon_handle[7], true);
		DrawGraph(bloodVec[90].x - screen_offset.x, bloodVec[90].y - screen_offset.y, loaded_bcollon_handle[8], true);
		DrawGraph(bloodVec[1].x  - screen_offset.x, bloodVec[1].y  - screen_offset.y, loaded_bcollon_handle[9], true);
		DrawGraph(bloodVec[2].x  - screen_offset.x, bloodVec[2].y  - screen_offset.y, loaded_bcollon_handle[10], true);
	}

	// �e�摜�`��
	for (int i = 0; i < 20; i++)
	{
		DrawCircle(bullet[i].x - screen_offset.x, bullet[i].y - screen_offset.y, 1, GetColor(255, 255, 255), TRUE);
	}
	
	//DrawFormatString(0, 0, 100, "addyadd = %f", std::min(addyadd, 20.f));
	//DrawFormatString(0, 100, 100, "�����̓V�C�� %f �ł�", GetPosition().x);
	//DrawFormatString(0, 100, 100, "�d�͉����l: %f ", yadd.y);
	//DrawBox(static_cast<int>(GetPosition().x - 14.f), static_cast<int>(GetPosition().y + 2.f), static_cast<int>(GetPosition().x - 14.f + 27), static_cast<int>(GetPosition().y + 2.f + 38), 100, true);
	//DrawBox(colPosition[0][0], colPosition[0][1], colPosition[0][2], 480, 400, true);
	//DrawBox(colPosition[1][0], colPosition[1][1], colPosition[1][2], colPosition[1][3], 400, true);
	
	float pos_x = GetPosition().x;
	float pos_y = GetPosition().y;
	//int mapchip_x = i * ground->GetPOS_MAP_X() - ground->GetMAP_OFFSET_X();
	//int mapchip_y = j * ground->GetPOS_MAP_Y() - ground->GetMAP_OFFSET_Y();
	//int mapchip_width = i * ground->GetPOS_MAP_X() + ground->GetPOS_MAP_WIDTH() - ground->GetMAP_OFFSET_X();
	//int mapchip_height = j * ground->GetPOS_MAP_Y() + ground->GetPOS_MAP_HEIGHT() - ground->GetMAP_OFFSET_Y();
	int chara_x = static_cast<int>(pos_x - offset_pos_x);
	int chara_y = static_cast<int>(pos_y + offset_pos_y);
	int chara_width = static_cast<int>(pos_x - offset_pos_x + offset_pos_width);
	int chara_height = static_cast<int>(pos_y + offset_pos_y + offset_pos_height);
	DrawBox(chara_x, chara_y, chara_width, chara_height, 400, true);
}

void SampleObject::Finalize()
{
	__super::Finalize();

	// �摜�̔j��
	DeleteGraph(loaded_sprite_handle);
	loaded_sprite_handle = 0;
}

void SampleObject::Chara_AnimFrame()
{
	if (--act_wait <= 0)
	{
		act_index++;
		act_wait = ACT_SPEED; // �A�j���[�V�����X�s�[�h
		act_index %= max_motion_index; // 0�ɖ߂�
	}
}

void SampleObject::Attack(float delta_seconds)
{
	// �e�̔���(���N���b�N)
	if (GetMouseInput() & MOUSE_INPUT_LEFT && canShot && !isDead)
	{
		for (int i = 0; i < 20; ++i) {
			if (bullet[i].flag == false) {
				bullet[i].flag = true;
				bullet[i].x = position.x;
				bullet[i].y = position.y + 20;
				bullet[i].pre_x = position.x;
				bullet[i].playerDirection = playerDirection;
				canShot = false;
				break;
			}
		}
		playerState = PlayerState::ATTACK;
		isAttacked = true;
		ChangeVolumeSoundMem(255 * 30 / 100, fireSound);
		PlaySoundMem(fireSound, DX_PLAYTYPE_BACK);
	}

	// �A�ˊԊu����
	if (isAttacked)
	{
		attackTimer++;
		if (attackTimer > 5)
		{
			isAttacked = false;
			attackTimer = 0;
		}
	}


	for (int i = 0; i < 20; ++i) {
		if (bullet[i].flag && bullet[i].playerDirection == PlayerDirection::LEFT) {
			bullet[i].x -= 1000.f * delta_seconds;

			// �e�̔򋗗��Ƃ��̂��Ƃ̏���
			if (bullet[i].x < bullet[i].pre_x - 500.f) {
				bullet[i].flag = false;
				bullet[i].x = 10000;
				bullet[i].y = 10000;
			}
		}
	}

	for (int i = 0; i < 20; ++i) {
		if (bullet[i].flag && bullet[i].playerDirection == PlayerDirection::RIGHT) {
			bullet[i].x += 1000.f * delta_seconds;

			// �e�̔򋗗��Ƃ��̂��Ƃ̏���
			if (bullet[i].x > bullet[i].pre_x + 500.f) {
				bullet[i].flag = false;
				bullet[i].x = 10000;
				bullet[i].y = 10000;
			}
		}
	}
}

void SampleObject::Jump()
{
	if (CheckHitKey(KEY_INPUT_SPACE) == 1 && canJump == true && !isDead)
	{
		if (!isGround && !isAttacked)
		{
			playerState = PlayerState::JUMP;
		}
		jumpTime++;
		if (!isJumping)
		{
			if (!isJumpOnce)
			{
				isGround = false; // TODO: ���ꂾ�ƍ����Ƃ��납��~�肽�Ƃ���Jump���[�V�����Ɉڍs���Ȃ�
				remainingJumps = maxJumpCount - 1;
				ChangeVolumeSoundMem(255 * 20 / 100, jump1Sound);
				PlaySoundMem(jump1Sound, DX_PLAYTYPE_BACK);
			}

			if (jumpTime < 10.f)
			{
				isJumpOnce = true;
				jump.y += -3.f;
				SetPosition(GetPosition() + jump);
			}
		}
		else if (isJumpKeyLeave)
		{
			act_index = 0;
			if (!isJumpOnce)
			{
				remainingJumps = maxJumpCount - 1;
				if (!isDead)
				{
					yadd.y = 0;
				}
				ChangeVolumeSoundMem(255 * 20 / 100, jump2Sound);
				PlaySoundMem(jump2Sound, DX_PLAYTYPE_BACK);
			}

			if (jumpTime < 10.f)
			{
				isJumpOnce = true;
				remainingJumps = remainingJumps - 1;
				jump.y += -3.f;
				SetPosition(GetPosition() + jump);
			}
			else
			{
				canJump = false;
				jumpTime = 0;
				jump.y = 0;
			}
		}
		else
		{
			canJump = false;
			jumpTime = 0;
			jump.y = 0;
		}
	}
	else
	{
		isJumping = true;
		isJumpKeyLeave = true;
		if (remainingJumps > 0)
		{
			jumpTime = 0;
		}
		jump.y = 0;
		isJumpOnce = false;
	}
}

void SampleObject::DetectManualMapchip()
{
	/*
	for (int i = 0; i < 3; i++)
	{
		// �����蔻��̐���
		if (bcp->CheckHit(colPosition[i][0], colPosition[i][1], colPosition[i][2], colPosition[i][3], static_cast<int>(GetPosition().x - 14.f), static_cast<int>(GetPosition().y + 2.f), static_cast<int>(GetPosition().x - 14.f + 27), static_cast<int>(GetPosition().y + 2.f + 38)))
		{
			// �ォ�瓖�����Ă��邩�`�F�b�N
			if (GetPosition().y >= prev_y)
			{
				// �n�`�����ォ�瓖�����Ă��邩�`�F�b�N����
				// ���̏ꍇ�̂�Y��␳
				// �����łȂ��ꍇ�͉����瓖�����Ă���
				if (colPosition[i][1] - 38 >= prev_y)
				{
					// �C�n�`�ɏォ�瓖�����Ă���
					Vector2D vec;
					vec.x = prev_x;
					prev_y = static_cast<float>(colPosition[i][1] - 38);
					vec.y = prev_y;
					if (GetPosition().y != prev_y)
					{
						SetPosition(vec);
					}
					if (!isDead)
					{
						yadd.y = 0.0f;
					}
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
				if (colPosition[i][3] <= prev_y + 2)
				{
					Vector2D vec;
					vec.x = prev_x;
					prev_y = colPosition[i][3];
					vec.y = prev_y;
					if (GetPosition().y != prev_y)
					{
						SetPosition(vec);
					}
					if (!isDead)
					{
						yadd.y = 0.0f;
					}
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
	}*/
}

void SampleObject::DetectMapchip()
{
	for (int j = 0; j < 12; j++) // �s��
	{
		for (int i = 0; i < ground->GetNUM_MAP_X(); i++) { // ��
			// ground�N���X�̃}�b�v�p�z��̐���������Ă��ăR���W�����v�Z������
			int no = ground->data[i + j * ground->GetNUM_MAP_X()];
			if (no != 7 && no != 11 && no != 15 && no != 19)
			{
				// �}�b�v�ƃL�����̃R���W�����p���W�v�Z
				int mapchip_x = i * ground->GetPOS_MAP_X() - ground->GetMAP_OFFSET_X();
				int mapchip_y = j * ground->GetPOS_MAP_Y() - ground->GetMAP_OFFSET_Y();
				int mapchip_width = i * ground->GetPOS_MAP_X() + ground->GetPOS_MAP_WIDTH() - ground->GetMAP_OFFSET_X();
				int mapchip_height = j * ground->GetPOS_MAP_Y() + ground->GetPOS_MAP_HEIGHT() - ground->GetMAP_OFFSET_Y();
				int chara_x = static_cast<int>(GetPosition().x - offset_pos_x);
				int chara_y = static_cast<int>(GetPosition().y + offset_pos_y);
				int chara_width = static_cast<int>(GetPosition().x - offset_pos_x + offset_pos_width);
				int chara_height = static_cast<int>(GetPosition().y + offset_pos_y + offset_pos_height);

				// �����蔻�肪����ꍇtrue
				if (bcp->CheckHit(mapchip_x, mapchip_y, mapchip_width, mapchip_height, chara_x, chara_y, chara_width, chara_height))
				{
					// �L�������}�b�v�`�b�v�̏ォ��L�������������Ă��邩���P�t���[���O�̍��W�Ɣ�r���ă`�F�b�N
					if (GetPosition().y >= prev_y)
					{
						// �R���W�����̂���}�b�v�`�b�v������ł���ꍇ������A�������Ē����`�̓����蔻��ɂ���
						if (ground->data[i + j * ground->GetNUM_MAP_X()] != ground->data[i + (j - 1) * ground->GetNUM_MAP_X()])
						{
							// �}�b�v�`�b�v���ォ��L�������������Ă���
							if (j * 40 - 38 >= prev_y)
							{
								HitFromAbove(j);
							}
							else // �}�b�v�`�b�v��艡����L�������������Ă���
							{
								HitFromSide();
							}
						}
						else // �}�b�v�`�b�v��艡����L�������������Ă���
						{
							HitFromSide();
						}
					}
					// �����瓖�����Ă��邩�`�F�b�N
					else if (GetPosition().y < prev_y)
					{
						// �R���W�����̂���}�b�v�`�b�v������ł���ꍇ������A�������Ē����`�̓����蔻��ɂ���
						if (ground->data[i + j * ground->GetNUM_MAP_X()] != ground->data[i + (j + 1) * ground->GetNUM_MAP_X()])
						{
							// �}�b�v�`�b�v��艺����L�������������Ă���
							if ((j + 1) * 40.f <= prev_y + 2)
							{
								HitFromBelow(j);
							}
							else // �}�b�v�`�b�v��艡����L�������������Ă���
							{
								HitFromSide();
							}
						}
						else // �}�b�v�`�b�v��艡����L�������������Ă���
						{
							HitFromSide();
						}
					}
				}
			}
		}
	}
}

void SampleObject::HitFromAbove(int j) // �}�b�v�`�b�v�̏ォ��L��������������
{
	Vector2D vec;
	vec.x = prev_x;
	prev_y = static_cast<float>(j * 40 - 38); // y���W���Œ肷��
	vec.y = prev_y;
	if (GetPosition().y != prev_y)
	{
		SetPosition(vec);
	}
	if (!isDead)
	{
		yadd.y = 0.0f;
	}
	// �W�����v�ƃx���V�e�B�̃u�[���l�X�V
	canJump = true;
	isJumping = false;
	isJumpKeyLeave = false;
	isJumpOnce = false;
	isGround = true;
	jumpTime = 0;
	jump.y = 0;
}

void SampleObject::HitFromBelow(int j) // �}�b�v�`�b�v�̉�����L��������������
{
	Vector2D vec;
	vec.x = GetPosition().x;
	prev_y = (j + 1) * 40.f;
	vec.y = prev_y;
	if (GetPosition().y != prev_y) // y���W���Œ肷��
	{
		SetPosition(vec);
	}
	if (!isDead)
	{
		yadd.y = 0.0f;
	}
}

void SampleObject::HitFromSide() // �}�b�v�`�b�v�̉�����L��������������
{
	Vector2D vec;
	vec.y = GetPosition().y;
	int _x = 0;

	if (playerDirection == PlayerDirection::RIGHT)
	{
		_x = static_cast<int>(std::min(GetPosition().x, prev_x));
	}
	else
	{
		_x = static_cast<int>(std::max(GetPosition().x, prev_x));
	}
	// �����߂��p(�}�X�N���邽��int���g�p���܂�)
	_x /= 5;
	_x *= 5;
	vec.x = static_cast<float>(_x);
	if (GetPosition().x != _x) // (GetPosition().x == _x)�̏ꍇ�������ɂ��蔲���鏰�ɂȂ�
	{
		SetPosition(vec);
	}
}

void SampleObject::Bleed()
{
	if (isDead)
	{
		for (int i = 0; i < blood_num + 1; i++)
		{
			prev_blood_x[i] = bloodVec[i].x;
			prev_blood_y[i] = bloodVec[i].y;
		}
	}

	// ���̍��W�X�V�E�R���W��������
	if (isDead)
	{
		// ���̗ʂ����������������_�����W�����ɓK�p
		for (int i = 0; i < blood_num; i++)
		{
			bloodVec[i].x += randomVal[i];
			bloodVec[i].y += randomVal[i + 1];
			// ���̃A�j���[�V�����ׂ̈ɐV���ȃx���V�e�B���쐬���ēK�p���Ă��܂�(addyadd)�B
			addyadd += yadd.y * 0.00001;
			bloodVec[i].y += std::min(addyadd, 20.f);
		}

		// ���ƃ}�b�v�`�b�v�̃R���W�����𔻒�
		for (int j = 0; j < 12; j++) // �s��
		{
			for (int i = 0; i < ground->GetNUM_MAP_X(); i++) { // ��
				int no = ground->data[i + j * ground->GetNUM_MAP_X()];
				if (no != 7 && no != 11 && no != 15 && no != 19)
				{
					for (int k = 0; k < blood_num + 1; k++)
					{
						// �}�b�v�ƌ��̃R���W�����p���W�v�Z
						int mapchip_x = i * ground->GetPOS_MAP_X() - ground->GetMAP_OFFSET_X();
						int mapchip_y = j * ground->GetPOS_MAP_Y() - ground->GetMAP_OFFSET_Y();
						int mapchip_width = i * ground->GetPOS_MAP_X() + ground->GetPOS_MAP_WIDTH() - ground->GetMAP_OFFSET_X();
						int mapchip_height = j * ground->GetPOS_MAP_Y() + ground->GetPOS_MAP_HEIGHT() - ground->GetMAP_OFFSET_Y();
						int blood_x = static_cast<int>(bloodVec[k].x);
						int blood_y = static_cast<int>(bloodVec[k].y);
						int blood_width = static_cast<int>(bloodVec[k].x + 1.f);
						int blood_height = static_cast<int>(bloodVec[k].y + 1.f);
						// �����蔻�肪����ꍇtrue(�}�b�v�`�b�v�̍��W�ƃT�C�Y VS ���̍��W�ƃT�C�Y)
						if (bcp->CheckHit(mapchip_x, mapchip_y, mapchip_width, mapchip_height,
							blood_x, blood_y, blood_width, blood_height))
						{
							// �����}�b�v�`�b�v�̏ォ��L�������������Ă��邩���P�t���[���O�̍��W�Ɣ�r���ă`�F�b�N
							if (bloodVec[k].y >= prev_blood_y[k])
							{
								// �R���W�����̂���}�b�v�`�b�v������ł���ꍇ������A�������Ē����`�̓����蔻��ɂ���
								if (ground->data[i + j * ground->GetNUM_MAP_X()] != ground->data[i + (j - 1) * ground->GetNUM_MAP_X()])
								{
									// �}�b�v�`�b�v���ォ�猌���������Ă���
									if (j * 40 - 1 >= prev_blood_y[k])
									{
										HitFromAbove_blood(j, k);
									}
									else // �}�b�v�`�b�v�̉����猌���������Ă���
									{
										HitFromSide_blood(k);
									}
								}
								else // �}�b�v�`�b�v�̉����猌���������Ă���
								{
									HitFromSide_blood(k);
								}
							}
							// �}�b�v�`�b�v�̉����猌���������Ă���
							else if (bloodVec[k].y < prev_blood_y[k])
							{
								// �R���W�����̂���}�b�v�`�b�v������ł���ꍇ������A�������Ē����`�̓����蔻��ɂ���
								if (ground->data[i + j * ground->GetNUM_MAP_X()] != ground->data[i + (j + 1) * ground->GetNUM_MAP_X()])
								{
									// �}�b�v�`�b�v�̉����猌���������Ă���
									if ((j + 1) * 40.f <= prev_blood_y[k] + 2)
									{
										HitFromBelow_blood(j, k);
									}
									else // �}�b�v�`�b�v�̉����猌���������Ă���
									{
										HitFromSide_blood(k);
									}
								}
								else // �}�b�v�`�b�v�̉����猌���������Ă���
								{
									HitFromSide_blood(k);
								}
							}
						}
					}
				}
			}
		}
	}
	else
	{
		// ���̍��W��������
		for (int i = 0; i < blood_num; i++)
		{
			bloodVelocity = 0.f;
			bloodVec[i].x = GetPosition().x;
			bloodVec[i].y = GetPosition().y;
		}

	}
}

void SampleObject::HitFromAbove_blood(int j, int k) // �}�b�v�`�b�v�̏ォ�猌����������
{
	Vector2D vec;
	vec.x = prev_blood_x[k];
	prev_blood_y[k] = static_cast<float>(j * 40 - 1);
	vec.y = prev_blood_y[k];
	if (bloodVec[k].y != prev_blood_y[k])
	{
		bloodVec[k].x = vec.x;
		bloodVec[k].y = vec.y;
	}
}

void SampleObject::HitFromBelow_blood(int j, int k) // �}�b�v�`�b�v�̉����猌����������
{
	Vector2D vec;
	vec.x = prev_blood_x[k];
	prev_blood_y[k] = (j + 1) * 40.f;
	vec.y = prev_blood_y[k];
	if (bloodVec[k].y != prev_blood_y[k])
	{
		bloodVec[k].x = vec.x;
		bloodVec[k].y = vec.y;
	}
}

void SampleObject::HitFromSide_blood(int k) // �}�b�v�`�b�v�̉����猌����������
{
	Vector2D vec;
	vec.y = bloodVec[k].y;
	int _x = static_cast<int>(std::max(bloodVec[k].x, prev_blood_x[k]));
	_x /= 10;
	_x *= 10;
	vec.x = static_cast<float>(_x);
	if (bloodVec[k].x != _x) // (GetPosition().x == _x)�̏ꍇ�������ɂ��蔲���鏰�ɂȂ�
	{
		bloodVec[k].x = vec.x;
		bloodVec[k].y = vec.y;
	}
}

void SampleObject::Dead()
{
	if (isDead == false)
	{
		StopSoundMem(bgm1);
		ChangeVolumeSoundMem(255 * 30 / 100, gameOverSound);
		PlaySoundMem(gameOverSound, DX_PLAYTYPE_BACK);
	}
	
	isDead = true;
}

void SampleObject::Clear()
{
	if (isCleared == false)
	{
		// �T�E���h��炵�Ė�I������V�[���J��
		StopSoundMem(bgm1);
		ChangeVolumeSoundMem(255 * 40 / 100, clearSound);
		PlaySoundMem(clearSound, DX_PLAYTYPE_NORMAL);
		GetOwnerScene()->SetIsCleared(true);
	}
	isCleared = true;
}

void SampleObject::Save()
{
	savePosition->SetCachePosition(GetPosition());
}

