#include "Enemy_Red.h"
#include "../Ground/Ground.h"
#include "../Source/Utility/BoxCollisionParams.h"
#include "../SampleObject/SampleObject.h"
#include "../Source/Scene/SampleScene/SampleScene.h"
#include <algorithm> // std::maxを使用するため
#define NOMINMAX
#include "DxLib.h"

Enemy_Red::Enemy_Red()
	: loaded_sprite_handle(0), chara_act(), isTurnLeft(false), motion_index(0), enemyState(), isAttack(false), ground(), bcp(), prev_x(), prev_y(), canJump(false), jumpTime(0), isJumpOnce(false), isJumping(false), isGround(false),
	isJumpKeyLeave(false), jumpCount(0), maxJumpCount(0), remainingJumps(0), sampleObject_(), addSpeed(3.f)
{
}

Enemy_Red::~Enemy_Red()
{
	Finalize();
}

void Enemy_Red::Initialize()
{
	__super::Initialize();

	// 画像の読み込み
	loaded_sprite_handle = LoadDivGraph(_T("Resources/Images/akasuke.bmp"), 8, 4, 2, 40, 48, chara_act);
	// サウンドの読み込み
	hitSound = LoadSoundMem("Resources/Sounds/hit.wav");
	deadSound = LoadSoundMem("Resources/Sounds/pound.wav");

	ground = new Ground();
	bcp = new BoxCollisionParams();

	gravity.y = .2f;
	maxJumpCount = 2;

	// 初期座標設定
	Vector2D initialVec;
	initialVec.x = 150.f;
	initialVec.y = 270.f;
	SetPosition(initialVec);

	// サンプルシーンからサンプルオブジェクトを取ってくる
	sampleObject_ = SampleScene::GetSampleObject();
	hp = 3;
	prevPosition_x = position.x;
	prevPosition_y = position.y;
	isLeftDir = true;
}

void Enemy_Red::Update(float delta_seconds)
{
	__super::Update(delta_seconds);
	if (isDead)
	{
		return;
	}

	EnemyState2 currentState = enemyState;

	Vector2D input_dir;

	// 前回の位置を覚えておく
	prev_x = GetPosition().x;
	prev_y = GetPosition().y;

	// プレイヤーを見つけたら
	if (isFindPlayer)
	{
		if (GetPosition().x < sampleObject_->GetPosition().x)
		{
			Vector2D vec;
			vec.y = GetPosition().y;
			vec.x = GetPosition().x;
			vec.x += 1 * addSpeed;
			SetPosition(vec);
			isTurnLeft = false;
		}
		else
		{
			Vector2D vec;
			vec.y = GetPosition().y;
			vec.x = GetPosition().x;
			vec.x -= 1 * addSpeed;
			SetPosition(vec);
			isTurnLeft = true;
		}

		if (canAttack)
		{
			enemyState = EnemyState2::ATTACK;
			addSpeed = 5.8f;
		}
		else
		{
			enemyState = EnemyState2::RUN;
			addSpeed = 3.f;

		}
	}

	// 往復する行動
	if (!isFindPlayer)
	{
		if (position.x >= prevPosition_x - 50.f && isLeftDir)
		{
			input_dir.x += -1;
			isTurnLeft = true;
			if (isGround == true && !isDamaged)
			{
				enemyState = EnemyState2::RUN;
			}
		}
		else if (position.x <= prevPosition_x + 50.f)
		{
			input_dir.x += 1;
			isTurnLeft = false;
			if (isGround == true && !isDamaged)
			{
				enemyState = EnemyState2::RUN;
			}
			isLeftDir = false;
		}
		else
		{
			isLeftDir = true;
		}
	}
	
	const float MOVEMENT_SPEED = 30.0f;
	delta_position = input_dir.Normalize() * MOVEMENT_SPEED * delta_seconds;
	SetPosition(GetPosition() + delta_position + yadd);
	yadd.y += gravity.y + 0.1f;
	yadd.y = std::min(10.0f, std::max(0.0f, yadd.y));

	// プレイヤーを検知
	if (isLeftDir)
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
		else
		{
			isFindPlayer = false;
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
		else
		{
			isFindPlayer = false;
		}
	}
	

	// プレイヤーと接触
	if (bcp->CheckHit(static_cast<int>(sampleObject_->GetPosition().x - 14.f), static_cast<int>(sampleObject_->GetPosition().y + 2.f), static_cast<int>(sampleObject_->GetPosition().x - 14.f + 27), static_cast<int>(sampleObject_->GetPosition().y + 2.f + 38),
		static_cast<int>(GetPosition().x - 14.f), static_cast<int>(GetPosition().y - 10.f), static_cast<int>(GetPosition().x - 14.f + 27), static_cast<int>(GetPosition().y - 10.f + 32)))
	{
		sampleObject_->Dead();
	}

	// 弾丸と接触
	for (int i = 0; i < 20; i++)
	{
		if (bcp->CheckHit(static_cast<int>(sampleObject_->GetBullet(i).x), static_cast<int>(sampleObject_->GetBullet(i).y), static_cast<int>(sampleObject_->GetBullet(i).x + 1), static_cast<int>(sampleObject_->GetBullet(i).y + 1),
			static_cast<int>(GetPosition().x - 14.f), static_cast<int>(GetPosition().y - 10.f), static_cast<int>(GetPosition().x - 14.f + 27), static_cast<int>(GetPosition().y - 10.f + 32)))
		{
			sampleObject_->SetBullet_x(i, 10000.f);
			sampleObject_->SetBullet_y(i, 10000.f);
			hp--;
			enemyState = EnemyState2::DAMAGE;
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

	// DAMAGEステートを暫く維持する
	if (isDamaged)
	{
		damageTimer++;
		if (damageTimer > 5)
		{
			isDamaged = false;
			damageTimer = 0;
		}
	}

	// ブロックの当たり判定
	for (int j = 0; j < 12; j++) // 行数
	{
		for (int i = 0; i < ground->GetNUM_MAP_X(); i++) { // 列数
			int no = ground->data[i + j * ground->GetNUM_MAP_X()];
			if (no != 7 && no != 11 && no != 15 && no != 19)
			{
				if (bcp->CheckHit(i * 32 - ground->GetMAP_OFFSET_X(), j * 40, i * 32 + 32 - ground->GetMAP_OFFSET_X(), j * 40 + 40, static_cast<int>(GetPosition().x - 14.f), static_cast<int>(GetPosition().y - 10.f), static_cast<int>(GetPosition().x - 14.f + 27), static_cast<int>(GetPosition().y - 10.f + 32)))
				{
					// 上から当たっているかチェック
					if (GetPosition().y >= prev_y)
					{
						// 地形よりも上から当たっているかチェックする
						// その場合のみYを補正
						// そうでない場合は横から当たっている
						if (j * 40 - 21 >= prev_y)
						{
							// ④地形に上から当たっている
							Vector2D vec;
							vec.x = prev_x;
							prev_y = static_cast<float>(j * 40 - 21);
							vec.y = prev_y;
							if (GetPosition().y != prev_y)
							{
								SetPosition(vec);
							}
							yadd.y = 0.0f;
							isGround = true;
						}
						else
						{
							// ⑤地形の横から当たっている
							// マスクするためintを使用します (floatだと、割って掛けると同じ値になってしまう)
							Vector2D vec;
							vec.y = GetPosition().y;
							int _x = static_cast<int>(std::max(GetPosition().x, prev_x));
							_x /= 20;
							_x *= 20;
							vec.x = static_cast<float>(_x);
							if (GetPosition().x != _x) // (GetPosition().x == _x)の場合下から上にすり抜ける床になる
							{
								SetPosition(vec);
							}
						}
					}
					// 下から当たっているかチェック
					else if (GetPosition().y < prev_y)
					{
						// 地形よりも下から当たっているかチェックする
						// その場合のみYを補正
						// そうでない場合は横から当たっている
						if ((j + 1) * 40.f <= prev_y)
						{
							Vector2D vec;
							vec.x = prev_x;
							prev_y = (j + 1) * 40.f;
							vec.y = prev_y;
							if (GetPosition().y != prev_y)
							{
								SetPosition(vec);
							}
							yadd.y = 0.0f;
						}
						else
						{
							// 地形の横から当たっている
							// マスクするためintを使用します (floatだと、割って掛けると同じ値になってしまう)
							Vector2D vec;
							vec.y = GetPosition().y;
							int _x = static_cast<int>(std::max(GetPosition().x, prev_x));
							_x /= 20;
							_x *= 20;
							vec.x = static_cast<float>(_x);
							if (GetPosition().x != _x) // (GetPosition().x == _x)の場合下から上にすり抜ける床になる
							{
								SetPosition(vec);
							}
						}
					}
				}
			}
		}
	}


	if (enemyState != currentState) // アニメーションフレームがそれぞれ違うのでここで初期化
	{
		act_index = 0;
	}
}

void Enemy_Red::Draw(const Vector2D& screen_offset)
{
	__super::Draw(screen_offset);

	int x, y;
	GetPosition().ToInt(x, y);
	Chara_AnimFrame();
	switch (enemyState)
	{
	case (EnemyState2::IDLE):
		motion_index = act_idle[act_index];
		max_motion_index = sizeof(act_idle) / sizeof(int);
		break;
	case (EnemyState2::RUN):
		motion_index = act_run[act_index];
		max_motion_index = sizeof(act_run) / sizeof(int);
		break;
	case (EnemyState2::ATTACK):
		motion_index = act_attack[act_index];
		max_motion_index = sizeof(act_attack) / sizeof(int);
		break;
	case (EnemyState2::JUMP):
		motion_index = act_jump[act_index];
		max_motion_index = sizeof(act_jump) / sizeof(int);
		break;
	case (EnemyState2::DAMAGE):
		motion_index = act_damage[act_index];
		max_motion_index = sizeof(act_damage) / sizeof(int);
		break;
	default:
		break;
	}

	// キャラ描画
	DrawRotaGraph(x - screen_offset.x, y - screen_offset.y, 1, 0, chara_act[motion_index], true, !isTurnLeft);

	/*
		if (isTurnLeft)
	{
		DrawBox(static_cast<int>(GetPosition().x - 340.f), static_cast<int>(GetPosition().y - 10.f), static_cast<int>(GetPosition().x - 140.f + 150), static_cast<int>(GetPosition().y - 10.f + 32), 100, false);
		if (canAttack)
		{
			DrawBox(static_cast<int>(GetPosition().x - 140.f), static_cast<int>(GetPosition().y - 10.f), static_cast<int>(GetPosition().x - 140.f + 120), static_cast<int>(GetPosition().y - 10.f + 32), 0, false);
		}
	}
	else
	{
		DrawBox(static_cast<int>(GetPosition().x - 14.f), static_cast<int>(GetPosition().y - 10.f), static_cast<int>(GetPosition().x - 14.f + 500), static_cast<int>(GetPosition().y - 10.f + 32), 100, false);
		if (canAttack)
		{
			DrawBox(static_cast<int>(GetPosition().x - 14.f), static_cast<int>(GetPosition().y - 10.f), static_cast<int>(GetPosition().x - 14.f + 100), static_cast<int>(GetPosition().y - 10.f + 32), 0, false);
		}
	}
	*/
	
	
	//DrawBox(static_cast<int>(GetPosition().x - 14.f), static_cast<int>(GetPosition().y - 10.f), static_cast<int>(GetPosition().x - 14.f + 27), static_cast<int>(GetPosition().y - 10.f + 32), 100, false);
	//DrawBox(SampleObject::PPosition_x - 14.f, SampleObject::PPosition_y + 2.f, SampleObject::PPosition_x - 14.f + 27, SampleObject::PPosition_y + 2.f + 38, 200, true);
	//DrawString(100, 150, hitTop.c_str(), GetColor(255, 255, 255));
	//DrawString(100, 200, hitSide.c_str(), GetColor(255, 255, 255));
}

void Enemy_Red::Finalize()
{
	__super::Finalize();

	// 画像の破棄
	for (int i = 0; i < 10; i++)
	{
		DeleteGraph(chara_act[i]);
	}

	//SampleScene* sampleScene = SampleScene::GetSampleScene();
	//GetOwnerScene()->DestroyObject(this);
	//SampleScene* sampleScene = SampleScene::GetSampleScene();
	//sampleScene->DestroyObject(sampleScene->enemy_Purple);
	//delete(sampleScene->enemy_Purple);
}

void Enemy_Red::Chara_AnimFrame()
{
	if (--act_wait <= 0)
	{
		act_index++;
		act_wait = ACT_SPEED; // アニメーションスピード
		act_index %= max_motion_index; // 0に戻す
	}
}