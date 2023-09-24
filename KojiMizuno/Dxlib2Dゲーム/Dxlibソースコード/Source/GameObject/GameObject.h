#pragma once

#include "../Utility/Vector2D.h"

/**
 * �Q�[�����ɕ\�������I�u�W�F�N�g�̊��N���X
 */
class GameObject
{
public:
	GameObject();
	virtual ~GameObject() {}

public:
	/**
	 * ������
	 */
	virtual void Initialize() {}
	virtual void Initialize(Vector2D initialVec, float add_x, float add_y, int direction) {}

	/**
	 * �X�V
	 * @param	delta_seconds	�O�t���[���Ƃ̍�������(s)
	 */
	virtual void Update(float delta_seconds) {}

	/**
	 * �`��
	 */
	virtual void Draw(const Vector2D& screen_offset) {}

	/**
	 * ���
	 */
	virtual void Finalize() {}

	/**
	 * �V�[���̎擾
	 * @return owner_scene
	 */
	class SceneBase* GetOwnerScene() const { return owner_scene; }

	/**
	 * �V�[���̃Z�b�g
	 * @param	new_owner_scene	�Z�b�g����Position
	 */
	void SetOwnerScene(class SceneBase* new_owner_scene);

	/**
	 * Position�̎擾
	 * @return	position
	 */
	Vector2D GetPosition() const { return position; }

	/**
	 * Position�̃Z�b�g
	 * @param	new_position	�Z�b�g����Position
	 */
	void SetPosition(const Vector2D& new_position);

	/**
	 * �`�揇�̎擾
	 * @return	draw_sort_priority
	 */
	int GetDrawSortPriority() const { return draw_sort_priority; }

	/**
	 * �`�揇�̃Z�b�g
	 * @param	new_priority	�Z�b�g����`�揇
	 */
	void SetDrawSortPriority(int new_priority);

protected:
	// �ʒu
	Vector2D position;

private:
	// �I�[�i�[�ƂȂ�V�[��
	class SceneBase* owner_scene;

	// �`�揇�B���l��������������`����s��
	int draw_sort_priority;
};