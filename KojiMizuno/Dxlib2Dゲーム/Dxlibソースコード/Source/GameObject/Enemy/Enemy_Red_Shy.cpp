#include "Enemy_Red_Shy.h"
#include "../Ground/Ground.h"
#include "../Source/Utility/BoxCollisionParams.h"
#include "../SampleObject/SampleObject.h"
#include "../Source/Scene/SampleScene/SampleScene.h"
#include <algorithm>
#define NOMINMAX
#include "DxLib.h"

Enemy_Red_Shy::Enemy_Red_Shy()
	: loaded_sprite_handle(0), chara_act(), enemyState(), ground(), bcp(), sampleObject_()
{
}

Enemy_Red_Shy::~Enemy_Red_Shy()
{
	Finalize();
}

void Enemy_Red_Shy::Initialize()
{
	__super::Initialize();

	// 画像読み込み
	loaded_sprite_handle = LoadDivGraph(_T("Resources/Images/akasuke.bmp"), 8, 4, 2, 40, 48, chara_act);
	loaded_parent_handle = LoadGraph(_T("Resources/Images/akasuke_wait.bmp"));
	// サウンド読み込み
	hitSound = LoadSoundMem("Resources/Sounds/hit.wav");
	deadSound = LoadSoundMem("Resources/Sounds/pound.wav");
	parentSound = LoadSoundMem("Resources/Sounds/splatter.wav");

	ground = new Ground();
	bcp = new BoxCollisionParams();

	gravity.y = .2f;

	// 初期座標設定
	Vector2D initialVec;
	initialVec.x = 1150.f;
	initialVec.y = 270.f;
	SetPosition(initialVec);

	// サンプルシーンからサンプルオブジェクトを取ってくる
	sampleObject_ = SampleScene::GetSampleObject();
	prevPosition_x = position.x;
	prevPosition_y = position.y;
}

void Enemy_Red_Shy::Update(float delta_seconds)
{
	__super::Update(delta_seconds);

	EnemyState currentState = enemyState;

	// 前回の位置を覚えておく
	prev_x = GetPosition().x;
	prev_y = GetPosition().y;

	Patrol(delta_seconds);

	// プレイヤーを検知
	DetectPlayer();

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

void Enemy_Red_Shy::Draw(const Vector2D& screen_offset)
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

	// キャラ描画
	if (isDead)
	{
		DrawExtendGraph(x - screen_offset.x - 300, y - screen_offset.y - 400, x - screen_offset.x + 100, y - screen_offset.y, chara_act[motion_index], true);
	}
	else
	{
		DrawRotaGraph(x - screen_offset.x, y - screen_offset.y, 1, 0, chara_act[motion_index], true, !isTurnLeft);
		DrawExtendGraph(1200 - screen_offset.x - change_parent_pos_x, 200 - screen_offset.y - change_parent_pos_y, 1500 - screen_offset.x - change_parent_pos_x, 500 - screen_offset.y - change_parent_pos_y, loaded_parent_handle, true);
	}
}

void Enemy_Red_Shy::Finalize()
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
void Enemy_Red_Shy::Chara_AnimFrame()
{
	if (--act_wait <= 0)
	{
		act_index++;
		act_wait = ACT_SPEED; // アニメーションスピード
		act_index %= max_motion_index; // 0に戻す
	}
}

/*
*	左右に往復する処理
*/
void Enemy_Red_Shy::Patrol(float delta_seconds)
{
	Vector2D input_dir;
	// プレイヤーを見つけたら追いかける
	if (isFindPlayer)
	{
		if (!isJumpOnce)
		{
			jumpTime++;

			if (jumpTime < 10.f)
			{
				Vector2D vec;
				vec.x = GetPosition().x;
				vec.y += GetPosition().y + -5.f;
				SetPosition(vec);
			}
			else
			{
				isJumpOnce = true;
			}
		}
		else
		{
			if (position.x <= prevPosition_x + 80.f)
			{
				MOVEMENT_SPEED = 120.f;
				input_dir.x += 1;
				isTurnLeft = false;
			}
		}
	}

	// 往復する行動
	if (!isFindPlayer)
	{
		if (position.x >= prevPosition_x - 8.f && isLeftDir)
		{
			input_dir.x += -1;
			isTurnLeft = true;
			if (isGround == true && !isDamaged)
			{
				enemyState = EnemyState::RUN;
			}
		}
		else if (position.x <= prevPosition_x + 8.f)
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
	}

	// ポジションの設定
	delta_position = input_dir.Normalize() * MOVEMENT_SPEED * delta_seconds;
	SetPosition(GetPosition() + delta_position + yadd);
	yadd.y += gravity.y + 0.1f; // 重力の設定
	yadd.y = std::min(10.0f, std::max(0.0f, yadd.y));
}

/*
*	プレイヤー発見時各種変数を変更する
*/
void Enemy_Red_Shy::DetectPlayer()
{
	if (isTurnLeft)
	{
		if (bcp->CheckHit(static_cast<int>(sampleObject_->GetPosition().x - 14.f), static_cast<int>(sampleObject_->GetPosition().y + 2.f), static_cast<int>(sampleObject_->GetPosition().x - 14.f + 27), static_cast<int>(sampleObject_->GetPosition().y + 2.f + 38),
			static_cast<int>(GetPosition().x - 340.f), static_cast<int>(GetPosition().y - 10.f), static_cast<int>(GetPosition().x - 140.f + 300), static_cast<int>(GetPosition().y - 10.f + 32)))
		{
			isFindPlayer = true;
			if (bcp->CheckHit(static_cast<int>(sampleObject_->GetPosition().x - 14.f), static_cast<int>(sampleObject_->GetPosition().y + 2.f), static_cast<int>(sampleObject_->GetPosition().x - 14.f + 27), static_cast<int>(sampleObject_->GetPosition().y + 2.f + 38),
				static_cast<int>(GetPosition().x - 140.f), static_cast<int>(GetPosition().y - 10.f), static_cast<int>(GetPosition().x - 140.f + 120), static_cast<int>(GetPosition().y - 10.f + 32)))
			{
				canAttack = true;
			}
			else
			{
				canAttack = false;
			}
		}
	}
	else
	{
		if (bcp->CheckHit(static_cast<int>(sampleObject_->GetPosition().x - 14.f), static_cast<int>(sampleObject_->GetPosition().y + 2.f), static_cast<int>(sampleObject_->GetPosition().x - 14.f + 27), static_cast<int>(sampleObject_->GetPosition().y + 2.f + 38),
			static_cast<int>(GetPosition().x - 14.f), static_cast<int>(GetPosition().y - 10.f), static_cast<int>(GetPosition().x - 14.f + 500), static_cast<int>(GetPosition().y - 10.f + 32)))
		{
			isFindPlayer = true;
			if (bcp->CheckHit(static_cast<int>(sampleObject_->GetPosition().x - 14.f), static_cast<int>(sampleObject_->GetPosition().y + 2.f), static_cast<int>(sampleObject_->GetPosition().x - 14.f + 27), static_cast<int>(sampleObject_->GetPosition().y + 2.f + 38),
				static_cast<int>(GetPosition().x - 14.f), static_cast<int>(GetPosition().y - 10.f), static_cast<int>(GetPosition().x - 14.f + 100), static_cast<int>(GetPosition().y - 10.f + 32)))
			{
				canAttack = true;
			}
			else
			{
				canAttack = false;
			}
		}
	}
}

/*
*	プレイヤーと接触した時
*/
void Enemy_Red_Shy::HitPlayer()
{
	// 敵のコリジョン範囲と重なればプレーヤーが死ぬ
	if (bcp->CheckHit(static_cast<int>(sampleObject_->GetPosition().x - 14.f), static_cast<int>(sampleObject_->GetPosition().y + 2.f), static_cast<int>(sampleObject_->GetPosition().x - 14.f + 27), static_cast<int>(sampleObject_->GetPosition().y + 2.f + 38),
		static_cast<int>(GetPosition().x - 14.f), static_cast<int>(GetPosition().y - 10.f), static_cast<int>(GetPosition().x - 14.f + 27), static_cast<int>(GetPosition().y - 10.f + 32)))
	{
		sampleObject_->Dead();
	}
}

/*
*	弾に被弾した時
*/
void Enemy_Red_Shy::HitBullet()
{
	// 弾とキャラが重なった時HPを減らすs
	for (int i = 0; i < 20; i++)
	{
		if (bcp->CheckHit(static_cast<int>(sampleObject_->GetBullet(i).x), static_cast<int>(sampleObject_->GetBullet(i).y), static_cast<int>(sampleObject_->GetBullet(i).x + 1), static_cast<int>(sampleObject_->GetBullet(i).y + 1),
			static_cast<int>(GetPosition().x - 14.f), static_cast<int>(GetPosition().y - 10.f), static_cast<int>(GetPosition().x - 14.f + 27), static_cast<int>(GetPosition().y - 10.f + 32)))
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
				isDead = true;
				enemyState = EnemyState::ATTACK;
				ChangeVolumeSoundMem(255 * 30 / 100, parentSound);
				sampleObject_->Dead();
			}
			else
			{
				ChangeVolumeSoundMem(255 * 30 / 100, hitSound);
				PlaySoundMem(hitSound, DX_PLAYTYPE_BACK);
				change_parent_pos_x += 30;
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
void Enemy_Red_Shy::DetectMapchip()
{
	for (int j = 0; j < ground->GetNUM_MAP_Y(); j++) // 行数分回す
	{
		for (int i = 0; i < ground->GetNUM_MAP_X(); i++) { // 列数分回す
			int no = ground->data[i + j * ground->GetNUM_MAP_X()];
			if (no != 7 && no != 11 && no != 15 && no != 19)
			{
				// キャラとマップチップに対するコリジョン判定
				if (bcp->CheckHit(i * 32 - ground->GetMAP_OFFSET_X(), (j * 40) - ground->GetMAP_OFFSET_Y(), i * 32 + 32 - ground->GetMAP_OFFSET_X(), (j * 40 + 40) - ground->GetMAP_OFFSET_Y(), static_cast<int>(GetPosition().x - 14.f), static_cast<int>(GetPosition().y - 10.f), static_cast<int>(GetPosition().x - 14.f + 27), static_cast<int>(GetPosition().y - 10.f + 32)))
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
						if ((j + 1) * 40.f <= prev_y)
						{
							Vector2D vec;
							vec.x = prev_x;
							prev_y = ((j + 1) - ground->GetMAP_OFFSET_Y()) * 40.f;
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
