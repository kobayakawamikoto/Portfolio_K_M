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
	// �e�N���X��Initialize()
	__super::Initialize();
	loaded_title_handle = LoadGraph("Resources/Images/title.jpg");
	GraphFilter(loaded_title_handle, DX_GRAPH_FILTER_MONO, -60, 7);

	// ���F�̒l���擾
	Cr = GetColor(255, 255, 255);
	// �`�悷�镶����̃T�C�Y��ݒ�
	SetFontSize(32);
	// �t�H���g�̃^�C�v���G�b�W���A���`�G�C���A�X�t�H���g�ɕύX
	ChangeFontType(DX_FONTTYPE_ANTIALIASING_EDGE);
}

SceneType TitleScene::Update(float delta_seconds)
{
	// �e�N���X��Update()
	return __super::Update(delta_seconds);
}

void TitleScene::Draw()
{
	// �e�N���X��Draw()
	__super::Draw();

	DrawExtendGraph(0, 0, 640, 530, loaded_title_handle, true);
	// ������̕`��
	DrawString(110, 300, "Start Press Space Key", Cr);
}

void TitleScene::Finalize()
{
	// �e�N���X��Finalize()
	__super::Finalize();
}
