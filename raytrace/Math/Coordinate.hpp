#ifndef COORDINATE_H
#define COORDINATE_H

#include "Parse/AST.h"

namespace Math {

class Coordinate
{
public:
	Coordinate();
	Coordinate(const Coordinate &c);
	Coordinate(float x, float y, float z, float w);
	explicit Coordinate(const ASTVector &astVector, float w);
	
	Coordinate &operator=(const Coordinate &c);

	float x() const;
	void setX(float x);

	float y() const;
	void setY(float y);

	float z() const;
	void setZ(float z);

	float w() const;
	void setW(float w);

	float &at(int i);
	const float &at(int i) const;

	float &operator()(int i);
	const float &operator()(int i) const;

protected:
	float mValues[4];
};

inline Coordinate::Coordinate()
{
	mValues[0] = mValues[1] = mValues[2] = mValues[3] = 0;
}

inline Coordinate::Coordinate(float x, float y, float z, float w)
{
	mValues[0] = x;
	mValues[1] = y;
	mValues[2] = z;
	mValues[3] = w;
}

inline Coordinate::Coordinate(const ASTVector &astVector, float w)
{
	mValues[0] = astVector.x;
	mValues[1] = astVector.y;
	mValues[2] = astVector.z;
	mValues[3] = w;
}

inline Coordinate::Coordinate(const Coordinate &c)
{
	for(int i=0; i<4; i++) 
		mValues[i] = c(i);
}

inline Coordinate &Coordinate::operator=(const Coordinate &c)
{
	for(int i=0; i<4; i++) 
		mValues[i] = c(i);

	return *this;
}

inline float Coordinate::x() const
{
	return mValues[0];
}

inline void Coordinate::setX(float x)
{
	mValues[0] = x;
}

inline float Coordinate::y() const
{
	return mValues[1];
}

inline void Coordinate::setY(float y)
{
	mValues[1] = y;
}

inline float Coordinate::z() const
{
	return mValues[2];
}

inline void Coordinate::setZ(float z)
{
	mValues[2] = z;
}

inline float Coordinate::w() const
{
	return mValues[3];
}

inline void Coordinate::setW(float z)
{
	mValues[3] = z;
}

inline float &Coordinate::at(int i)
{
	return mValues[i];
}

inline const float &Coordinate::at(int i) const
{
	return mValues[i];
}

inline float &Coordinate::operator()(int i)
{
	return at(i);
}

inline const float &Coordinate::operator()(int i) const
{
	return at(i);
}

}

#endif