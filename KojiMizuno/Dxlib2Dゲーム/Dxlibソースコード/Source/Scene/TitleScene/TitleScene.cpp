#include "TitleScene.h"
#include "../../SystemTypes.h"
#include "../../GameObject/SampleObject/SampleObject.h"
#include "../../GameObject/Enemy/Enemy_Purple.h"
#include "../../GameObject/Ground/Ground.h"
#include "DxLib.h"


TitleScene::TitleScene()
{
}

void TitleScene::Initialize()
{
	// 親クラスのInitialize()
	__super::Initialize();
	loaded_title_handle = LoadGraph("Resources/Images/title.jpg");
	GraphFilter(loaded_title_handle, DX_GRAPH_FILTER_MONO, -60, 7);

	// 白色の値を取得
	Cr = GetColor(255, 255, 255);
	// 描画する文字列のサイズを設定
	SetFontSize(32);
	// フォントのタイプをエッジつきアンチエイリアスフォントに変更
	ChangeFontType(DX_FONTTYPE_ANTIALIASING_EDGE);
}

SceneType TitleScene::Update(float delta_seconds)
{
	// 親クラスのUpdate()
	return __super::Update(delta_seconds);
}

void TitleScene::Draw()
{
	// 親クラスのDraw()
	__super::Draw();

	DrawExtendGraph(0, 0, 640, 530, loaded_title_handle, true);
	// 文字列の描画
	DrawString(110, 300, "Start Press Space Key", Cr);
}

void TitleScene::Finalize()
{
	// 親クラスのFinalize()
	__super::Finalize();
}
