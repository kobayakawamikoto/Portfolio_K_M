#include "SavePosition.h"
#include "../GameObject/GameObject.h"

// staticïœêîÇÃíËã`
SavePosition* SavePosition::savePosition = nullptr;
Vector2D SavePosition::cache_position;
int SavePosition::DeathCount;

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

void SavePosition::SetDeathCount(int DeathCount_)
{
	DeathCount += DeathCount_;
}

void SavePosition::InitialDeathCount()
{
	DeathCount = 0;
}