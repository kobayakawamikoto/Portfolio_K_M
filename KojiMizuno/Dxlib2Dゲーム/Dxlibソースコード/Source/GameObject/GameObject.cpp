#include "GameObject.h"
#include "../Scene/SceneBase.h"

GameObject::GameObject()
	: owner_scene(nullptr)
	, position(Vector2D())
	, draw_sort_priority(0)
{
}

void GameObject::SetOwnerScene(SceneBase* new_owner_scene)
{
	owner_scene = new_owner_scene;
}

void GameObject::SetPosition(const Vector2D& new_position)
{
	position = new_position;
}

void GameObject::SetDrawSortPriority(int new_priority)
{
	draw_sort_priority = new_priority;
}