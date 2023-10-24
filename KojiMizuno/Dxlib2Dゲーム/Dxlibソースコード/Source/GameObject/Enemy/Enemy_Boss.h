#pragma once
#pragma once

#include "../GameObject.h"
#include "../Ground/Ground.h"
#include "../Source/Utility/BoxCollisionParams.h"
#include "../SampleObject/SampleObject.h"
#include "../Gimmick/Gimmick_Apple.h"
#include <string>
#include "EnemyState.h"



class Enemy_Boss : public GameObject
{
public:
	Enemy_Boss();
	virtual ~Enemy_Boss();

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
	

	// ボス画像
	int loaded_sprite_handle;

	// アニメーション用
	int motion_index = 0;
	int act_idle[4] = { 0, 0, 0, 0 };
	int act_run[4] = { 4, 5, 6, 7 };
	int act_run2[4] = { 17, 18, 19, 20 };
	int act_attack[4] = { 0, 0, 0, 0 };
	int act_jump[4] = { 0, 0, 0, 0 };
	int act_damage[4] = { 1, 1, 1, 1 };
	int act_damage2[4] = { 14, 14, 14, 14 };
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
	// ジャンプと接地
	bool canJump = false;
	float jumpTime = 0;
	int jumpCount = 0;
	bool isJumpOnce = false;
	bool isJumping = false;
	bool isJumpKeyLeave = false;
	bool isGround = false;
	int remainingJumps = 0;
	int maxJumpCount = 2;

	int hp = 30;
	// initialでキャッシュする座標
	float prevPosition_x = 0;
	float prevPosition_y = 0;

	bool isLeftDir = false;
	bool isDead = false;
	bool isDamaged = false;
	float damageTimer = 0;

	Gimmick_Apple* gimmick_apple[20];
	bool isInitializeApple = true;
	float random_number = 0;
	float randomVal[21]{};
	int apple_number = 20;
	Vector2D apple_pos[20];
	bool isPrevLeftDir = false;
	void Attack1(float delta_seconds);
	void Attack2(float delta_seconds);
	int enemy_level;
	void apple_finalize();
	int fallSound;

	// 誘導弾
	Gimmick_Apple* gimmick_apple_2;
	Vector2D apple_pos_2;
	float vx;
	float vy;
	float vx_nor;
	float vy_nor;
	float rotate = 0;
	float cache_PlayerPosition_x;
	float cache_PlayerPosition_y;

	// サウンド
	int hitSound = 0;
	int deadSound = 0;

	void HitPlayer();
	void HitBullet();
	void DetectMapchip();
	void Move(float delta_seconds);
};