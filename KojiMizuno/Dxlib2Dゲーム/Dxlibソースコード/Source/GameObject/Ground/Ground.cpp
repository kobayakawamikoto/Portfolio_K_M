#include "Ground.h"

#include "DxLib.h"

Ground::Ground() :
	chip(), loaded_map_handle(0)
{
}

Ground::~Ground()
{
	Finalize();
}

void Ground::Initialize()
{
	__super::Initialize();

	// ‰æ‘œ‚Ì“Ç‚İ‚İ
	loaded_map_handle = DeleteGraph(loaded_map_handle); 
	LoadDivGraph(_T("Resources/Images/mapchip.bmp"), 19, 4, 5, 32, 40, chip);
	loaded_bg_handle = LoadGraph("Resources/Images/bg.jpg");
	GraphFilter(loaded_bg_handle, DX_GRAPH_FILTER_MONO, -60, 7);
}

void Ground::Update(float delta_seconds)
{
	__super::Update(delta_seconds);

}

void Ground::Draw(const Vector2D& screen_offset)
{
	__super::Draw(screen_offset);

	// ”wŒi‰æ‘œ•`‰æ
	DrawExtendGraph(0, 0, 640, 480, loaded_bg_handle, true);

	// ƒ}ƒbƒv‰æ‘œ‚Ì•`‰æ
	int x, y;
	GetPosition().ToInt(x, y);
	for (int j = 0; j < NUM_MAP_Y; j++) // s”•ª‰ñ‚·
	{
		for (int i = 0; i < NUM_MAP_X; i++) // —ñ”•ª‰ñ‚·
		{
			int no = data[i + j * NUM_MAP_X];
			DrawGraph(i * SIZE_MAP_X - static_cast<int>(screen_offset.x) - MAP_OFFSET_X, j * SIZE_MAP_Y - static_cast<int>(screen_offset.y) - MAP_OFFSET_Y, chip[no], true);
		}
	}
}

void Ground::Finalize()
{
	__super::Finalize();

	// ‰æ‘œ‚Ì”jŠü
	DeleteGraph(loaded_map_handle);
	loaded_map_handle = 0;
	DeleteGraph(loaded_bg_handle);
	loaded_bg_handle = 0;
}

