#include "SampleObject.h"
#include "../Ground/Ground.h"
#include "../Source/Utility/BoxCollisionParams.h"
#include "../Source/Utility/SavePosition.h"
#include "../Source/Scene/SampleScene/SampleScene.h"
#include <algorithm>
#include <random>
#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#define NOMINMAX
#include "DxLib.h"

// コリジョンを付けたい場所を手動で設定 第1引数:x座標, 第２引数:y座標, 第３引数:x座標+width, 第４引数:y座標+height
//int colPosition[][4] = { { 0, 440, 800, 480 }, {512, 200, 512 + 32, 200 + 160} };

SampleObject::SampleObject()
	: loaded_sprite_handle(), chara_act(), motion_index(0), playerState(), playerDirection(), ground(), bcp(),
	jumpCount(0)
{
}

SampleObject::~SampleObject()
{
	Finalize();
}

void SampleObject::Initialize()
{
	__super::Initialize();

	// キャラ画像の読み込み
	loaded_sprite_handle = LoadDivGraph(_T("Resources/Images/collon.bmp"), 27, 4, 7, 128, 128, chara_act);
	loaded_gameover_handle = LoadGraph("Resources/Images/gameover.png");
	// 初期位置設定
	Vector2D initialVec;
	initialVec.x = 100.f;
	initialVec.y = 400.f;
	//initialVec.x = -600.f;
	//initialVec.y = 10.f;
	//initialVec.x = 100.f;
	//initialVec.y = -310.f;
	SetPosition(initialVec);
	// 現在の座標をSavePositionクラスに格納
	savePosition = SavePosition::GetSavePosition();
	if (savePosition->GetCachePosition().x != 0 && savePosition->GetCachePosition().y != 0)
	{
		SetPosition(savePosition->GetCachePosition());
	}

	ground = new Ground();
	bcp = new BoxCollisionParams();

	gravity.y = .2f; // 重力
	playerDirection = PlayerDirection::RIGHT;

	// サウンド読み込み
	gameOverSound = LoadSoundMem("Resources/Sounds/gameover.mp3");
	fireSound = LoadSoundMem("Resources/Sounds/fire2.wav");
	jump1Sound = LoadSoundMem("Resources/Sounds/jump1.wav");
	jump2Sound = LoadSoundMem("Resources/Sounds/jump2.wav");
	clearSound = LoadSoundMem("Resources/Sounds/clear.ogg");
	bgm1 = LoadSoundMem("Resources/Sounds/12.ogg");
	ChangeVolumeSoundMem(255 * 30 / 100, bgm1);
	PlaySoundMem(bgm1, DX_PLAYTYPE_LOOP);
	SetFontSize(20);

	// 死体画像読み込み
	for (int i = 0; i <= 10; i++)
	{
		// ファイル名を生成
		std::stringstream filename;
		filename << "Resources/Images/b_collon" << i + 1 << ".bmp";

		// 画像をロードし、ハンドルを配列に格納
		loaded_bcollon_handle[i] = LoadGraph(filename.str().c_str());
	}
	// 死体画像の位置をランダムで生成
	for (int i = 0; i < blood_num + 1; i++)
	{
		std::random_device rd; // ハードウェアの乱数ジェネレータを使用してシードを生成
		std::mt19937 gen(rd()); // メルセンヌ・ツイスタ法を使用する乱数生成器
		std::uniform_real_distribution<float> distribution(-20.0, 20.0); // 1.0から10.0までのfloatを生成する分布

		random_number = distribution(gen); // ランダムなfloatを生成
		// 生成された値が小さいときに再度生成する補正
		if (random_number <= .8f && random_number >= -.8f)
		{
			random_number = distribution(gen);
		}
		randomVal[i] = (random_number);
	}
}

void SampleObject::Update(float delta_seconds)
{
	__super::Update(delta_seconds);

	if (isCleared)
	{
		return;
	}

	PlayerState currentState = playerState;

	// 前回の位置を覚えておく
	prev_x = GetPosition().x;
	prev_y = GetPosition().y;

	// 射撃
	Attack(delta_seconds);

	// ２段ジャンプ
	Jump();

	// アニメーションフレームがそれぞれ違うのでここで初期化
	if (playerState != currentState)
	{
		act_index = 0;
	}

	// 移動に関する処理
	Move(delta_seconds);

	/*
	** コリジョン判定タイプ１
	* 任意の場所に手動でコリジョンを設定する
	*/
	//DetectManualMapchip();

	/*
	** コリジョン判定タイプ２
	* マップチップの番号でコリジョン判定する
	*/
	DetectMapchip();

	// 流血表現
	Bleed();
}

void SampleObject::Draw(const Vector2D& screen_offset)
{
	__super::Draw(screen_offset);
	int x, y;
	GetPosition().ToInt(x, y);
	// アニメーションの速さとアニメーションのフレーム番号更新
	Chara_AnimFrame();
	// ステートでアニメーション分岐・描画
	switch (playerState)
	{
	case (PlayerState::IDLE):
		motion_index = act_idle[act_index];
		max_motion_index = sizeof(act_idle) / sizeof(int);
		break;
	case (PlayerState::RUN):
		motion_index = act_run[act_index];
		max_motion_index = sizeof(act_run) / sizeof(int);
		break;
	case (PlayerState::ATTACK):
		motion_index = act_attack[act_index];
		max_motion_index = sizeof(act_attack) / sizeof(int);
		break;
	case (PlayerState::JUMP):
		motion_index = act_jump[act_index];
		max_motion_index = sizeof(act_jump) / sizeof(int);
		break;
	default:
		break;
	}

	// キャラ描画
	if (isDead)
	{
		// 死亡したらキャラ描画させない
		Finalize();
		// ｹﾞｰﾑｵﾊﾞｰ画像表示
		DrawExtendGraph(0, 0, 640, 480, loaded_gameover_handle, true);
	}
	else
	{
		DrawRotaGraph(x - screen_offset.x, y - screen_offset.y, 1, 0, chara_act[motion_index], true, isTurnLeft);
	}

	// 死亡画像描画
	if (isDead)
	{
		// 血の描画
		for (int i = 0; i < blood_num; i++)
		{
			DrawCircle(bloodVec[i].x - screen_offset.x, bloodVec[i].y - screen_offset.y, 1, GetColor(255, 0, 0), TRUE);
		}
		// バラバラのコロンちゃん描画
		DrawGraph(bloodVec[10].x - screen_offset.x, bloodVec[10].y - screen_offset.y, loaded_bcollon_handle[0], true);
		DrawGraph(bloodVec[20].x - screen_offset.x, bloodVec[20].y - screen_offset.y, loaded_bcollon_handle[1], true);
		DrawGraph(bloodVec[30].x - screen_offset.x, bloodVec[30].y - screen_offset.y, loaded_bcollon_handle[2], true);
		DrawGraph(bloodVec[40].x - screen_offset.x, bloodVec[40].y - screen_offset.y, loaded_bcollon_handle[3], true);
		DrawGraph(bloodVec[50].x - screen_offset.x, bloodVec[50].y - screen_offset.y, loaded_bcollon_handle[4], true);
		DrawGraph(bloodVec[60].x - screen_offset.x, bloodVec[60].y - screen_offset.y, loaded_bcollon_handle[5], true);
		DrawGraph(bloodVec[70].x - screen_offset.x, bloodVec[70].y - screen_offset.y, loaded_bcollon_handle[6], true);
		DrawGraph(bloodVec[80].x - screen_offset.x, bloodVec[80].y - screen_offset.y, loaded_bcollon_handle[7], true);
		DrawGraph(bloodVec[90].x - screen_offset.x, bloodVec[90].y - screen_offset.y, loaded_bcollon_handle[8], true);
		DrawGraph(bloodVec[1].x - screen_offset.x, bloodVec[1].y - screen_offset.y, loaded_bcollon_handle[9], true);
		DrawGraph(bloodVec[2].x - screen_offset.x, bloodVec[2].y - screen_offset.y, loaded_bcollon_handle[10], true);
	}

	// 弾画像描画
	for (int i = 0; i < 20; i++)
	{
		DrawCircle(bullet[i].x - screen_offset.x, bullet[i].y - screen_offset.y, 1, GetColor(255, 255, 255), TRUE);
	}

	//DrawFormatString(0, 100, 100, "collon_x %f ", GetPosition().x);
	//DrawFormatString(0, 200, 200, "collon_y %f ", GetPosition().y);

	float pos_x = GetPosition().x;
	float pos_y = GetPosition().y;
	int chara_x = static_cast<int>(pos_x - offset_pos_x);
	int chara_y = static_cast<int>(pos_y + offset_pos_y);
	int chara_width = static_cast<int>(pos_x - offset_pos_x + offset_pos_width);
	int chara_height = static_cast<int>(pos_y + offset_pos_y + offset_pos_height);
	//DrawBox(chara_x - screen_offset.x, chara_y - screen_offset.y, chara_width - screen_offset.x, chara_height - screen_offset.y, 400, true);
}

void SampleObject::Finalize()
{
	__super::Finalize();

	// 画像の破棄
	DeleteGraph(loaded_sprite_handle);
	loaded_sprite_handle = 0;
}

/*
*	アニメーションフレームスピードの設定
*/
void SampleObject::Chara_AnimFrame()
{
	if (--act_wait <= 0)
	{
		act_index++;
		act_wait = ACT_SPEED; // アニメーションスピード
		act_index %= max_motion_index; // 0に戻す
	}
}

/*
* 射撃の処理
*/
void SampleObject::Attack(float delta_seconds)
{
	int leftButtonState = GetMouseInput();
	// ボタンが離された瞬間を検知
	if ((prevLeftButtonState & MOUSE_INPUT_LEFT) && !(leftButtonState & MOUSE_INPUT_LEFT)) {
		// ボタンが離されたときの処理をここに記述する
		canShot = true;
	}
	prevLeftButtonState = leftButtonState; // 現在の状態を保存

	// 弾の発射(左クリック)
	if (GetMouseInput() & MOUSE_INPUT_LEFT && canShot && !isDead)
	{
		for (int i = 0; i < 20; ++i) {
			if (bullet[i].flag == false) {
				bullet[i].flag = true;
				bullet[i].x = position.x;
				bullet[i].y = position.y + 20;
				bullet[i].pre_x = position.x;
				bullet[i].playerDirection = playerDirection;
				canShot = false;
				break;
			}
		}
		playerState = PlayerState::ATTACK;
		isAttacked = true;
		ChangeVolumeSoundMem(255 * 30 / 100, fireSound);
		PlaySoundMem(fireSound, DX_PLAYTYPE_BACK);
	}

	// 連射間隔制限
	if (isAttacked)
	{
		attackTimer++;
		if (attackTimer > 5)
		{
			isAttacked = false;
			attackTimer = 0;
		}
	}


	for (int i = 0; i < 20; ++i) {
		if (bullet[i].flag && bullet[i].playerDirection == PlayerDirection::LEFT) {
			bullet[i].x -= 1000.f * delta_seconds;

			// 弾の飛距離とそのあとの処理
			if (bullet[i].x < bullet[i].pre_x - 500.f) {
				bullet[i].flag = false;
				bullet[i].x = 10000;
				bullet[i].y = 10000;
			}
		}
	}

	for (int i = 0; i < 20; ++i) {
		if (bullet[i].flag && bullet[i].playerDirection == PlayerDirection::RIGHT) {
			bullet[i].x += 1000.f * delta_seconds;

			// 弾の飛距離とそのあとの処理
			if (bullet[i].x > bullet[i].pre_x + 500.f) {
				bullet[i].flag = false;
				bullet[i].x = 10000;
				bullet[i].y = 10000;
			}
		}
	}
}

/*
*	移動処理
*/
void SampleObject::Move(float delta_seconds)
{
	Vector2D input_dir;
	if (CheckHitKey(KEY_INPUT_A) == 1) // 右移動
	{
		input_dir.x = -1;
		isTurnLeft = true;
		if (isGround == true && !isAttacked)
		{
			playerState = PlayerState::RUN;
		}
		playerDirection = PlayerDirection::LEFT;
	}
	else if (CheckHitKey(KEY_INPUT_D) == 1) // 左移動
	{
		input_dir.x = 1;
		isTurnLeft = false;
		if (isGround == true && !isAttacked)
		{
			playerState = PlayerState::RUN;
		}
		playerDirection = PlayerDirection::RIGHT;
	}
	else
	{
		if (isGround == true && !isAttacked)
		{
			playerState = PlayerState::IDLE;
		}
	}

	// ポジションの設定
	const float MOVEMENT_SPEED = 300.0f;
	delta_position = input_dir.Normalize() * MOVEMENT_SPEED * delta_seconds;
	SetPosition(GetPosition() + delta_position + yadd); // キャラ座標の更新
	yadd.y += gravity.y + 10 * delta_seconds; // 重力の設定
	yadd.y = std::min(20.0f, std::max(0.0f, yadd.y));
}

/*
*	ジャンプ処理s
*/
void SampleObject::Jump()
{
	if (CheckHitKey(KEY_INPUT_SPACE) == 1 && canJump == true && !isDead)
	{
		if (!isGround && !isAttacked)
		{
			playerState = PlayerState::JUMP;
		}
		jumpTime++;
		if (!isJumping)
		{
			if (!isJumpOnce)
			{
				isGround = false; // TODO: これだと高いところから降りたときにJumpモーションに移行しない
				remainingJumps = maxJumpCount - 1;
				ChangeVolumeSoundMem(255 * 20 / 100, jump1Sound);
				PlaySoundMem(jump1Sound, DX_PLAYTYPE_BACK);
			}

			if (jumpTime < 10.f)
			{
				isJumpOnce = true;
				jump.y += -3.f;
				SetPosition(GetPosition() + jump);
			}
		}
		else if (isJumpKeyLeave)
		{
			act_index = 0;
			if (!isJumpOnce)
			{
				remainingJumps = maxJumpCount - 1;
				if (!isDead)
				{
					yadd.y = 0;
				}
				ChangeVolumeSoundMem(255 * 20 / 100, jump2Sound);
				PlaySoundMem(jump2Sound, DX_PLAYTYPE_BACK);
			}

			if (jumpTime < 10.f)
			{
				isJumpOnce = true;
				remainingJumps = remainingJumps - 1;
				jump.y += -3.f;
				SetPosition(GetPosition() + jump);
			}
			else
			{
				canJump = false;
				jumpTime = 0;
				jump.y = 0;
			}
		}
		else
		{
			canJump = false;
			jumpTime = 0;
			jump.y = 0;
		}
	}
	else
	{
		isJumping = true;
		isJumpKeyLeave = true;
		if (remainingJumps > 0)
		{
			jumpTime = 0;
		}
		jump.y = 0;
		isJumpOnce = false;
	}
}

void SampleObject::DetectManualMapchip()
{
	/*
	for (int i = 0; i < 3; i++)
	{
		// 当たり判定の是非
		if (bcp->CheckHit(colPosition[i][0], colPosition[i][1], colPosition[i][2], colPosition[i][3], static_cast<int>(GetPosition().x - 14.f), static_cast<int>(GetPosition().y + 2.f), static_cast<int>(GetPosition().x - 14.f + 27), static_cast<int>(GetPosition().y + 2.f + 38)))
		{
			// 上から当たっているかチェック
			if (GetPosition().y >= prev_y)
			{
				// 地形よりも上から当たっているかチェックする
				// その場合のみYを補正
				// そうでない場合は横から当たっている
				if (colPosition[i][1] - 38 >= prev_y)
				{
					// ④地形に上から当たっている
					Vector2D vec;
					vec.x = prev_x;
					prev_y = static_cast<float>(colPosition[i][1] - 38);
					vec.y = prev_y;
					if (GetPosition().y != prev_y)
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
				if (colPosition[i][3] <= prev_y + 2)
				{
					Vector2D vec;
					vec.x = prev_x;
					prev_y = colPosition[i][3];
					vec.y = prev_y;
					if (GetPosition().y != prev_y)
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
	}*/
}

/*
*	マップチップに対するコリジョン判定
*/
void SampleObject::DetectMapchip()
{
	for (int j = 0; j < ground->GetNUM_MAP_Y(); j++) // 行数分回す
	{
		for (int i = 0; i < ground->GetNUM_MAP_X(); i++) { // 列数分回す
			// groundクラスのマップ用配列の数字を取ってきてコリジョン計算をする
			int no = ground->data[i + j * ground->GetNUM_MAP_X()];
			if (no != 7 && no != 11 && no != 15 && no != 19)
			{
				// マップ/キャラのコリジョン用座標
				int mapchip_x = i * ground->GetPOS_MAP_X() - ground->GetMAP_OFFSET_X();
				int mapchip_y = j * ground->GetPOS_MAP_Y() - ground->GetMAP_OFFSET_Y();
				int mapchip_width = i * ground->GetPOS_MAP_X() + ground->GetPOS_MAP_WIDTH() - ground->GetMAP_OFFSET_X();
				int mapchip_height = j * ground->GetPOS_MAP_Y() + ground->GetPOS_MAP_HEIGHT() - ground->GetMAP_OFFSET_Y();
				int chara_x = static_cast<int>(GetPosition().x - offset_pos_x);
				int chara_y = static_cast<int>(GetPosition().y + offset_pos_y);
				int chara_width = static_cast<int>(GetPosition().x - offset_pos_x + offset_pos_width);
				int chara_height = static_cast<int>(GetPosition().y + offset_pos_y + offset_pos_height);

				// キャラとマップチップに対するコリジョン判定
				if (bcp->CheckHit(mapchip_x, mapchip_y, mapchip_width, mapchip_height, chara_x, chara_y, chara_width, chara_height))
				{
					// 上から当たっているかチェック
					if (GetPosition().y >= prev_y)
					{
						// コリジョンのあるマップチップが並んでいる場合それらを連結させて長方形の当たり判定にする
						if (ground->data[i + j * ground->GetNUM_MAP_X()] != ground->data[i + (j - 1) * ground->GetNUM_MAP_X()])
						{
							/*
							*	マップチップよりも上から当たっているかチェックする
							*	その場合のみy座標を固定する
							*/
							// マップチップに上から当たっている場合のポジション設定
							if ((j * 40 - 38) - ground->GetMAP_OFFSET_Y() >= prev_y)
							{
								HitFromAbove(j);
							}
							// マップチップに上からかつ横から当たっている場合のポジション設定
							else
							{
								HitFromSide();
							}
						}
						// マップチップに上からかつ横から当たっている場合のポジション設定
						else
						{
							HitFromSide();
						}
					}
					// 下から当たっているかチェック
					else if (GetPosition().y < prev_y)
					{
						// コリジョンのあるマップチップが並んでいる場合それらを連結させて長方形の当たり判定にする
						if (ground->data[i + j * ground->GetNUM_MAP_X()] != ground->data[i + (j + 1) * ground->GetNUM_MAP_X()])
						{
							/*
							*	マップチップよりも下から当たっているかチェックする
							*	その場合のみy座標を固定する
							*/
							// マップチップに下から当たっている場合のポジション設定
							if (((j + 1) * 40.f) - ground->GetMAP_OFFSET_Y() <= prev_y + 2)
							{
								HitFromBelow(j);
							}
							// マップチップに下からかつ横から当たっている場合のポジション設定
							else
							{
								HitFromSide();
							}
						}
						// マップチップに下からかつ横から当たっている場合のポジション設定
						else
						{
							HitFromSide();
						}
					}
				}
			}
		}
	}
}

/*
*	マップチップの上にキャラが当たった
*/
void SampleObject::HitFromAbove(int j) 
{
	Vector2D vec;
	vec.x = prev_x;
	prev_y = static_cast<float>((j * 40 - 38) - ground->GetMAP_OFFSET_Y()); // y座標を固定する
	vec.y = prev_y;
	if (GetPosition().y != prev_y)
	{
		SetPosition(vec);
	}
	if (!isDead)
	{
		yadd.y = 0.0f; // 重力
	}
	// ジャンプとベロシティのブール値更新
	GroundBool();
}

/*
*	マップチップの下にキャラが当たった
*/
void SampleObject::HitFromBelow(int j) // マップチップの下からキャラが当たった
{
	Vector2D vec;
	vec.x = GetPosition().x;
	prev_y = ((j + 1) * 40.f) - ground->GetMAP_OFFSET_Y();
	vec.y = prev_y;
	if (GetPosition().y != prev_y) // y座標を固定する
	{
		SetPosition(vec);
	}
	if (!isDead)
	{
		yadd.y = 0.0f; // 重力
	}
}

/*
*	マップチップの横にキャラが当たった
*/
void SampleObject::HitFromSide() // マップチップの横からキャラが当たった
{
	Vector2D vec;
	vec.y = GetPosition().y;
	int _x = 0;

	if (playerDirection == PlayerDirection::RIGHT)
	{
		_x = static_cast<int>(std::min(GetPosition().x, prev_x));
	}
	else
	{
		_x = static_cast<int>(std::max(GetPosition().x, prev_x));
	}
	// 押し戻し用(マスクするためintを使用します)
	_x /= 5;
	_x *= 5;
	vec.x = static_cast<float>(_x);
	if (GetPosition().x != _x) // (GetPosition().x == _x)の場合下から上にすり抜ける床になる
	{
		SetPosition(vec);
	}
}

/*
*	地面に接地時のステート
*/
void SampleObject::GroundBool()
{
	canJump = true; // ジャンプ可能かどうか
	isJumping = false; // 現在ジャンプしているか
	isJumpKeyLeave = false; // ジャンプキーが離されたかどうか
	isJumpOnce = false;// 一度ジャンプしたかどうか
	isGround = true; // 地面に接地しているか
	jumpTime = 0; // ジャンプしている時間(この秒数で上に上がるか下に下がるかを決めている)
	jump.y = 0; // y座標のベロシティ
}

/*
*	ゲームオーバー時の流血表現の処理
*/
void SampleObject::Bleed()
{
	if (isDead)
	{
		for (int i = 0; i < blood_num + 1; i++)
		{
			prev_blood_x[i] = bloodVec[i].x;
			prev_blood_y[i] = bloodVec[i].y;
		}
	}

	// 血の座標更新・コリジョン判定
	if (isDead)
	{
		// 血の量だけ生成したランダム座標を血に適用
		for (int i = 0; i < blood_num; i++)
		{
			bloodVec[i].x += randomVal[i];
			bloodVec[i].y += randomVal[i + 1];
			// 血のアニメーションの為に新たなベロシティ変数を作成して適用しています(addyadd)。
			addyadd += yadd.y * 0.00001;
			bloodVec[i].y += std::min(addyadd, 20.f);
		}

		// 血とマップチップのコリジョンを判定
		for (int j = 0; j < ground->GetNUM_MAP_Y(); j++) // 行数分回す
		{
			for (int i = 0; i < ground->GetNUM_MAP_X(); i++) { // 列数分回す
				int no = ground->data[i + j * ground->GetNUM_MAP_X()];
				if (no != 7 && no != 11 && no != 15 && no != 19)
				{
					for (int k = 0; k < blood_num + 1; k++)
					{
						// マップ/血のコリジョン用座標
						int mapchip_x = i * ground->GetPOS_MAP_X() - ground->GetMAP_OFFSET_X();
						int mapchip_y = j * ground->GetPOS_MAP_Y() - ground->GetMAP_OFFSET_Y();
						int mapchip_width = i * ground->GetPOS_MAP_X() + ground->GetPOS_MAP_WIDTH() - ground->GetMAP_OFFSET_X();
						int mapchip_height = j * ground->GetPOS_MAP_Y() + ground->GetPOS_MAP_HEIGHT() - ground->GetMAP_OFFSET_Y();
						int blood_x = static_cast<int>(bloodVec[k].x);
						int blood_y = static_cast<int>(bloodVec[k].y);
						int blood_width = static_cast<int>(bloodVec[k].x + 1.f);
						int blood_height = static_cast<int>(bloodVec[k].y + 1.f);

						// 血とマップチップに対するコリジョン判定
						if (bcp->CheckHit(mapchip_x, mapchip_y, mapchip_width, mapchip_height,
							blood_x, blood_y, blood_width, blood_height))
						{
							// 上から当たっているかチェック
							if (bloodVec[k].y >= prev_blood_y[k])
							{
								// コリジョンのあるマップチップが並んでいる場合それらを連結させて長方形の当たり判定にする
								if (ground->data[i + j * ground->GetNUM_MAP_X()] != ground->data[i + (j - 1) * ground->GetNUM_MAP_X()])
								{
									/*
									*	マップチップよりも上から当たっているかチェックする
									*	その場合のみy座標を固定する
									*/
									// マップチップに上から当たっている場合のポジション設定
									if ((j * 40 - 1) - ground->GetMAP_OFFSET_Y() >= prev_blood_y[k])
									{
										HitFromAbove_blood(j, k);
									}
									// マップチップに上からかつ横から当たっている場合のポジション設定
									else
									{
										HitFromSide_blood(k);
									}
								}
								// マップチップに上からかつ横から当たっている場合のポジション設定
								else
								{
									HitFromSide_blood(k);
								}
							}
							// マップチップの下から血があたっている
							else if (bloodVec[k].y < prev_blood_y[k])
							{
								// コリジョンのあるマップチップが並んでいる場合それらを連結させて長方形の当たり判定にする
								if (ground->data[i + j * ground->GetNUM_MAP_X()] != ground->data[i + (j + 1) * ground->GetNUM_MAP_X()])
								{
									/*
									*	マップチップよりも下から当たっているかチェックする
									*	その場合のみy座標を固定する
									*/
									// マップチップに下から当たっている場合のポジション設定
									if (((j + 1) * 40.f) - ground->GetMAP_OFFSET_Y() <= prev_blood_y[k] + 2)
									{
										HitFromBelow_blood(j, k);
									}
									// マップチップに下からかつ横から当たっている場合のポジション設定
									else
									{
										HitFromSide_blood(k);
									}
								}
								// マップチップに下からかつ横から当たっている場合のポジション設定
								else
								{
									HitFromSide_blood(k);
								}
							}
						}
					}
				}
			}
		}
	}
	else
	{
		// 血の座標を初期化
		for (int i = 0; i < blood_num; i++)
		{
			bloodVelocity = 0.f;
			bloodVec[i].x = GetPosition().x;
			bloodVec[i].y = GetPosition().y;
		}

	}
}

/*
*	マップチップの上に血が当たった
*/
void SampleObject::HitFromAbove_blood(int j, int k)
{
	Vector2D vec;
	vec.x = prev_blood_x[k];
	prev_blood_y[k] = static_cast<float>((j * 40 - 1) - ground->GetMAP_OFFSET_Y());
	vec.y = prev_blood_y[k];
	if (bloodVec[k].y != prev_blood_y[k])
	{
		bloodVec[k].x = vec.x;
		bloodVec[k].y = vec.y;
	}
}

/*
*	マップチップの下に血が当たった
*/
void SampleObject::HitFromBelow_blood(int j, int k)
{
	Vector2D vec;
	vec.x = prev_blood_x[k];
	prev_blood_y[k] = ((j + 1) * 40.f - ground->GetMAP_OFFSET_Y());
	vec.y = prev_blood_y[k];
	if (bloodVec[k].y != prev_blood_y[k])
	{
		bloodVec[k].x = vec.x;
		bloodVec[k].y = vec.y;
	}
}

/*
*	マップチップの横に血が当たった
*/
void SampleObject::HitFromSide_blood(int k)
{
	Vector2D vec;
	vec.y = bloodVec[k].y;
	int _x = static_cast<int>(std::max(bloodVec[k].x, prev_blood_x[k]));
	_x /= 10;
	_x *= 10;
	vec.x = static_cast<float>(_x);
	if (bloodVec[k].x != _x)
	{
		bloodVec[k].x = vec.x;
		bloodVec[k].y = vec.y;
	}
}

/*
*	ゲームオーバーした時
*/
void SampleObject::Dead()
{
	if (isDead == false)
	{
		StopSoundMem(bgm1);
		ChangeVolumeSoundMem(255 * 30 / 100, gameOverSound);
		PlaySoundMem(gameOverSound, DX_PLAYTYPE_BACK);
		savePosition->SetDeathCount(1);
	}

	isDead = true;
}

/*
*	クリアした時
*/
void SampleObject::Clear()
{
	if (isCleared == false)
	{
		// サウンドを鳴らして鳴り終えたらシーン遷移
		StopSoundMem(bgm1);
		ChangeVolumeSoundMem(255 * 40 / 100, clearSound);
		PlaySoundMem(clearSound, DX_PLAYTYPE_NORMAL);
		GetOwnerScene()->SetIsCleared(true);
	}
	isCleared = true;
}

/*
*	偽クリアした時
*/
void SampleObject::FakeClear()
{
	if (isCleared == false)
	{
		// サウンドを鳴らして鳴り終えたらシーン遷移
		StopSoundMem(bgm1);
		ChangeVolumeSoundMem(255 * 40 / 100, clearSound);
		PlaySoundMem(clearSound, DX_PLAYTYPE_NORMAL);
	}
	isCleared = true;
}

/*
*	セーブの処理
*/
void SampleObject::Save()
{
	savePosition->SetCachePosition(GetPosition());
}

