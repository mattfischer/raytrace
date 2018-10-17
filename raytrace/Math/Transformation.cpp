#include "Math/Transformation.hpp"
#include "Math/Vector.hpp"
#include "Math/Point.hpp"

#include <cmath>

#pragma warning( disable : 4355 )

using std::sin;
using std::cos;

namespace Math {
	Point BaseTransformation::origin() const
	{
		return Point(matrix().at(3,0), matrix().at(3,1), matrix().at(3,2));
	}

	InverseTransformation::InverseTransformation(const Transformation &transformation)
	: mTransformation(transformation)
	{
	}

	const BaseTransformation &InverseTransformation::inverse() const
	{
		return mTransformation;
	}

	const Matrix &InverseTransformation::matrix() const
	{
		return mTransformation.inverseMatrix();
	}

	const Matrix &InverseTransformation::inverseMatrix() const
	{
		return mTransformation.matrix();
	}

	const BaseTransformation &Transformation::inverse() const
	{
		return mInverse;
	}

	const Matrix &Transformation::matrix() const
	{
		return mMatrix;
	}

	const Matrix &Transformation::inverseMatrix() const
	{
		return mInverseMatrix;
	}

	Transformation Transformation::translate(const Vector &vector)
	{
		return translate(vector.x(), vector.y(), vector.z());
	}

	Transformation Transformation::translate(float x, float y, float z)
	{
		return Transformation(
					Matrix(
					  1, 0, 0, x,
					  0, 1, 0, y,
					  0, 0, 1, z,
					  0, 0, 0, 1), 
					Matrix(
					  1, 0, 0, -x,
					  0, 1, 0, -y,
					  0, 0, 1, -z,
					  0, 0, 0, 1));
	}

	Transformation Transformation::scale(const Vector &vector)
	{
		return scale(vector.x(), vector.y(), vector.z());
	}

	Transformation Transformation::scale(float x, float y, float z)
	{
		return Transformation(
					Matrix(
					  x, 0, 0, 0,
					  0, y, 0, 0,
					  0, 0, z, 0,
					  0, 0, 0, 1), 
					Matrix(
					  1/x, 0, 0, 0,
					  0, 1/y, 0, 0,
					  0, 0, 1/z, 0,
					  0, 0, 0, 1));
	}

	Transformation Transformation::uniformScale(float factor)
	{
		return scale(factor, factor, factor);
	}

	static float rad(float deg)
	{
		return deg * 3.14 / 180.0;
	}

	Transformation Transformation::rotate(const Vector &vector)
	{
		return rotate(vector.x(), vector.y(), vector.z());
	}

	Transformation Transformation::rotate(float x, float y, float z)
	{
		float rx = rad(x);
		float ry = rad(y);
		float rz = rad(z);

		Matrix xRotate(
					  1, 0, 0, 0,
					  0, cos(rx), -sin(rx), 0,
					  0, sin(rx), cos(rx), 0,
					  0, 0, 0, 1);

		Matrix xRotateInverse(
					  1, 0, 0, 0,
					  0, cos(rx), sin(rx), 0,
					  0, -sin(rx), cos(rx), 0,
					  0, 0, 0, 1);

		Matrix yRotate(
					  cos(ry), 0, -sin(ry), 0,
					  0, 1, 0, 0,
					  sin(ry), 0, cos(ry), 0,
					  0, 0, 0, 1);

		Matrix yRotateInverse(
					  cos(ry), 0, sin(ry), 0,
					  0, 1, 0, 0,
					  -sin(ry), 0, cos(ry), 0,
					  0, 0, 0, 1);

		Matrix zRotate(
					  cos(rz), sin(rz), 0, 0,
					  -sin(rz), cos(rz), 0, 0,
					  0, 0, 1, 0,
					  0, 0, 0, 1);

		Matrix zRotateInverse(
					  cos(rz), -sin(rz), 0, 0,
					  sin(rz), cos(rz), 0, 0,
					  0, 0, 1, 0,
					  0, 0, 0, 1);

		return Transformation(xRotate * yRotate * zRotate, zRotateInverse * yRotateInverse * xRotateInverse);
	}

	Transformation::Transformation()
	: mInverse(*this)
	{
	}

	Transformation::Transformation(const Matrix &matrix, const Matrix &inverseMatrix)
	: mMatrix(matrix), mInverseMatrix(inverseMatrix), mInverse(*this)
	{
	}

	Transformation::Transformation(const Transformation &c)
	: mMatrix(c.matrix()),
	  mInverseMatrix(c.inverseMatrix()),
	  mInverse(*this)
	{
	}

	Transformation &Transformation::operator=(const BaseTransformation &c)
	{
		mMatrix = c.matrix();
		mInverseMatrix = c.inverseMatrix();

		return *this;
	}

	Transformation &Transformation::operator=(const Transformation &c)
	{
		mMatrix = c.matrix();
		mInverseMatrix = c.inverseMatrix();

		return *this;
	}

	Transformation operator*(const BaseTransformation &a, const BaseTransformation &b)
	{
		return Transformation(b.matrix() * a.matrix(), a.inverseMatrix() * b.inverseMatrix());
	}
}