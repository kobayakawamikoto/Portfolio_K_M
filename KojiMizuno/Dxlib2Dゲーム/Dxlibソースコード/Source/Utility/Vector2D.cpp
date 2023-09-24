#include "Vector2D.h"

#include <cmath>
#include <cfenv>

const Vector2D Vector2D::operator +(const Vector2D& in_vector) const
{
	return Vector2D(x + in_vector.x, y + in_vector.y);
}

Vector2D& Vector2D::operator +=(const Vector2D& in_vector)
{
	x += in_vector.x;
	y += in_vector.y;
	return *this;
}

const Vector2D Vector2D::operator -(const Vector2D& in_vector) const
{
	return Vector2D(x - in_vector.x, y - in_vector.y);
}

Vector2D& Vector2D::operator -=(const Vector2D& in_vector)
{
	x -= in_vector.x;
	y -= in_vector.y;
	return *this;
}

const Vector2D Vector2D::operator *(float in_scalar) const
{
	return Vector2D(x * in_scalar, y * in_scalar);
}

Vector2D& Vector2D::operator *=(float in_scalar)
{
	x *= in_scalar;
	y *= in_scalar;
	return *this;
}

const Vector2D Vector2D::operator /(float in_scalar) const
{
	// 1e-5(0.00001)ÇÊÇËè¨Ç≥Ç¢èÍçáÇÕ0Ç∆Ç›Ç»Ç∑
	if (fabsf(in_scalar) < 1e-5f)
	{
		// É[ÉçèúéZÇÕåãâ Ç0Ç…Ç∑ÇÈ
		return Vector2D(0.0f, 0.0f);
	}

	return Vector2D(x / in_scalar, y / in_scalar);
}

Vector2D& Vector2D::operator /=(float in_scalar)
{
	// 1e-5(0.00001)ÇÊÇËè¨Ç≥Ç¢èÍçáÇÕ0Ç∆Ç›Ç»Ç∑
	if (fabsf(in_scalar) < 1e-5f)
	{
		// É[ÉçèúéZÇÕåãâ Ç0Ç…Ç∑ÇÈ
		x = 0.0f;
		y = 0.0f;
	}
	else
	{
		x /= in_scalar;
		y /= in_scalar;
	}

	return *this;
}

float Vector2D::Dot(const Vector2D& A, const Vector2D& B)
{
	return (A.x * B.x) + (A.y * B.y);
}

float Vector2D::Cross(const Vector2D& A, const Vector2D& B)
{
	return (A.x * B.y) - (A.y * B.x);
}

float Vector2D::Length() const
{
	return sqrtf(powf(x, 2.0f) + powf(y, 2.0f));
}

Vector2D Vector2D::Normalize() const
{
	return *this / Length();
}

void Vector2D::ToInt(int& out_x, int& out_y) const
{
	out_x = (int)std::round(x);
	out_y = (int)std::round(y);
}