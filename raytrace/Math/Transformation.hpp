#ifndef MATH_TRANSFORMATION_HPP
#define MATH_TRANSFORMATION_HPP

#include "Math/Forwards.hpp"
#include "Math/Matrix.hpp"

#include "Parse/AST.h"

namespace Math {

class BaseTransformation {
public:
	virtual const BaseTransformation &inverse() const = 0;

	virtual const Matrix &matrix() const = 0;
	virtual const Matrix &inverseMatrix() const = 0;

	Point origin() const;
};

class Transformation;
class InverseTransformation : public BaseTransformation
{
public:
	InverseTransformation(const Transformation &transformation);
	const BaseTransformation &inverse() const;

	const Matrix &matrix() const;
	const Matrix &inverseMatrix() const;

private:
	const Transformation &mTransformation;
};

class Transformation : public BaseTransformation
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
	explicit Transformation(const BaseTransformation &c);
	Transformation &operator=(const BaseTransformation &c);

	const BaseTransformation &inverse() const;

	const Matrix &matrix() const;
	const Matrix &inverseMatrix() const;

protected:
	Matrix mMatrix;
	Matrix mInverseMatrix;
	InverseTransformation mInverse;
};

Transformation operator*(const BaseTransformation &a, const BaseTransformation &b);

}

#endif