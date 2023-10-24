#include "SceneManager.h"

#include <cassert>
#include "DxLib.h"

#include "SceneBase.h"
#include "SampleScene/SampleScene.h"
#include "TitleScene/TitleScene.h"
#include "ClearScene/ClearScene.h"

SceneManager::SceneManager()
	: current_scene(nullptr)
{
}

SceneManager::~SceneManager()
{
	Finalize();
}

void SceneManager::Initialize()
{
	// �J�n�V�[����SAMPLE_SCENE�ɐݒ�
	ChangeScene(SceneType::TITLE_SCENE);
}

void SceneManager::Update(float DeltaSeconds)
{
	// �V�[���̍X�V
	SceneType result_scene_type = current_scene->Update(DeltaSeconds); // SceneBase�ɃA�N�Z�X���Ă�
	if (CheckHitKey(KEY_INPUT_R) == 1)
	{
		// �������ׂč폜���܂�
		InitSoundMem();
		ChangeScene(result_scene_type);
	}

	// �^�C�g���V�[���ŃX�y�[�X�L�[�������ꂽ��T���v���V�[����
	if (CheckHitKey(KEY_INPUT_SPACE) == 1 && result_scene_type == SceneType::TITLE_SCENE)
	{
		ChangeScene(SceneType::SAMPLE_SCENE);
	}

	// �N���A������^�C�g����
	if (current_scene->GetIsCleared())
	{
		ChangeScene(SceneType::CLEAR_SCENE);
	}

	if (CheckHitKey(KEY_INPUT_RETURN) == 1 && result_scene_type == SceneType::CLEAR_SCENE)
	{
		savePosition = SavePosition::GetSavePosition();
		savePosition->InitialDeathCount();
		savePosition->SetCachePosition(Vector2D(100.f, 400.f));
		ChangeScene(SceneType::TITLE_SCENE);
	}

	/*
	// ���݂�Update����󂯎�����V�[�����ʂ̏ꍇ�A�V�[���̑J�ڂ��s��
	if (result_scene_type != current_scene->GetSceneType())
	{
		ChangeScene(result_scene_type);
	}*/

	// �`��
	Draw();

	
}

void SceneManager::Finalize()
{
	// CurrentScene�̉��
	if (current_scene != nullptr)
	{
		current_scene->Finalize();
		delete current_scene;
		current_scene = nullptr;
	}
}

void SceneManager::Draw()
{
	// ����ʂɕ`��
	SetDrawScreen(DX_SCREEN_BACK);

	// ��ʂ��N���A
	ClearDrawScreen();

	// �V�[���̕`��
	current_scene->Draw();

	// ��ʂ��X�V
	ScreenFlip();
}

void SceneManager::ChangeScene(SceneType new_scene_type)
{
	// �V�[���̐���
	SceneBase* new_scene = CreateScene(new_scene_type);
	assert(new_scene != nullptr);

	// ���݂̃V�[���̉��
	// �ŏ��̃V�[���������̂�CurrentScene��nullptr
	if (current_scene != nullptr)
	{
		current_scene->Finalize();
		delete current_scene;
	}

	// �V�����V�[���̊J�n
	
	new_scene->Initialize();
	
	current_scene = new_scene;
}

SceneBase* SceneManager::CreateScene(SceneType new_scene_type)
{
	switch (new_scene_type)
	{
	case SceneType::TITLE_SCENE:
		return new TitleScene();
	case SceneType::CLEAR_SCENE:
		return new ClearScene();
	case SceneType::SAMPLE_SCENE:
		return new SampleScene(); // SampleScene�N���X�ɃA�N�Z�X���ăN���X�̎擾
	default:
		return nullptr;
	}
}