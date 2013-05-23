#ifndef TRANSFORMATION_H
#define TRANSFORMATION_H

#include "Math/Matrix.hpp"
#include "Math/Point.hpp"
#include "Parse/AST.h"

namespace Math {

class Transformation
{
public:
	static Transformation translate(float x, float y, float z);
	static Transformation translate(const Vector &vector);

	static Transformation scale(float x, float y, float z);
	static Transformation scale(const Vector &vector);

	static Transformation uniformScale(float factor);

	static Transformation rotate(float x, float y, float z);
	static Transformation rotate(const Vector &vector);

	static Transformation fromAst(AST *ast);

	Transformation();
	Transformation(const Matrix &matrix, const Matrix &inverseMatrix);
	Transformation(const Transformation &c);
	Transformation &operator=(const Transformation &c);

	Transformation inverse() const;

	const Matrix &matrix() const;
	const Matrix &inverseMatrix() const;

	Point origin() const;

protected:
	Matrix mMatrix;
	Matrix mInverseMatrix;
};

Transformation operator*(const Transformation &a, const Transformation &b);

inline Transformation::Transformation(const Transformation &c)
: mMatrix(c.mMatrix), 
  mInverseMatrix(c.mInverseMatrix)
{
}

inline Transformation &Transformation::operator=(const Transformation &c)
{
	mMatrix = c.mMatrix;
	mInverseMatrix = c.mInverseMatrix;

	return *this;
}

inline Transformation Transformation::inverse() const
{
	return Transformation(mInverseMatrix, mMatrix);
}

inline const Matrix &Transformation::matrix() const
{
	return mMatrix;
}

inline const Matrix &Transformation::inverseMatrix() const
{
	return mInverseMatrix;
}

}

#endif