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
class Gimmick_Apple : public GameObject
{
public:
	Gimmick_Apple();
	virtual ~Gimmick_Apple();

public:
	//~ Begin GameObject interface
	virtual void Initialize(Vector2D initialVec, float add_x, float add_y, int direction) override;
	virtual void Update(float delta_seconds) override;
	virtual void Draw(const Vector2D& screen_offset) override;
	virtual void Finalize() override;
	//~ End GameObject interface

private:
	SampleObject* sampleObject_;
	BoxCollisionParams* bcp;

	// 画像
	int loaded_apple;

	bool isPlayerChecked;

	// サウンド
	int fallSound;

	// 初期設定
	float add_x_;
	float add_y_;
	int direction_;

	void DetectPlayer_1();
	void DetectPlayer_2();
	void DetectPlayer_3();
	void DetectPlayer_4();
	void DetectPlayer_6();
	void DetectPlayer_7();
	void DetectPlayer_8();
	void DetectPlayer_9();
};