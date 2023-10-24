#include "Enemy_Boss.h"
#include "../Ground/Ground.h"
#include "../Source/Utility/BoxCollisionParams.h"
#include "../SampleObject/SampleObject.h"
#include "../Source/Scene/SampleScene/SampleScene.h"
#include <algorithm>
#include <random>
#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#define NOMINMAX
#include "DxLib.h"

Enemy_Boss::Enemy_Boss()
	: loaded_sprite_handle(0), chara_act(), enemyState(), ground(), bcp(), sampleObject_()
{
}

Enemy_Boss::~Enemy_Boss()
{
	Finalize();
}

void Enemy_Boss::Initialize()
{
	__super::Initialize();

	// ボス画像の読み込み
	loaded_sprite_handle = LoadDivGraph(_T("Resources/Images/murasuke_boss.bmp"), 24, 4, 6, 40, 48, chara_act);

	// サウンド読み込み
	hitSound = LoadSoundMem("Resources/Sounds/hit.wav");
	deadSound = LoadSoundMem("Resources/Sounds/pound.wav");

	ground = new Ground();
	bcp = new BoxCollisionParams();

	// インスタンス生成
	for (int i = 0; i < apple_number; i++)
	{
		gimmick_apple[i] = new Gimmick_Apple();
		Vector2D vec;
		vec.x = -250.f;
		vec.y = -350.f;
		gimmick_apple[i]->Initialize(vec, -1.f, -1.f, 1);
		apple_pos[i] = vec;
	}

	// Attack1(弾幕)用のランダム変数生成
	for (int i = 0; i < apple_number + 1; i++)
	{
		std::random_device rd; // ハードウェアの乱数ジェネレータを使用してシードを生成
		std::mt19937 gen(rd()); // メルセンヌ・ツイスタ法を使用する乱数生成器
		std::uniform_real_distribution<float> distribution(-8.f, 8.f); // 1.0から10.0までのfloatを生成する分布

		random_number = distribution(gen); // ランダムなfloatを生成
		randomVal[i] = (random_number);
	}

	// ボスが出すリンゴ弾の初期座標更新
	Vector2D vec;
	vec.x = -150.f;
	vec.y = -350.f;
	//vec.x = GetPosition().x - 100.f;
	//vec.y = GetPosition().y - 250.f;
	gimmick_apple_2 = new Gimmick_Apple();
	gimmick_apple_2->Initialize(vec, -1.f, -1.f, 1);
	
	apple_pos_2 = vec;
	gimmick_apple_2->SetPosition(apple_pos_2);

	gravity.y = .2f;

	// ボス初期座標設定
	Vector2D initialVec;
	initialVec.x = -100.f;
	initialVec.y = -280.f;
	SetPosition(initialVec);

	// サンプルシーンからサンプルオブジェクトを取ってくる
	sampleObject_ = SampleScene::GetSampleObject();

	// 往復行動のため最初の位置を取っておく
	prevPosition_x = position.x;
	prevPosition_y = position.y;
	isLeftDir = true;

	// サウンド読み込み
	fallSound = LoadSoundMem("Resources/Sounds/fall.wav");
}

void Enemy_Boss::Update(float delta_seconds)
{
	__super::Update(delta_seconds);
	if (isDead)
	{
		return;
	}

	EnemyState currentState = enemyState;

	// 方向をキャッシュ(Attack1で使用)
	isPrevLeftDir = isLeftDir;

	// 約1フレーム前の位置を覚えておき、コリジョン用に使う
	prev_x = GetPosition().x;
	prev_y = GetPosition().y;

	// 往復移動
	Move(delta_seconds);

	// プレイヤーと接触
	HitPlayer();

	// 弾丸と接触
	HitBullet();

	// ブロックの当たり判定
	DetectMapchip();

	// 攻撃
	if (sampleObject_->GetPosition().x < -15.f && sampleObject_->GetPosition().x > -600.f &&
		sampleObject_->GetPosition().y < -75.f && sampleObject_->GetPosition().y > -450.f)
	{
		if (hp > 15)
		{
			Attack1(delta_seconds);
		}
		else
		{
			Attack2(delta_seconds);
			apple_finalize();
		}
	}

	if (enemyState != currentState) // アニメーションフレームがそれぞれ違うのでここで初期化
	{
		act_index = 0;
	}
}

void Enemy_Boss::Draw(const Vector2D& screen_offset)
{
	__super::Draw(screen_offset);

	// 画像の描画
	int x, y;
	GetPosition().ToInt(x, y);
	Chara_AnimFrame();
	switch (enemyState)
	{
	case (EnemyState::IDLE):
		motion_index = act_idle[act_index];
		max_motion_index = sizeof(act_idle) / sizeof(int);
		break;
	case (EnemyState::RUN):
		motion_index = act_run[act_index];
		max_motion_index = sizeof(act_run) / sizeof(int);
		break;
	case (EnemyState::ATTACK):
		motion_index = act_attack[act_index];
		max_motion_index = sizeof(act_attack) / sizeof(int);
		break;
	case (EnemyState::JUMP):
		motion_index = act_jump[act_index];
		max_motion_index = sizeof(act_jump) / sizeof(int);
		break;
	case (EnemyState::DAMAGE):
		motion_index = act_damage[act_index];
		max_motion_index = sizeof(act_damage) / sizeof(int);
		break;
	default:
		break;
	}

	// ボスキャラ描画
	DrawExtendGraph(x - screen_offset.x - 300, y - screen_offset.y - 400, x - screen_offset.x + 150, y - screen_offset.y + 30, chara_act[motion_index], true);
	// ボスキャラの描画範囲確認用
	//DrawBox(x - screen_offset.x - 180, y - screen_offset.y - 200, x - screen_offset.x + 50, y - screen_offset.y + 30, 100, false);
	//DrawBox(x - screen_offset.x - 120, y - screen_offset.y - 280, x - screen_offset.x - 20, y - screen_offset.y - 200, 100, false);

	// リンゴ弾描画
	for (int i = 0; i < apple_number; i++)
	{
		gimmick_apple[i]->Draw(screen_offset);
	}
	if (hp < 15)
	{
		gimmick_apple_2->Draw(screen_offset);
	}
}

/*
*	終了時呼び出される
*/
void Enemy_Boss::Finalize()
{
	__super::Finalize();

	// 画像の破棄
	for (int i = 0; i < apple_number; i++)
	{
		DeleteGraph(chara_act[i]);
	}
	gimmick_apple_2->Finalize();
}

/*
*	アニメーションフレームスピードの設定
*/
void Enemy_Boss::Chara_AnimFrame()
{
	// アニメーションフレームのスピードを設定しています
	if (--act_wait <= 0)
	{
		act_index++;
		act_wait = ACT_SPEED;
		act_index %= max_motion_index;
	}
}

/*
*	こうげきの第一段階
*	ランダム弾幕
*/
void Enemy_Boss::Attack1(float delta_seconds)
{
	// 往復のたびに弾幕攻撃をします
	if (isPrevLeftDir == isLeftDir)
	{
		for (int i = 0; i < apple_number; ++i) {
			apple_pos[i].x += randomVal[i];
			apple_pos[i].y += randomVal[i + 1];
			gimmick_apple[i]->SetPosition(apple_pos[i]);
			gimmick_apple[i]->Update(delta_seconds);
		}
	}
	else
	{
		ChangeVolumeSoundMem(255 * 30 / 100, fallSound);
		PlaySoundMem(fallSound, DX_PLAYTYPE_BACK);

		// 弾を初期位置に戻す
		for (int i = 0; i < apple_number; ++i)
		{
			Vector2D vec;
			vec.x = GetPosition().x - 100.f;
			vec.y = GetPosition().y - 250.f;
			apple_pos[i] = vec;
		}
		// 弾幕の進む方向を攻撃ごとにランダムで決める
		for (int i = 0; i < apple_number + 1; i++)
		{
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_real_distribution<float> distribution(-8.f, 8.f);

			random_number = distribution(gen);
			randomVal[i] = (random_number);
		}
	}
}

/*
*	こうげきの第二段階
*	追尾弾
*/
void Enemy_Boss::Attack2(float delta_seconds)
{
	// リンゴとプレイヤーを結ぶ直線を平行移動させて、原点からみた直線座標へ移動(atan2関数を使用するため)
	double dx = sampleObject_->GetPosition().x - apple_pos_2.x;
	double dy = sampleObject_->GetPosition().y - apple_pos_2.y;
	// atan2関数を使用して角度を計算する
	double angle = atan2(dy, dx);
	// sin/cos関数を使用して、角度を-1~1の数値に変換して、ポジションに加算していく
	apple_pos_2.x += static_cast<int>(3 * cos(angle));
	apple_pos_2.y += static_cast<int>(3 * sin(angle));
	gimmick_apple_2->SetPosition(apple_pos_2);
	gimmick_apple_2->Update(delta_seconds);
}

/*
*	リンゴを消して解放
*/
void Enemy_Boss::apple_finalize()
{
	for (int i = 0; i < apple_number; i++)
	{
		gimmick_apple[i]->Finalize();
	}
}

/*
*	プレイヤーと接触した時
*/
void Enemy_Boss::HitPlayer()
{
	int chara_x = static_cast<int>(sampleObject_->GetPosition().x - 14.f);
	int chara_y = static_cast<int>(sampleObject_->GetPosition().y + 2.f);
	int chara_width = static_cast<int>(sampleObject_->GetPosition().x - 14.f + 27);
	int chara_height = static_cast<int>(sampleObject_->GetPosition().y + 2.f + 38);
	int enemy_x = static_cast<int>(GetPosition().x - 180.f);
	int enemy_y = static_cast<int>(GetPosition().y - 200.f);
	int enemy_width = static_cast<int>(GetPosition().x + 50);
	int enemy_height = static_cast<int>(GetPosition().y + 30);

	// 胴体のコリジョン範囲と重なればプレーヤーが死ぬ
	if (bcp->CheckHit(chara_x, chara_y, chara_width, chara_height,
		enemy_x, enemy_y, enemy_width, enemy_height))
	{
		sampleObject_->Dead();
	}
	// 耳のコリジョン範囲と重なればプレーヤーが死ぬ
	enemy_x = static_cast<int>(GetPosition().x - 120.f);
	enemy_y = static_cast<int>(GetPosition().y - 280.f);
	enemy_width = static_cast<int>(GetPosition().x - 20);
	enemy_height = static_cast<int>(GetPosition().y - 200);
	if (bcp->CheckHit(chara_x, chara_y, chara_width, chara_height,
		enemy_x, enemy_y, enemy_width, enemy_height))
	{
		sampleObject_->Dead();
	}
}

/*
*	弾に被弾した時
*/
void Enemy_Boss::HitBullet()
{
	for (int i = 0; i < 20; i++)
	{
		int bullet_x = static_cast<int>(sampleObject_->GetBullet(i).x);
		int bullet_y = static_cast<int>(sampleObject_->GetBullet(i).y);
		int bullet_width = static_cast<int>(sampleObject_->GetBullet(i).x + 1);
		int bullet_height = static_cast<int>(sampleObject_->GetBullet(i).y + 1);
		int enemy_x = static_cast<int>(GetPosition().x - 120.f);
		int enemy_y = static_cast<int>(GetPosition().y - 280.f);
		int enemy_width = static_cast<int>(GetPosition().x - 20);
		int enemy_height = static_cast<int>(GetPosition().y - 200);

		// 弾とキャラが重なった時HPを減らす
		if (bcp->CheckHit(bullet_x, bullet_y, bullet_width, bullet_height,
			enemy_x, enemy_y, enemy_width, enemy_height))
		{
			sampleObject_->SetBullet_x(i, 10000.f);
			sampleObject_->SetBullet_y(i, 10000.f);
			hp--;
			enemyState = EnemyState::DAMAGE;
			isDamaged = true;
			if (hp <= 0)
			{
				ChangeVolumeSoundMem(255 * 30 / 100, deadSound);
				PlaySoundMem(deadSound, DX_PLAYTYPE_BACK);
				Finalize();
				isDead = true;
			}
			else
			{
				ChangeVolumeSoundMem(255 * 30 / 100, hitSound);
				PlaySoundMem(hitSound, DX_PLAYTYPE_BACK);
			}
		}
	}
	// 一定時間DAMAGEステートを維持する
	if (isDamaged)
	{
		damageTimer++;
		if (damageTimer > 5)
		{
			isDamaged = false;
			damageTimer = 0;
		}
	}
}

/*
*	マップチップに対するコリジョン判定
*/
void Enemy_Boss::DetectMapchip()
{
	for (int j = 0; j < ground->GetNUM_MAP_Y(); j++) // 行数分回す
	{
		for (int i = 0; i < ground->GetNUM_MAP_X(); i++) { // 列数分回す
			int no = ground->data[i + j * ground->GetNUM_MAP_X()];
			if (no != 7 && no != 11 && no != 15 && no != 19)
			{
				int mapchip_x = i * ground->GetPOS_MAP_X() - ground->GetMAP_OFFSET_X();
				int mapchip_y = j * ground->GetPOS_MAP_Y() - ground->GetMAP_OFFSET_Y();
				int mapchip_width = i * ground->GetPOS_MAP_X() + ground->GetPOS_MAP_WIDTH() - ground->GetMAP_OFFSET_X();
				int mapchip_height = j * ground->GetPOS_MAP_Y() + ground->GetPOS_MAP_HEIGHT() - ground->GetMAP_OFFSET_Y();
				int enemy_x = static_cast<int>(GetPosition().x - 14.f);
				int enemy_y = static_cast<int>(GetPosition().y - 10.f);
				int enemy_width = static_cast<int>(GetPosition().x - 14.f + 27);
				int enemy_height = static_cast<int>(GetPosition().y - 10.f + 32);

				// キャラとマップチップに対するコリジョン判定
				if (bcp->CheckHit(mapchip_x, mapchip_y, mapchip_width, mapchip_height,
					enemy_x, enemy_y, enemy_width, enemy_height))
				{
					// 上から当たっているかチェック
					if (GetPosition().y >= prev_y)
					{
						/*
						*	マップチップよりも上から当たっているかチェックする
						*	その場合のみy座標を固定する
						*/
						// マップチップに上から当たっている場合のポジション設定
						if ((j * 40 - 21) - ground->GetMAP_OFFSET_Y() >= prev_y)
						{
							Vector2D vec;
							vec.x = prev_x;
							prev_y = static_cast<float>((j * 40 - 21) - ground->GetMAP_OFFSET_Y());
							vec.y = prev_y;
							if (GetPosition().y != prev_y)
							{
								SetPosition(vec);
							}
							// 各種変数の変更
							yadd.y = 0.0f; // 重力
							canJump = true; // ジャンプ可能かどうか
							isJumping = false; // 現在ジャンプしているか
							isJumpOnce = false; // 一度ジャンプしたかどうか
							isGround = true; // 地面に接地しているか
							jumpTime = 0; // ジャンプしている時間(この秒数で上に上がるか下に下がるかを決めている)
							jump.y = 0; // y座標のベロシティ
						}
						// マップチップに上からかつ横から当たっている場合のポジション設定
						else
						{
							Vector2D vec;
							vec.y = GetPosition().y;
							int _x = static_cast<int>(std::max(GetPosition().x, prev_x));
							_x /= 20;
							_x *= 20;
							vec.x = static_cast<float>(_x);
							if (GetPosition().x != _x)
							{
								SetPosition(vec);
							}
						}
					}
					// 下から当たっているかチェック
					else if (GetPosition().y < prev_y)
					{
						/*
						*	マップチップよりも下から当たっているかチェックする
						*	その場合のみy座標を固定する
						*/
						// マップチップに下から当たっている場合のポジション設定
						if (((j + 1) * 40.f) - ground->GetMAP_OFFSET_Y() <= prev_y)
						{
							Vector2D vec;
							vec.x = prev_x;
							prev_y = ((j + 1) * 40.f) - ground->GetMAP_OFFSET_Y();
							vec.y = prev_y;
							if (GetPosition().y != prev_y)
							{
								SetPosition(vec);
							}
							yadd.y = 0.0f;
						}
						// マップチップに下からかつ横から当たっている場合のポジション設定
						else
						{
							Vector2D vec;
							vec.y = GetPosition().y;
							int _x = static_cast<int>(std::max(GetPosition().x, prev_x));
							_x /= 20;
							_x *= 20;
							vec.x = static_cast<float>(_x);
							if (GetPosition().x != _x)
							{
								SetPosition(vec);
							}
						}
					}
				}
			}
		}
	}
}

/*
*	左右に往復する処理
*/
void Enemy_Boss::Move(float delta_seconds)
{
	Vector2D input_dir;
	if (position.x >= prevPosition_x - 50.f && isLeftDir)
	{
		input_dir.x += -1;
		isTurnLeft = true;
		if (isGround == true && !isDamaged)
		{
			enemyState = EnemyState::RUN;
		}
	}
	else if (position.x <= prevPosition_x + 50.f)
	{
		input_dir.x += 1;
		isTurnLeft = false;
		if (isGround == true && !isDamaged)
		{
			enemyState = EnemyState::RUN;
		}
		isLeftDir = false;
	}
	else
	{
		isLeftDir = true;
	}

	// ポジションの設定
	delta_position = input_dir.Normalize() * MOVEMENT_SPEED * delta_seconds;
	SetPosition(GetPosition() + delta_position + yadd);
	yadd.y += gravity.y + 0.1f; // 重力の設定
	yadd.y = std::min(10.0f, std::max(0.0f, yadd.y));
}
