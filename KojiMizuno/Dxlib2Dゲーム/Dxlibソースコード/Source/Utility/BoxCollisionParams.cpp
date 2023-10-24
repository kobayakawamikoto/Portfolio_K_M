#include "BoxCollisionParams.h"
#include "../GameObject/Enemy/Enemy_Purple.h"
#include "../GameObject/Ground/Ground.h"
#include "../GameObject/GameObject.h"


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
int BoxCollisionParams::CheckHit(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
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
