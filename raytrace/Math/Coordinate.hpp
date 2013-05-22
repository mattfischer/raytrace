#ifndef COORDINATE_H
#define COORDINATE_H

#include "Parse/AST.h"

namespace Math {

class Coordinate
{
public:
	Coordinate();
	Coordinate(const Coordinate &c);
	Coordinate(double x, double y, double z);
	explicit Coordinate(const ASTVector &astVector);
	
	Coordinate &operator=(const Coordinate &c);

	double x() const;
	void setX(double x);

	double y() const;
	void setY(double y);

	double z() const;
	void setZ(double z);

	double &at(int i);
	const double &at(int i) const;

	double &operator()(int i);
	const double &operator()(int i) const;

protected:
	double mValues[3];
};

inline Coordinate::Coordinate()
{
	mValues[0] = mValues[1] = mValues[2] = 0;
}

inline Coordinate::Coordinate(double x, double y, double z)
{
	mValues[0] = x;
	mValues[1] = y;
	mValues[2] = z;
}

inline Coordinate::Coordinate(const ASTVector &astVector)
{
	mValues[0] = astVector.x;
	mValues[1] = astVector.y;
	mValues[2] = astVector.z;
}

inline Coordinate::Coordinate(const Coordinate &c)
{
	for(int i=0; i<3; i++) 
		mValues[i] = c(i);
}

inline Coordinate &Coordinate::operator=(const Coordinate &c)
{
	for(int i=0; i<3; i++) 
		mValues[i] = c(i);

	return *this;
}

inline double Coordinate::x() const
{
	return mValues[0];
}

inline void Coordinate::setX(double x)
{
	mValues[0] = x;
}

inline double Coordinate::y() const
{
	return mValues[1];
}

inline void Coordinate::setY(double y)
{
	mValues[1] = y;
}

inline double Coordinate::z() const
{
	return mValues[2];
}

inline void Coordinate::setZ(double z)
{
	mValues[2] = z;
}

inline double &Coordinate::at(int i)
{
	return mValues[i];
}

inline const double &Coordinate::at(int i) const
{
	return mValues[i];
}

inline double &Coordinate::operator()(int i)
{
	return at(i);
}

inline const double &Coordinate::operator()(int i) const
{
	return at(i);
}

}

#endif