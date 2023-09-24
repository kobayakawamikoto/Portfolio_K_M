#pragma once

#include "../GameObject.h"
#include "../Ground/Ground.h"
#include "../Source/Utility/BoxCollisionParams.h"
#include "../SampleObject/SampleObject.h"
#include <string>

enum class EnemyState {
	IDLE,
	RUN,
	JUMP,
	ATTACK,
	DAMAGE,
	DEAD
};

class Enemy_Purple : public GameObject
{
public:
	Enemy_Purple();
	virtual ~Enemy_Purple();

public:
	//~ Begin GameObject interface
	virtual void Initialize() override;
	virtual void Update(float delta_seconds) override;
	virtual void Draw(const Vector2D& screen_offset) override;
	virtual void Finalize() override;
	//~ End GameObject interface

private:
	EnemyState enemyState;
	SampleObject* sampleObject_;
	Ground* ground;
	BoxCollisionParams* bcp;

	// �摜
	int loaded_sprite_handle;

	// �A�j���[�V����
	int motion_index = 0;
	int act_idle[4] = { 0, 0, 0, 0 };
	int act_run[4] = { 4, 5, 6, 7 };
	int act_attack[4] = { 0, 0, 0, 0 };
	int act_jump[4] = { 0, 0, 0, 0 };
	int act_damage[4] = { 1, 1, 1, 1 };
	int chara_act[25];
	const int ACT_SPEED = 5;
	int act_wait = ACT_SPEED;
	int act_index = 0;
	int max_motion_index = 8;
	bool isTurnLeft = false;
	bool isAttack = false;
	float MOVEMENT_SPEED = 30.0f;
	void Chara_AnimFrame();
	
	Vector2D delta_position = Vector2D();
	// �O�t���[���̍��W
	float prev_x = 0;
	float prev_y = 0;
	Vector2D yadd = Vector2D();
	Vector2D jump = Vector2D();
	Vector2D gravity = Vector2D();
	// �W�����v
	bool canJump = false;
	float jumpTime = 0;
	int jumpCount = 0;
	bool isJumpOnce = false;
	bool isJumping = false;
	bool isJumpKeyLeave = false;
	bool isGround = false;
	int remainingJumps = 0;
	int maxJumpCount = 2;

	std::string hitTop;
	std::string hitSide;
	int hp = 3;
	// initial�ŃL���b�V��������W
	float prevPosition_x = 0;
	float prevPosition_y = 0;

	bool isLeftDir = false;
	bool isDead = false;
	bool isDamaged = false;
	float damageTimer = 0;

	// �T�E���h
	int hitSound = 0;
	int deadSound = 0;

	// �R���W����
	void DetectPlayer();
	void HitBullet();
	void DetectMapchip();
};