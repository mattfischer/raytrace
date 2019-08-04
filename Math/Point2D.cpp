#include "Math/Point2D.hpp"

namespace Math {
	Point2D::Point2D()
	{
		mU = mV = 0;
	}

	Point2D::Point2D(float u, float v)
	{
		mU = u;
		mV = v;
	}

	float Point2D::u() const
	{
		return mU;
	}

	float Point2D::v() const
	{
		return mV;
	}

	Point2D Point2D::operator*(float b) const
	{
		return Point2D(mU * b, mV * b);
	}

	Point2D Point2D::operator+(const Point2D &b) const
	{
		return Point2D(mU + b.u(), mV + b.v());
	}
}