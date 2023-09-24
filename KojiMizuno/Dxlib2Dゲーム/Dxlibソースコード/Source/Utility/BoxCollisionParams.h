#pragma once

#include "../GameObject/GameObject.h"

/**
 * 2Dベクトルクラス
 */
struct BoxCollisionParams
{
public:
	BoxCollisionParams(){}
	virtual ~BoxCollisionParams() {}

private:
	float pos_x_;
	float pos_y_;
	float width_;
	float height_;

public:
	int CheckHit(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);
	bool DetectMapchip(int player_x, int player_y, int player_width, int player_height, int player_prev_x, int player_prev_y);

	void SetPosition(float pos_x, float pos_y, float width, float height);
};