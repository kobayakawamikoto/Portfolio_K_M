#pragma once
#include "../Source/Utility/SavePosition.h"

// �V�[���^�C�v�̑O���錾
enum class SceneType : unsigned short;

/**
 * �e�V�[�����Ǘ�����N���X
 * �V�[���̐���/�j����ύX���s��
 */
class SceneManager
{
public:
	SceneManager();
	virtual ~SceneManager();

public:
	/**
	 * ������
	 */
	void Initialize();

	/**
	 * �X�V
	 * @param	delta_seconds	�O�t���[���Ƃ̍�������(s)
	 */
	void Update(float delta_seconds);

	/**
	 * ���
	 */
	void Finalize();

private:
	/**
	 * �`��
	 */
	void Draw();

	/**
	 * �V�[���̑J��
	 * @param	new_scene_type	�J�ڂ������V�[��
	 */
	void ChangeScene(SceneType new_scene_type);

	/**
	 * �V�[���̐���
	 * @param	new_scene_type	��������V�����V�[��
	 * @return	���������V�[��
	 */
	class SceneBase* CreateScene(SceneType new_scene_type);

private:
	// ���݂̃V�[��
	class SceneBase* current_scene;
	SavePosition* savePosition;
};