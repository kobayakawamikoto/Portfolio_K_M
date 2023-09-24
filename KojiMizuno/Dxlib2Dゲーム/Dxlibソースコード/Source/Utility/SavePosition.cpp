#include "SavePosition.h"
#include "../GameObject/GameObject.h"

// staticïœêîÇÃíËã`
SavePosition* SavePosition::savePosition = nullptr;
Vector2D SavePosition::cache_position;

SavePosition::SavePosition()
{

}

SavePosition* SavePosition::GetSavePosition()
{
	return savePosition;
}

Vector2D SavePosition::GetCachePosition()
{
	return cache_position;
}

void SavePosition::SetCachePosition(Vector2D cache_vec)
{
	cache_position = cache_vec;
}