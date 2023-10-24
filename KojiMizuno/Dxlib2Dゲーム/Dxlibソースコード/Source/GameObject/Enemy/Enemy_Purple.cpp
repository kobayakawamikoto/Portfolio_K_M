#include "Enemy_Purple.h"
#include "../Ground/Ground.h"
#include "../Source/Utility/BoxCollisionParams.h"
#include "../SampleObject/SampleObject.h"
#include "../Source/Scene/SampleScene/SampleScene.h"
#include <algorithm>
#define NOMINMAX
#include "DxLib.h"

Enemy_Purple::Enemy_Purple()
	: loaded_sprite_handle(0), chara_act(), enemyState(), ground(), bcp(), sampleObject_()
{
}

Enemy_Purple::~Enemy_Purple()
{
	Finalize();
}

void Enemy_Purple::Initialize()
{
	__super::Initialize();

	// 画像の読み込み
	loaded_sprite_handle = LoadDivGraph(_T("Resources/Images/murasuke.bmp"), 12, 4, 3, 40, 48, chara_act);

	// サウンド読み込み
	hitSound = LoadSoundMem("Resources/Sounds/hit.wav");
	deadSound = LoadSoundMem("Resources/Sounds/pound.wav");

	ground = new Ground();
	bcp = new BoxCollisionParams();
	gravity.y = .2f;

	// 初期座標設定
	Vector2D initialVec;
	initialVec.x = 500.f;
	initialVec.y = 400.f;
	SetPosition(initialVec);

	// サンプルシーンからサンプルオブジェクトを取ってくる
	sampleObject_ = SampleScene::GetSampleObject();

	// 往復行動のため最初の位置を取っておく
	prevPosition_x = position.x;
	prevPosition_y = position.y;
	isLeftDir = true;
}

void Enemy_Purple::Update(float delta_seconds)
{
	__super::Update(delta_seconds);
	if (isDead)
	{
		return;
	}

	EnemyState currentState = enemyState;

	// 約1フレーム前の位置を覚えておき、コリジョン用に使う
	prev_x = GetPosition().x;
	prev_y = GetPosition().y;
	
	// 左右に往復する
	Move(delta_seconds);

	// プレイヤーと接触
	HitPlayer();

	// 弾丸と接触
	HitBullet();

	// ブロックの当たり判定
	DetectMapchip();

	if (enemyState != currentState) // アニメーションフレームがそれぞれ違うのでここで初期化
	{
		act_index = 0;
	}
}

void Enemy_Purple::Draw(const Vector2D& screen_offset)
{
	__super::Draw(screen_offset);

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

	// キャラ描画
	DrawRotaGraph(x + screen_offset.x, y + screen_offset.y, 1, 0, chara_act[motion_index], true, !isTurnLeft);
}

void Enemy_Purple::Finalize()
{
	__super::Finalize();

	// 画像の破棄
	for (int i = 0; i < 10; i++)
	{
		DeleteGraph(chara_act[i]);
	}
}

/*
*	アニメーションフレームスピードの設定
*/
void Enemy_Purple::Chara_AnimFrame()
{
	if (--act_wait <= 0)
	{
		act_index++;
		act_wait = ACT_SPEED; // アニメーションスピード
		act_index %= max_motion_index; // 0に戻す
	}
}

/*
*	プレイヤーと接触した時
*/
void Enemy_Purple::HitPlayer()
{
	int chara_x = static_cast<int>(sampleObject_->GetPosition().x - 14.f);
	int chara_y = static_cast<int>(sampleObject_->GetPosition().y + 2.f);
	int chara_width = static_cast<int>(sampleObject_->GetPosition().x - 14.f + 27);
	int chara_height = static_cast<int>(sampleObject_->GetPosition().y + 2.f + 38);
	int enemy_x = static_cast<int>(GetPosition().x - 14.f);
	int enemy_y = static_cast<int>(GetPosition().y - 10.f);
	int enemy_width = static_cast<int>(GetPosition().x - 14.f + 27);
	int enemy_height = static_cast<int>(GetPosition().y - 10.f + 32);

	// 敵コリジョン範囲と重なればプレーヤーが死ぬ
	if (bcp->CheckHit(chara_x, chara_y, chara_width, chara_height,
		enemy_x, enemy_y, enemy_width, enemy_height))
	{
		sampleObject_->Dead();
	}
}

/*
*	弾に被弾した時
*/
void Enemy_Purple::HitBullet()
{
	for (int i = 0; i < 20; i++)
	{
		int bullet_x = static_cast<int>(sampleObject_->GetBullet(i).x);
		int bullet_y = static_cast<int>(sampleObject_->GetBullet(i).y);
		int bullet_width = static_cast<int>(sampleObject_->GetBullet(i).x + 1);
		int bullet_height = static_cast<int>(sampleObject_->GetBullet(i).y + 1);
		int enemy_x = static_cast<int>(GetPosition().x - 14.f);
		int enemy_y = static_cast<int>(GetPosition().y - 10.f);
		int enemy_width = static_cast<int>(GetPosition().x - 14.f + 27);
		int enemy_height = static_cast<int>(GetPosition().y - 10.f + 32);

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
void Enemy_Purple::DetectMapchip()
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
void Enemy_Purple::Move(float delta_seconds)
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
