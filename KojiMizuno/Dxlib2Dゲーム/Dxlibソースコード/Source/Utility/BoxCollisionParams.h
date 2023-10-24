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
};