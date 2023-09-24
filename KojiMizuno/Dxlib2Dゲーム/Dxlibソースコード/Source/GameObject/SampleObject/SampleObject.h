#pragma once

#include "../GameObject.h"
#include "../Ground/Ground.h"
#include "../Source/Utility/BoxCollisionParams.h"
#include "../Source/Utility/SavePosition.h"
#include <vector>

// Player�̏��
enum class PlayerState {
	IDLE,
	RUN,
	JUMP,
	ATTACK,
	DAMAGE,
	DEAD
};

// Player�̌���
enum class PlayerDirection {
	LEFT,
	RIGHT
};

// �e�ۂ̏��
struct BULLET {
	bool flag;
	float x;
	float y;
	float pre_x;
	PlayerDirection playerDirection;
};

class SampleObject : public GameObject
{
public:
	SampleObject();
	virtual ~SampleObject();

public:
	//~ Begin GameObject interface
	virtual void Initialize() override;
	virtual void Update(float delta_seconds) override;
	virtual void Draw(const Vector2D& screen_offset) override;
	virtual void Finalize() override;
	//~ End GameObject interface

private:
	PlayerState playerState = PlayerState::IDLE;
	PlayerDirection playerDirection = PlayerDirection::RIGHT;

	// �摜�i�[�A�h���X
	int loaded_sprite_handle = 0;
	int loaded_bcollon_handle[11]{};
	int loaded_gameover_handle = 0;

	// �A�j���[�V�����֘A
	int motion_index = 0;
	int act_idle[24] = { 0, 1, 3, 2, 3, 1, 0, 0, 0, 0, 0, 0, 0, 2, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0};
	int act_run[8] = { 4, 5, 6, 7, 8, 9, 10, 11 };
	int act_attack[3] = {24, 25, 26};
	int act_jump[13] = { 16, 17, 18, 19, 20, 21, 21, 21, 21, 21, 21, 21, 21 };
	int chara_act[30];
	const int ACT_SPEED = 5;
	int act_wait = ACT_SPEED;
	int act_index = 0;
	int max_motion_index = 8;
	bool isTurnLeft = false;
	bool isAttack = false;
	void Chara_AnimFrame();

	Ground* ground;
	BoxCollisionParams* bcp;
	SavePosition* savePosition;
	Vector2D delta_position;

	Vector2D yadd = Vector2D();
	Vector2D jump = Vector2D();
	Vector2D gravity = Vector2D();
	// �W�����v�֌W
	bool canJump = false;
	float jumpTime = 0;
	int jumpCount = 0;
	bool isJumpOnce = false;
	bool isJumping = false;
	bool isJumpKeyLeave = false;
	bool isGround = false;
	int remainingJumps = 0;
	int maxJumpCount = 2;

	bool isDead = false;

	// �T�E���h�֌W
	int gameOverSound = 0;
	int fireSound = 0;
	int jump1Sound = 0;
	int jump2Sound = 0;
	int bgm1 = 0;

	// ��
	float blood_x = 0;
	float blood_y = 0;
	float bloodVelocity = 0;
	float random_number = 0;
	float blood_num = 500;
	Vector2D bloodVec[501]{};
	float randomVal[501]{};
	float prev_blood_x[501]{};
	float prev_blood_y[501]{};
	
	float addyadd = 0;
	float addyaddClamp = 0;

	// �e(�U��)
	BULLET bullet[20];
	int prevLeftButtonState = 0;
	bool canShot = true;
	bool isAttacked = false;
	float attackTimer = 0;

	int clearSound = 0;
	bool isCleared = false;

	void Attack(float delta_seconds);
	void Jump();
	void DetectManualMapchip();
	void DetectMapchip();
	void HitFromAbove(int j);
	void HitFromBelow(int j);
	void HitFromSide();
	void HitFromAbove_blood(int j, int k);
	void HitFromBelow_blood(int j, int k);
	void HitFromSide_blood(int k);
	void Bleed();
	

public:
	float offset_pos_x = 14.f; // Player�̃R���W�����pX���W�𓾂邽�߂̃I�t�Z�b�g
	float offset_pos_y = 2.f; // Player�̃R���W�����pY���W�𓾂邽�߂̃I�t�Z�b�g
	float offset_pos_width = 27.f; // Player�̃R���W�����pwidth���W�𓾂邽�߂̃I�t�Z�b�g
	float offset_pos_height = 38.f; // Player�̃R���W�����pheight���W�𓾂邽�߂̃I�t�Z�b�g
	float prev_x = 0.f; // Player�̖�1�t���[���O��X���W
	float prev_y = 0.f; // Player�̖�1�t���[���O��Y���W
	void Dead();
	void Clear();
	void Save();
	
	BULLET GetBullet(int i) { return bullet[i]; }
	void SetBullet_x(int i, float x) { bullet[i].x = x; }
	void SetBullet_y(int i, float y) { bullet[i].y = y; }
	void SetIsCleared(bool isCleared_) { isCleared = isCleared_; }
};