#pragma once

#include "../GameObject.h"
#include "../Source/Utility/BoxCollisionParams.h"
#include "../SampleObject/SampleObject.h"
#include <string>

class Platform_Passable : public GameObject
{
public:
	Platform_Passable();
	virtual ~Platform_Passable();

public:
	//~ Begin GameObject interface
	virtual void Initialize(Vector2D initialVec, float radian_constant, float speed, int direction) override;
	virtual void Update(float delta_seconds) override;
	virtual void Draw(const Vector2D& screen_offset) override;
	virtual void Finalize() override;
	//~ End GameObject interface

	SampleObject* sampleObject_;
	BoxCollisionParams* bcp;

	// 画像
	int loaded_sprite_handle = 0;

	// アニメーション
	int chara_act[25]{};
	const int ACT_SPEED = 5;
	int act_wait = ACT_SPEED;
	int act_index = 0;
	bool isTurnLeft = false;
	float addSpeed = 3.f;
	void Chara_AnimFrame();

	// 前フレームの座標
	Vector2D delta_position = Vector2D();
	float prev_x = 0.f;
	float prev_y = 0.f;

	float MOVEMENT_SPEED = 30.0f;
	float radian_x = 0.f;
	float radian_period = 1.f;
	float speed = 5.f;

	float prevPosition_x = 0;
	float prevPosition_y = 0;
	bool isLeftDir = true;

	void Move(float delta_seconds);
	void HitPlayer();
};