#pragma once

#include "../GameObject.h"
#include "../Ground/Ground.h"
#include "../Source/Utility/BoxCollisionParams.h"
#include "../SampleObject/SampleObject.h"
#include <string>
#include "EnemyState.h"



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

	// 画像
	int loaded_sprite_handle;

	// アニメーション用
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

	// 前フレームの座標
	float prev_x = 0;
	float prev_y = 0;
	Vector2D yadd = Vector2D();
	Vector2D jump = Vector2D();
	Vector2D gravity = Vector2D();

	// ジャンプと接地ステート
	bool canJump = false;
	float jumpTime = 0;
	int jumpCount = 0;
	bool isJumpOnce = false;
	bool isJumping = false;
	bool isJumpKeyLeave = false;
	bool isGround = false;
	int remainingJumps = 0;
	int maxJumpCount = 2;

	int hp = 3;
	// initialでキャッシュする座標
	float prevPosition_x = 0;
	float prevPosition_y = 0;

	bool isLeftDir = false;
	bool isDead = false;
	bool isDamaged = false;
	float damageTimer = 0;

	// サウンド
	int hitSound = 0;
	int deadSound = 0;

	// コリジョン
	void HitPlayer();
	void HitBullet();
	void DetectMapchip();
	void Move(float delta_seconds);
};