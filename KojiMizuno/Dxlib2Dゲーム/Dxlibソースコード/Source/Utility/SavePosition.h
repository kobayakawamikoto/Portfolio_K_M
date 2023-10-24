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
	static int DeathCount;

	static SavePosition* GetSavePosition();
	static Vector2D GetCachePosition();
	static void SetCachePosition(Vector2D cache_vec);
	static int GetDeathCount() { return DeathCount; }
	static void SetDeathCount(int DeathCount_);
	static void InitialDeathCount();
};