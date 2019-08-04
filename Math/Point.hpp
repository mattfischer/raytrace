#ifndef MATH_POINT_HPP
#define MATH_POINT_HPP

namespace Math {
	class Vector;
	class BaseTransformation;
	class Matrix;
	class Point
	{
	public:
		Point();
		Point(float x, float y, float z);
		explicit Point(const Vector &vector);

		float x() const;
		float y() const;
		float z() const;

		Point operator+(const Vector &b) const;
		Point operator-(const Vector &b) const;
		Vector operator-(const Point &b) const;

	private:
		float mX;
		float mY;
		float mZ;
	};

	Point operator*(const BaseTransformation &transformation, const Point &point);
	Point operator*(const Matrix &matrix, const Point &point);
}

#endif
