#include "Ground.h"

#include "DxLib.h"

Ground::Ground():
	chip(), loaded_sprite_handle(0)
{
}

Ground::~Ground()
{
	Finalize();
}

void Ground::Initialize()
{
	__super::Initialize();

	// �摜�̓ǂݍ���
	loaded_sprite_handle = DeleteGraph(loaded_sprite_handle);LoadDivGraph(_T("Resources/Images/mapchip.bmp"), 19, 4, 5, 32, 40, chip);
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

	// �w�i�摜�`��
	DrawExtendGraph(0, 0, 640, 480, loaded_bg_handle, true);

	// �}�b�v�摜�̕`��
	int x, y;
	GetPosition().ToInt(x, y);
	for (int j = 0; j < NUM_MAP_Y; j++) // �s
	{
		for (int i = 0; i < NUM_MAP_X; i++) // ��
		{
			int no = data[i + j * NUM_MAP_X];
			DrawGraph(i * SIZE_MAP_X - static_cast<int>(screen_offset.x) - MAP_OFFSET_X, j * SIZE_MAP_Y - static_cast<int>(screen_offset.y), chip[no], true);
		}
	}
}

void Ground::Finalize()
{
	__super::Finalize();

	// �摜�̔j��
	DeleteGraph(loaded_sprite_handle);
	loaded_sprite_handle = 0;
	DeleteGraph(loaded_bg_handle);
	loaded_bg_handle = 0;
}

