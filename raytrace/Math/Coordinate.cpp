#include "Math/Coordinate.hpp"

namespace Math {
	Coordinate::Coordinate()
	{
		mValues[0] = mValues[1] = mValues[2] = mValues[3] = 0;
	}

	Coordinate::Coordinate(float x, float y, float z, float w)
	{
		mValues[0] = x;
		mValues[1] = y;
		mValues[2] = z;
		mValues[3] = w;
	}

	Coordinate::Coordinate(const Coordinate &c)
	{
		for(int i=0; i<4; i++)
			mValues[i] = c(i);
	}

	Coordinate &Coordinate::operator=(const Coordinate &c)
	{
		for(int i=0; i<4; i++) 
			mValues[i] = c(i);

		return *this;
	}

	float Coordinate::x() const
	{
		return mValues[0];
	}

	void Coordinate::setX(float x)
	{
		mValues[0] = x;
	}

	float Coordinate::y() const
	{
		return mValues[1];
	}

	void Coordinate::setY(float y)
	{
		mValues[1] = y;
	}

	float Coordinate::z() const
	{
		return mValues[2];
	}

	void Coordinate::setZ(float z)
	{
		mValues[2] = z;
	}

	float Coordinate::w() const
	{
		return mValues[3];
	}

	void Coordinate::setW(float z)
	{
		mValues[3] = z;
	}

	float &Coordinate::at(int i)
	{
		return mValues[i];
	}

	const float &Coordinate::at(int i) const
	{
		return mValues[i];
	}

	float &Coordinate::operator()(int i)
	{
		return at(i);
	}

	const float &Coordinate::operator()(int i) const
	{
		return at(i);
	}
}