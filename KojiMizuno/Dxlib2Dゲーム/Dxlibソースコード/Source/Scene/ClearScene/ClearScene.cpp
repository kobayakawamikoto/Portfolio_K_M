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
	// �e�N���X��Initialize()
	__super::Initialize();

	// ���F�̒l���擾
	Cr = GetColor(255, 255, 255);
	// �`�悷�镶����̃T�C�Y��ݒ�
	SetFontSize(32);
	// �t�H���g�̃^�C�v���G�b�W���A���`�G�C���A�X�t�H���g�ɕύX
	ChangeFontType(DX_FONTTYPE_ANTIALIASING_EDGE);

	savePosition = SavePosition::GetSavePosition();
}

SceneType ClearScene::Update(float delta_seconds)
{
	// �e�N���X��Update()
	return __super::Update(delta_seconds);
}

void ClearScene::Draw()
{
	// �e�N���X��Draw()
	__super::Draw();

	// ������̕`��
	DrawString(150, 150, "�Q�[���N���A!!", Cr);
	DrawFormatString(150, 250, 100, "�Q�[���I�[�o�[��: %d ", savePosition->GetDeathCount());
	DrawString(50, 350, "�G���^�[�L�[�������Ă�������", Cr);
}

void ClearScene::Finalize()
{
	// �e�N���X��Finalize()
	__super::Finalize();
}
