#pragma once

/**
 * 2Dベクトルクラス
 */
struct Vector2D
{
public:
	Vector2D()
		: x(0.0f), y(0.0f)
	{}

	Vector2D(float in_x, float in_y)
		: x(in_x), y(in_y)
	{}

public:
	const Vector2D operator +(const Vector2D& in_vector) const;
	Vector2D& operator +=(const Vector2D& in_vector);
	const Vector2D operator -(const Vector2D& in_vector) const;
	Vector2D& operator -=(const Vector2D& in_vector);
	const Vector2D operator *(float in_scalar) const;
	Vector2D& operator *=(float in_scalar);
	const Vector2D operator /(float in_scalar) const;
	Vector2D& operator /=(float in_scalars);

public:
	static float Dot(const Vector2D& a, const Vector2D& b);
	static float Cross(const Vector2D& a, const Vector2D& b);

public:
	float Length() const;
	Vector2D Normalize() const;

public:
	/**
	 * Intに変換する
	 * 小数点以下は切り捨て
	 * @return 変換したx,y
	 */
	void ToInt(int& out_x, int& out_y) const;

public:
	float x, y;
};