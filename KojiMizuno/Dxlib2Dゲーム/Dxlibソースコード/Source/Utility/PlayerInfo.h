#pragma once
#include "../GameObject/GameObject.h"
#include "../GameObject/SampleObject/SampleObject.h"

struct PlayerInfo
{
public:
	PlayerInfo()
		: x(0.0f), y(0.0f)
	{}

	PlayerInfo(float in_x, float in_y)
		: x(in_x), y(in_y)
	{}

public:
	const PlayerInfo operator +(const PlayerInfo& in_vector) const;
	PlayerInfo& operator +=(const PlayerInfo& in_vector);
	const PlayerInfo operator -(const PlayerInfo& in_vector) const;
	PlayerInfo& operator -=(const PlayerInfo& in_vector);
	const PlayerInfo operator *(float in_scalar) const;
	PlayerInfo& operator *=(float in_scalar);
	const PlayerInfo operator /(float in_scalar) const;
	PlayerInfo& operator /=(float in_scalars);

public:
	float x, y, x_width, y_height, prev_x, prev_y;
	bool isDead, canJump, isJumping, isJumpKeyLeave, isJumpOnce, isGround, jumpTime;
	Vector2D yadd, jump;
};