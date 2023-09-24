#pragma once

#include "../GameObject.h"
#include "../Ground/Ground.h"
#include "../Source/Utility/BoxCollisionParams.h"
#include "../SampleObject/SampleObject.h"
#include "Enemy_Red.h"
#include <string>

enum class EnemyState3 {
	IDLE,
	RUN,
	JUMP,
	ATTACK,
	DAMAGE,
	DEAD
};

class Enemy_Red_Shy : public GameObject
{
public:
	Enemy_Red_Shy();
	virtual ~Enemy_Red_Shy();

public:
	//~ Begin GameObject interface
	virtual void Initialize() override;
	virtual void Update(float delta_seconds) override;
	virtual void Draw(const Vector2D& screen_offset) override;
	virtual void Finalize() override;
	//~ End GameObject interface

private:
	EnemyState3 enemyState;
	SampleObject* sampleObject_;
	Ground* ground;
	BoxCollisionParams* bcp;

	// 画像
	int loaded_sprite_handle;
	int loaded_parent_handle;
	// アニメーション
	int motion_index;
	int act_idle[4] = { 0, 0, 0, 0 };
	int act_run[4] = { 0, 1, 2, 3 };
	int act_attack[4] = { 4, 5, 6 };
	int act_jump[4] = { 0, 0, 0, 0 };
	int act_damage[4] = { 7, 7, 7, 7 };
	int chara_act[25];
	const int ACT_SPEED = 5;
	int act_wait = ACT_SPEED;
	int act_index = 0;
	int max_motion_index = 8;
	bool isTurnLeft;
	bool isAttack;
	void Chara_AnimFrame();

	Vector2D delta_position;
	float prev_x;
	float prev_y;
	Vector2D yadd;
	Vector2D jump;
	Vector2D gravity;

	// ジャンプ
	bool canJump;
	float jumpTime;
	int jumpCount;
	bool isJumpOnce;
	bool isJumping;
	bool isJumpKeyLeave;
	bool isGround;
	int remainingJumps;
	int maxJumpCount;

	std::string hitTop;
	std::string hitSide;
	int hp;
	float prevPosition_x;
	float prevPosition_y;
	bool isLeftDir;
	bool isDead;
	bool isDamaged;
	float damageTimer;

	// サウンド
	int hitSound;
	int deadSound;
	int parentSound;

	bool isFindPlayer;
	bool canAttack;
	float addSpeed;
	float MOVEMENT_SPEED;

	// 親画像演出用
	float change_parent_pos_x;
	float change_parent_pos_y;
};