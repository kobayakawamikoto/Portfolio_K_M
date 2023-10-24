#include "ClearScene.h"
#include "../../SystemTypes.h"
#include "../../GameObject/SampleObject/SampleObject.h"
#include "../../GameObject/Enemy/Enemy_Purple.h"
#include "../../GameObject/Ground/Ground.h"
#include "DxLib.h"


ClearScene::ClearScene()
{
}

void ClearScene::Initialize()
{
	// 親クラスのInitialize()
	__super::Initialize();

	// 白色の値を取得
	Cr = GetColor(255, 255, 255);
	// 描画する文字列のサイズを設定
	SetFontSize(32);
	// フォントのタイプをエッジつきアンチエイリアスフォントに変更
	ChangeFontType(DX_FONTTYPE_ANTIALIASING_EDGE);

	savePosition = SavePosition::GetSavePosition();
}

SceneType ClearScene::Update(float delta_seconds)
{
	// 親クラスのUpdate()
	return __super::Update(delta_seconds);
}

void ClearScene::Draw()
{
	// 親クラスのDraw()
	__super::Draw();

	// 文字列の描画
	DrawString(150, 150, "ゲームクリア!!", Cr);
	DrawFormatString(150, 250, 100, "ゲームオーバー回数: %d ", savePosition->GetDeathCount());
	DrawString(50, 350, "エンターキーを押してください", Cr);
}

void ClearScene::Finalize()
{
	// 親クラスのFinalize()
	__super::Finalize();
}
