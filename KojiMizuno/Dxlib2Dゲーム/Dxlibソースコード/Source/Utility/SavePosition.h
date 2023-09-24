#pragma once

#include "../GameObject/GameObject.h"

/**
 * サンプルシーン
 * サンプル用に簡易的に実装
 */
class SavePosition
{
public:
	SavePosition();
	virtual ~SavePosition() {}


	static Vector2D cache_position;
	static SavePosition* savePosition;
	static SavePosition* GetSavePosition();
	static Vector2D GetCachePosition();
	static void SetCachePosition(Vector2D cache_vec);
};