#include "Gimmick_Save.h"
#include "../Ground/Ground.h"
#include "../Source/Utility/BoxCollisionParams.h"
#include "../SampleObject/SampleObject.h"
#include "../Source/Scene/SampleScene/SampleScene.h"
#include <algorithm> // std::max‚ğg—p‚·‚é‚½‚ß
#define NOMINMAX
#include "DxLib.h"

Gimmick_Save::Gimmick_Save() : isSaved(false)
{

}

Gimmick_Save::~Gimmick_Save()
{
	Finalize();
}

void Gimmick_Save::Initialize(Vector2D initialVec)
{
	__super::Initialize();

	sampleObject_ = SampleScene::GetSampleObject();
	bcp = new BoxCollisionParams();

	// ‰ŠúÀ•Wİ’è
	//Vector2D initialVec;
	//initialVec.x = 500.f;
	//initialVec.y = 100.f;
	SetPosition(initialVec);

	// ‰æ‘œ‚Ì“Ç‚İ‚İ
	loaded_save_handle = LoadGraph("Resources/Images/save.png");
	loaded_saved_handle = LoadGraph("Resources/Images/saved.png");
}

void Gimmick_Save::Update(float delta_seconds)
{
	__super::Update(delta_seconds);

	Vector2D input_dir;

	// ’eŠÛ‚ª“–‚½‚Á‚½‚ç
	for (int i = 0; i < 20; i++)
	{
		if (bcp->CheckHit(static_cast<int>(sampleObject_->GetBullet(i).x), static_cast<int>(sampleObject_->GetBullet(i).y), static_cast<int>(sampleObject_->GetBullet(i).x + 1), static_cast<int>(sampleObject_->GetBullet(i).y + 1),
			static_cast<int>(GetPosition().x - 14.f), static_cast<int>(GetPosition().y - 10.f), static_cast<int>(GetPosition().x - 14.f + 27), static_cast<int>(GetPosition().y - 10.f + 32)))
		{
			sampleObject_->SetBullet_x(i, 10000.f);
			sampleObject_->SetBullet_y(i, 10000.f);
			isSaved = true;
			saveCount = 0;
			sampleObject_->Save();
		}
	}
	if (isSaved && saveCount > 50.f)
	{
		isSaved =  false;
	}
	saveCount++;
}

void Gimmick_Save::Draw(const Vector2D& screen_offset)
{
	__super::Draw(screen_offset);

	// ‰æ‘œ‚Ì•`‰æ
	int x, y;
	GetPosition().ToInt(x, y);
	DrawGraph(static_cast<int>(GetPosition().x - 14.f - screen_offset.x), static_cast<int>(GetPosition().y - 10.f - screen_offset.y), loaded_save_handle, true);

	if (isSaved) // Â‚­“_“”
	{
		DrawGraph(static_cast<int>(GetPosition().x - 14.f - screen_offset.x), static_cast<int>(GetPosition().y - 10.f - screen_offset.y), loaded_saved_handle, true);
	}
}

void Gimmick_Save::Finalize()
{
	__super::Finalize();

	// ‰æ‘œ‚Ì”jŠü
	DeleteGraph(loaded_save_handle);
	DeleteGraph(loaded_saved_handle);
}