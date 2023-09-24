#pragma once

#include "../GameObject.h"
#include "../Ground/Ground.h"

/**
 * サンプルマップ
 * サンプル用に簡易的に実装
 */
class Collision : public GameObject
{
public:
	Collision();
	virtual ~Collision();

public:
	//~ Begin GameObject interface
	virtual void Initialize() override;
	virtual void Update(float delta_seconds) override;
	virtual void Draw(const Vector2D& screen_offset) override;
	virtual void Finalize() override;
	//~ End GameObject interface

private:
	int loaded_sprite_handle;
	const int SIZE_MAP_X = 32;	//< マップチップXサイズ
	const int SIZE_MAP_Y = 40;	//< マップチップYサイズ
	const int NUM_CHIP_X = 4; //< 横4種類
	const int NUM_CHIP_Y = 2; //< 縦2種類
	const int NUM_CHIP_ALL = 8; //< 8個
	const int NUM_MAP_X = 60; // 32x60=1920
	const int NUM_MAP_Y = 12; // 40x12=480
	const int MAP_OFFSET_X = 640;
	const int MAP_OFFSET_Y = 0;
	int chip[25];

	Ground* ground;
	int CheckHit(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);

	bool DetectMapchip(int chara_x, int chara_y, int chara_width, int chara_height, int prev_x, int prev_y);

public:
	

	int GetNUM_MAP_X() { return NUM_MAP_X; }
	int GetNUM_MAP_Y() { return NUM_MAP_Y; }
	int GetMAP_OFFSET_X() { return MAP_OFFSET_X; }
	int GetMAP_OFFSET_Y() { return MAP_OFFSET_Y; }
};