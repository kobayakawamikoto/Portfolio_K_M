#include "Collision.h"
#include "../Ground/Ground.h"
#include "../Source/Utility/BoxCollisionParams.h"
#include "../SampleObject/SampleObject.h"
#include "../Source/Scene/SampleScene/SampleScene.h"
#include "../Source/Utility/PlayerInfo.h"
#include <algorithm> // std::max���g�p���邽��
#include <random>
#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#define NOMINMAX
#include "DxLib.h"

Collision::Collision() :
	chip(), loaded_sprite_handle(0)
{
}

Collision::~Collision()
{
	Finalize();
}

void Collision::Initialize()
{
	__super::Initialize();

	ground = new Ground();
}

void Collision::Update(float delta_seconds)
{
	__super::Update(delta_seconds);

}

void Collision::Draw(const Vector2D& screen_offset)
{
	__super::Draw(screen_offset);

}

void Collision::Finalize()
{
	__super::Finalize();
}

//
// �����蔻����s���֐�
// 
// �΂̃L���� ��   x1
// �@�@�@�@�@ ��   y1
// �@�@�@�@�@ ���� w1
// �@�@�@�@�@ �c�� h1
// ���̃L���� ��   x2
// �@�@�@�@�@ ��   y2
// �@�@�@�@�@ ���� w2
// �@�@�@�@�@ �c�� h2
//
int Collision::CheckHit(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
	int L1 = x1;      // ��
	int R1 = w1; // �E(��+����)
	int L2 = x2;      // ��
	int R2 = w2; // �E(��+����)

	if (R1 <= L2) return 0;
	if (R2 <= L1) return 0;

	int U1 = y1;      // ��
	int D1 = h1; // ��(��+�c��)
	int U2 = y2;      // ��
	int D2 = h2; // ��(��+�c��)

	if (D1 <= U2) return 0;
	if (D2 <= U1) return 0;

	// ����ȊO�̏ꍇ�͓������Ă���
	return 1;
}

bool Collision::DetectMapchip(int chara_x, int chara_y, int chara_width, int chara_height, int prev_x, int prev_y)
{
	/*
	for (int j = 0; j < 12; j++) // �s��
	{
		for (int i = 0; i < ground->GetNUM_MAP_X(); i++) { // ��
			int no = ground->data[i + j * ground->GetNUM_MAP_X()];
			if (no != 7 && no != 11 && no != 15 && no != 19)
			{
				// �����蔻��̐���
				if (CheckHit(i * 32 - ground->GetMAP_OFFSET_X(), j * 40, i * 32 + 32 - ground->GetMAP_OFFSET_X(), j * 40 + 40,
					static_cast<int>(GetPosition().x - 14.f), static_cast<int>(GetPosition().y + 2.f), static_cast<int>(GetPosition().x - 14.f + 27), static_cast<int>(GetPosition().y + 2.f + 38)))
				{
					//// ���E���瓖�����Ă���
					//if (GetPosition().x != prev_x)
					//{
					//	Vector2D vec_ = GetPosition();
					//	vec_.x = std::max(GetPosition().x, prev_x);
					//	vec_.x /= 32; // SIZE_MAP_X
					//	vec_.x *= 32;
					//	SetPosition(vec_);
					//}
					//// �㉺���瓖�����Ă���
					//if (GetPosition().y != prev_y)
					//{
					//	Vector2D vec_ = GetPosition();
					//	vec_.y = std::max(GetPosition().y, prev_y);
					//	vec_.y /= 40; // SIZE_MAP_Y
					//	vec_.y *= 40;
					//	SetPosition(vec_);
					//}
					// �O��̈ʒu�ɖ߂�

					// �ォ�瓖�����Ă��邩�`�F�b�N
					if (chara_y >= prev_y)
					{
						// ��ɃR���W�����̂���}�b�v�`�b�v���Ȃ��Ƃ���������
						if (ground->data[i + j * ground->GetNUM_MAP_X()] != ground->data[i + (j - 1) * ground->GetNUM_MAP_X()])
						{
							// �n�`�����ォ�瓖�����Ă��邩�`�F�b�N����
							// ���̏ꍇ�̂�Y��␳
							// �����łȂ��ꍇ�͉����瓖�����Ă���
							if (j * 40 - 38 >= prev_y)
							{
								// �C�n�`�ɏォ�瓖�����Ă���
								Vector2D vec;
								vec.x = prev_x;
								prev_y = static_cast<float>(j * 40 - 38);
								vec.y = prev_y;
								if (chara_y != prev_y)
								{
									SetPosition(vec);
								}
								if (!isDead)
								{
									yadd.y = 0.0f;
								}
								canJump = true;
								isJumping = false;
								isJumpKeyLeave = false;
								isJumpOnce = false;
								isGround = true;
								jumpTime = 0;
								jump.y = 0;
							}
							else
							{
								// �D�n�`�̉����瓖�����Ă���
								// �}�X�N���邽��int���g�p���܂� (float���ƁA�����Ċ|����Ɠ����l�ɂȂ��Ă��܂�)
								Vector2D vec;
								vec.y = chara_y;
								int _x = 0;

								if (playerDirection == PlayerDirection::RIGHT)
								{
									_x = static_cast<int>(std::min(chara_x, prev_x));
								}
								else
								{
									_x = static_cast<int>(std::max(chara_x, prev_x));
								}

								_x /= 5;
								_x *= 5;
								vec.x = static_cast<float>(_x);
								if (chara_x != _x) // (GetPosition().x == _x)�̏ꍇ�������ɂ��蔲���鏰�ɂȂ�
								{
									SetPosition(vec);
								}
							}
						}
						else
						{
							// �D�n�`�̉����瓖�����Ă���
							// �}�X�N���邽��int���g�p���܂� (float���ƁA�����Ċ|����Ɠ����l�ɂȂ��Ă��܂�)
							Vector2D vec;
							vec.y = chara_y;
							int _x = 0;

							if (playerDirection == PlayerDirection::RIGHT)
							{
								_x = static_cast<int>(std::min(chara_x, prev_x));
							}
							else
							{
								_x = static_cast<int>(std::max(chara_x, prev_x));
							}

							_x /= 5;
							_x *= 5;
							vec.x = static_cast<float>(_x);
							if (chara_x != _x) // (GetPosition().x == _x)�̏ꍇ�������ɂ��蔲���鏰�ɂȂ�
							{
								SetPosition(vec);
							}
						}
					}
					// �����瓖�����Ă��邩�`�F�b�N
					else if (chara_y < prev_y)
					{
						// ���ɃR���W�����̂���}�b�v�`�b�v���Ȃ��Ƃ���������
						if (ground->data[i + j * ground->GetNUM_MAP_X()] != ground->data[i + (j + 1) * ground->GetNUM_MAP_X()])
						{
							// �n�`���������瓖�����Ă��邩�`�F�b�N����
							// ���̏ꍇ�̂�Y��␳
							// �����łȂ��ꍇ�͉����瓖�����Ă���
							if ((j + 1) * 40.f <= prev_y + 2)
							{
								Vector2D vec;
								vec.x = chara_x;
								prev_y = (j + 1) * 40.f;
								vec.y = prev_y;
								if (chara_y != prev_y)
								{
									SetPosition(vec);
								}
								if (!isDead)
								{
									yadd.y = 0.0f;
								}
							}
							else
							{
								// �n�`�̉����瓖�����Ă���
								// �}�X�N���邽��int���g�p���܂� (float���ƁA�����Ċ|����Ɠ����l�ɂȂ��Ă��܂�)
								Vector2D vec;
								vec.y = chara_y;
								int _x = 0;

								if (playerDirection == PlayerDirection::RIGHT)
								{
									_x = static_cast<int>(std::min(chara_x, prev_x));
								}
								else
								{
									_x = static_cast<int>(std::max(chara_x, prev_x));
								}

								_x /= 5;
								_x *= 5;
								vec.x = static_cast<float>(_x);
								if (chara_x != _x) // (GetPosition().x == _x)�̏ꍇ�������ɂ��蔲���鏰�ɂȂ�
								{
									SetPosition(vec);
								}
							}
						}
						else
						{
							// �n�`�̉����瓖�����Ă���
							// �}�X�N���邽��int���g�p���܂� (float���ƁA�����Ċ|����Ɠ����l�ɂȂ��Ă��܂�)
							Vector2D vec;
							vec.y = chara_y;
							int _x = 0;

							if (playerDirection == PlayerDirection::RIGHT)
							{
								_x = static_cast<int>(std::min(chara_x, prev_x));
							}
							else
							{
								_x = static_cast<int>(std::max(chara_x, prev_x));
							}

							_x /= 5;
							_x *= 5;
							vec.x = static_cast<float>(_x);
							if (chara_x != _x) // (GetPosition().x == _x)�̏ꍇ�������ɂ��蔲���鏰�ɂȂ�
							{
								SetPosition(vec);
							}
						}
					}
				}
			}
		}
	}*/
	return false;
}


