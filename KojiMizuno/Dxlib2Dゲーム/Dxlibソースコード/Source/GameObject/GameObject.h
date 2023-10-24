#pragma once

#include "../Utility/Vector2D.h"

/**
 * ゲーム内に表示されるオブジェクトの基底クラス
 */
class GameObject
{
public:
	GameObject();
	virtual ~GameObject() {}

public:
	/**
	 * 初期化
	 */
	virtual void Initialize() {}
	virtual void Initialize(Vector2D initialVec, float add_x, float add_y, int direction) {}
	virtual void Initialize(Vector2D initialVec) {}

	/**
	 * 更新
	 * @param	delta_seconds	前フレームとの差分時間(s)
	 */
	virtual void Update(float delta_seconds) {}

	/**
	 * 描画
	 */
	virtual void Draw(const Vector2D& screen_offset) {}

	/**
	 * 解放
	 */
	virtual void Finalize() {}

	/**
	 * シーンの取得
	 * @return owner_scene
	 */
	class SceneBase* GetOwnerScene() const { return owner_scene; }

	/**
	 * シーンのセット
	 * @param	new_owner_scene	セットするPosition
	 */
	void SetOwnerScene(class SceneBase* new_owner_scene);

	/**
	 * Positionの取得
	 * @return	position
	 */
	Vector2D GetPosition() const { return position; }

	/**
	 * Positionのセット
	 * @param	new_position	セットするPosition
	 */
	void SetPosition(const Vector2D& new_position);

	/**
	 * 描画順の取得
	 * @return	draw_sort_priority
	 */
	int GetDrawSortPriority() const { return draw_sort_priority; }

	/**
	 * 描画順のセット
	 * @param	new_priority	セットする描画順
	 */
	void SetDrawSortPriority(int new_priority);

protected:
	// 位置
	Vector2D position;

private:
	// オーナーとなるシーン
	class SceneBase* owner_scene;

	// 描画順。数値が小さい順から描画を行う
	int draw_sort_priority;
};