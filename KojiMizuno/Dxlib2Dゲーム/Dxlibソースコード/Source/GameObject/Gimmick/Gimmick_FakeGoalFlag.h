#pragma once

#include "../GameObject.h"
#include "../Ground/Ground.h"
#include "../Source/Utility/BoxCollisionParams.h"
#include "../SampleObject/SampleObject.h"
#include <string>

/**
 * サンプルオブジェクト
 * サンプル用に簡易的に実装
 */
class Gimmick_FakeGoalFlag : public GameObject
{
public:
	Gimmick_FakeGoalFlag();
	virtual ~Gimmick_FakeGoalFlag();

public:
	//~ Begin GameObject interface
	virtual void Initialize(Vector2D initialVec) override;
	virtual void Update(float delta_seconds) override;
	virtual void Draw(const Vector2D& screen_offset) override;
	virtual void Finalize() override;
	//~ End GameObject interface

private:
	SampleObject* sampleObject_;
	BoxCollisionParams* bcp;

	// 各画像用
	int loaded_goal_flag;
	int loaded_windows;

	bool isFakeCleared;
	Vector2D delta_position;
};