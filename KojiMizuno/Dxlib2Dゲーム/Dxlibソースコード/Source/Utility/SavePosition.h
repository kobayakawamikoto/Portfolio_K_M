#pragma once

#include "../GameObject/GameObject.h"

/**
 * �T���v���V�[��
 * �T���v���p�ɊȈՓI�Ɏ���
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