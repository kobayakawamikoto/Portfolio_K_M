#include "Collision.h"
#include "../Ground/Ground.h"
#include "../Source/Utility/BoxCollisionParams.h"
#include "../SampleObject/SampleObject.h"
#include "../Source/Scene/SampleScene/SampleScene.h"
#include "../Source/Utility/PlayerInfo.h"
#include <algorithm> // std::maxを使用するため
#include <random>
#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#define NOMINMAX
#include "DxLib.h"

Collision::Collision() :
	chip(), loaded_sprite_handle(0)
{
}

Collision::~Collision()
{
	Finalize();
}

void Collision::Initialize()
{
	__super::Initialize();

	ground = new Ground();
}

void Collision::Update(float delta_seconds)
{
	__super::Update(delta_seconds);

}

void Collision::Draw(const Vector2D& screen_offset)
{
	__super::Draw(screen_offset);

}

void Collision::Finalize()
{
	__super::Finalize();
}

//
// 当たり判定を行う関数
// 
// 緑のキャラ 左   x1
// 　　　　　 上   y1
// 　　　　　 横幅 w1
// 　　　　　 縦幅 h1
// 黄のキャラ 左   x2
// 　　　　　 上   y2
// 　　　　　 横幅 w2
// 　　　　　 縦幅 h2
//
int Collision::CheckHit(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
	int L1 = x1;      // 左
	int R1 = w1; // 右(左+横幅)
	int L2 = x2;      // 左
	int R2 = w2; // 右(左+横幅)

	if (R1 <= L2) return 0;
	if (R2 <= L1) return 0;

	int U1 = y1;      // 上
	int D1 = h1; // 下(上+縦幅)
	int U2 = y2;      // 上
	int D2 = h2; // 下(上+縦幅)

	if (D1 <= U2) return 0;
	if (D2 <= U1) return 0;

	// それ以外の場合は当たっている
	return 1;
}

bool Collision::DetectMapchip(int chara_x, int chara_y, int chara_width, int chara_height, int prev_x, int prev_y)
{
	/*
	for (int j = 0; j < 12; j++) // 行数
	{
		for (int i = 0; i < ground->GetNUM_MAP_X(); i++) { // 列数
			int no = ground->data[i + j * ground->GetNUM_MAP_X()];
			if (no != 7 && no != 11 && no != 15 && no != 19)
			{
				// 当たり判定の是非
				if (CheckHit(i * 32 - ground->GetMAP_OFFSET_X(), j * 40, i * 32 + 32 - ground->GetMAP_OFFSET_X(), j * 40 + 40,
					static_cast<int>(GetPosition().x - 14.f), static_cast<int>(GetPosition().y + 2.f), static_cast<int>(GetPosition().x - 14.f + 27), static_cast<int>(GetPosition().y + 2.f + 38)))
				{
					//// 左右から当たっている
					//if (GetPosition().x != prev_x)
					//{
					//	Vector2D vec_ = GetPosition();
					//	vec_.x = std::max(GetPosition().x, prev_x);
					//	vec_.x /= 32; // SIZE_MAP_X
					//	vec_.x *= 32;
					//	SetPosition(vec_);
					//}
					//// 上下から当たっている
					//if (GetPosition().y != prev_y)
					//{
					//	Vector2D vec_ = GetPosition();
					//	vec_.y = std::max(GetPosition().y, prev_y);
					//	vec_.y /= 40; // SIZE_MAP_Y
					//	vec_.y *= 40;
					//	SetPosition(vec_);
					//}
					// 前回の位置に戻す

					// 上から当たっているかチェック
					if (chara_y >= prev_y)
					{
						// 上にコリジョンのあるマップチップがないときだけ判定
						if (ground->data[i + j * ground->GetNUM_MAP_X()] != ground->data[i + (j - 1) * ground->GetNUM_MAP_X()])
						{
							// 地形よりも上から当たっているかチェックする
							// その場合のみYを補正
							// そうでない場合は横から当たっている
							if (j * 40 - 38 >= prev_y)
							{
								// ④地形に上から当たっている
								Vector2D vec;
								vec.x = prev_x;
								prev_y = static_cast<float>(j * 40 - 38);
								vec.y = prev_y;
								if (chara_y != prev_y)
								{
									SetPosition(vec);
								}
								if (!isDead)
								{
									yadd.y = 0.0f;
								}
								canJump = true;
								isJumping = false;
								isJumpKeyLeave = false;
								isJumpOnce = false;
								isGround = true;
								jumpTime = 0;
								jump.y = 0;
							}
							else
							{
								// ⑤地形の横から当たっている
								// マスクするためintを使用します (floatだと、割って掛けると同じ値になってしまう)
								Vector2D vec;
								vec.y = chara_y;
								int _x = 0;

								if (playerDirection == PlayerDirection::RIGHT)
								{
									_x = static_cast<int>(std::min(chara_x, prev_x));
								}
								else
								{
									_x = static_cast<int>(std::max(chara_x, prev_x));
								}

								_x /= 5;
								_x *= 5;
								vec.x = static_cast<float>(_x);
								if (chara_x != _x) // (GetPosition().x == _x)の場合下から上にすり抜ける床になる
								{
									SetPosition(vec);
								}
							}
						}
						else
						{
							// ⑤地形の横から当たっている
							// マスクするためintを使用します (floatだと、割って掛けると同じ値になってしまう)
							Vector2D vec;
							vec.y = chara_y;
							int _x = 0;

							if (playerDirection == PlayerDirection::RIGHT)
							{
								_x = static_cast<int>(std::min(chara_x, prev_x));
							}
							else
							{
								_x = static_cast<int>(std::max(chara_x, prev_x));
							}

							_x /= 5;
							_x *= 5;
							vec.x = static_cast<float>(_x);
							if (chara_x != _x) // (GetPosition().x == _x)の場合下から上にすり抜ける床になる
							{
								SetPosition(vec);
							}
						}
					}
					// 下から当たっているかチェック
					else if (chara_y < prev_y)
					{
						// 下にコリジョンのあるマップチップがないときだけ判定
						if (ground->data[i + j * ground->GetNUM_MAP_X()] != ground->data[i + (j + 1) * ground->GetNUM_MAP_X()])
						{
							// 地形よりも下から当たっているかチェックする
							// その場合のみYを補正
							// そうでない場合は横から当たっている
							if ((j + 1) * 40.f <= prev_y + 2)
							{
								Vector2D vec;
								vec.x = chara_x;
								prev_y = (j + 1) * 40.f;
								vec.y = prev_y;
								if (chara_y != prev_y)
								{
									SetPosition(vec);
								}
								if (!isDead)
								{
									yadd.y = 0.0f;
								}
							}
							else
							{
								// 地形の横から当たっている
								// マスクするためintを使用します (floatだと、割って掛けると同じ値になってしまう)
								Vector2D vec;
								vec.y = chara_y;
								int _x = 0;

								if (playerDirection == PlayerDirection::RIGHT)
								{
									_x = static_cast<int>(std::min(chara_x, prev_x));
								}
								else
								{
									_x = static_cast<int>(std::max(chara_x, prev_x));
								}

								_x /= 5;
								_x *= 5;
								vec.x = static_cast<float>(_x);
								if (chara_x != _x) // (GetPosition().x == _x)の場合下から上にすり抜ける床になる
								{
									SetPosition(vec);
								}
							}
						}
						else
						{
							// 地形の横から当たっている
							// マスクするためintを使用します (floatだと、割って掛けると同じ値になってしまう)
							Vector2D vec;
							vec.y = chara_y;
							int _x = 0;

							if (playerDirection == PlayerDirection::RIGHT)
							{
								_x = static_cast<int>(std::min(chara_x, prev_x));
							}
							else
							{
								_x = static_cast<int>(std::max(chara_x, prev_x));
							}

							_x /= 5;
							_x *= 5;
							vec.x = static_cast<float>(_x);
							if (chara_x != _x) // (GetPosition().x == _x)の場合下から上にすり抜ける床になる
							{
								SetPosition(vec);
							}
						}
					}
				}
			}
		}
	}*/
	return false;
}


