#ifndef COORDINATE_H
#define COORDINATE_H

#include "Parse/AST.h"

namespace Math {

class Coordinate
{
public:
	Coordinate();
	Coordinate(const Coordinate &c);
	Coordinate(double x, double y, double z, double w);
	explicit Coordinate(const ASTVector &astVector, double w);
	
	Coordinate &operator=(const Coordinate &c);

	double x() const;
	void setX(double x);

	double y() const;
	void setY(double y);

	double z() const;
	void setZ(double z);

	double w() const;
	void setW(double w);

	double &at(int i);
	const double &at(int i) const;

	double &operator()(int i);
	const double &operator()(int i) const;

protected:
	double mValues[4];
};

inline Coordinate::Coordinate()
{
	mValues[0] = mValues[1] = mValues[2] = mValues[3] = 0;
}

inline Coordinate::Coordinate(double x, double y, double z, double w)
{
	mValues[0] = x;
	mValues[1] = y;
	mValues[2] = z;
	mValues[3] = w;
}

inline Coordinate::Coordinate(const ASTVector &astVector, double w)
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

inline double Coordinate::w() const
{
	return mValues[3];
}

inline void Coordinate::setW(double z)
{
	mValues[3] = z;
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