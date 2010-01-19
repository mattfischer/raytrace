#include "transformation.h"

#include <math.h>

Transformation Transformation::translate(double x, double y, double z)
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

Transformation Transformation::scale(double x, double y, double z)
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

Transformation Transformation::uniformScale(double factor)
{
	return scale(factor, factor, factor);
}

static double rad(double deg)
{
	return deg * 3.14 / 180.0;
}

Transformation Transformation::rotate(double x, double y, double z)
{
	double rx = rad(x);
	double ry = rad(y);
	double rz = rad(z);

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
				  cos(rz), sin(rz), 0, 0,
				  -sin(rz), cos(rz), 0, 0,
				  0, 0, 1, 0,
				  0, 0, 0, 1);

	return Transformation(xRotate * yRotate * zRotate, zRotateInverse * yRotateInverse * xRotateInverse);
}

Transformation::Transformation()
{
}

Transformation::Transformation(const Matrix &matrix, const Matrix &inverseMatrix)
: mMatrix(matrix), mInverseMatrix(inverseMatrix),
  mMatrixTranspose(matrix.transpose()), mInverseMatrixTranspose(inverseMatrix.transpose())
{
	mOrigin = mMatrix * Vector(0, 0, 0);
	mInverseOrigin = mInverseMatrix * Vector(0, 0, 0);
	mTransposeOrigin = mMatrixTranspose * Vector(0, 0, 0);
	mInverseTransposeOrigin = mInverseMatrixTranspose * Vector(0, 0, 0);
}

Transformation::Transformation(const Transformation &c)
: mMatrix(c.mMatrix), 
  mInverseMatrix(c.mInverseMatrix), 
  mMatrixTranspose(c.mMatrixTranspose),
  mInverseMatrixTranspose(c.mInverseMatrixTranspose),
  mOrigin(c.mOrigin), 
  mInverseOrigin(c.mInverseOrigin),
  mTransposeOrigin(c.mTransposeOrigin), 
  mInverseTransposeOrigin(c.mInverseTransposeOrigin)
{
}

Transformation &Transformation::operator=(const Transformation &c)
{
	mMatrix = c.mMatrix;
	mInverseMatrix = c.mInverseMatrix;
	mMatrixTranspose = c.mMatrixTranspose;
	mInverseMatrixTranspose = c.mInverseMatrixTranspose;

	mOrigin = c.mOrigin;
	mInverseOrigin = c.mInverseOrigin;
	mTransposeOrigin = c.mTransposeOrigin;
	mInverseTransposeOrigin = c.mInverseTransposeOrigin;

	return *this;
}


const Matrix &Transformation::matrix() const
{
	return mMatrix;
}

const Matrix &Transformation::inverseMatrix() const
{
	return mInverseMatrix;
}

const Matrix &Transformation::matrixTranspose() const
{
	return mMatrixTranspose;
}

const Matrix &Transformation::inverseMatrixTranspose() const
{
	return mInverseMatrixTranspose;
}

const Vector &Transformation::origin() const
{
	return mOrigin;
}

const Vector &Transformation::inverseOrigin() const
{
	return mInverseOrigin;
}

const Vector &Transformation::transposeOrigin() const
{
	return mTransposeOrigin;
}

const Vector &Transformation::inverseTransposeOrigin() const
{
	return mInverseTransposeOrigin;
}

Matrix Transformation::transformMatrix(const Matrix &matrix) const
{
	return mMatrix * matrix;
}

Vector Transformation::transformPoint(const Vector &point) const
{
	return mMatrix * point;
}

Vector Transformation::transformDirection(const Vector &direction) const
{
	return mMatrix.multiply(direction, false);
}

Vector Transformation::transformNormal(const Vector &normal) const
{
	return mInverseMatrixTranspose.multiply(normal, false).normalize();
}

Ray Transformation::transformRay(const Ray &ray) const
{
	return Ray(transformPoint(ray.origin()), transformDirection(ray.direction()));
}

Transformation Transformation::transformTransformation(const Transformation &transformation) const
{
	return Transformation(transformation.matrix() * mMatrix, mInverseMatrix * transformation.inverseMatrix());
}

Matrix Transformation::inverseTransformMatrix(const Matrix &matrix) const
{
	return mInverseMatrix * matrix;
}

Vector Transformation::inverseTransformPoint(const Vector &point) const
{
	return mInverseMatrix * point;
}

Vector Transformation::inverseTransformDirection(const Vector &direction) const
{
	return mInverseMatrix.multiply(direction, false);
}

Vector Transformation::inverseTransformNormal(const Vector &normal) const
{
	return mMatrixTranspose.multiply(normal, false).normalize();
}

Ray Transformation::inverseTransformRay(const Ray &ray) const
{
	return Ray(inverseTransformPoint(ray.origin()), inverseTransformDirection(ray.direction()));
}

Transformation Transformation::inverseTransformTransformation(const Transformation &transformation) const
{
	return Transformation(transformation.inverseMatrix() * mInverseMatrix, mMatrix * transformation.matrix());
}
