#ifndef TRANSFORMATION_H
#define TRANSFORMATION_H

#include "matrix.h"
#include "vector.h"
#include "ray.h"
#include "ast.h"

class Transformation
{
public:
	static Transformation translate(double x, double y, double z);
	static Transformation translate(const Vector &vector);

	static Transformation scale(double x, double y, double z);
	static Transformation scale(const Vector &vector);

	static Transformation uniformScale(double factor);

	static Transformation rotate(double x, double y, double z);
	static Transformation rotate(const Vector &vector);

	static Transformation fromAst(AST *ast);

	Transformation();
	Transformation(const Matrix &matrix, const Matrix &inverseMatrix);
	Transformation(const Transformation &c);
	Transformation &operator=(const Transformation &c);

	const Matrix &matrix() const;
	const Matrix &inverseMatrix() const;
	const Matrix &matrixTranspose() const;
	const Matrix &inverseMatrixTranspose() const;

	const Vector &origin() const;
	const Vector &inverseOrigin() const;
	const Vector &transposeOrigin() const;
	const Vector &inverseTransposeOrigin() const;

	Matrix transformMatrix(const Matrix &matrix) const;
	Vector transformPoint(const Vector &point) const;
	Vector transformDirection(const Vector &direction) const;
	Vector transformNormal(const Vector &normal) const;
	Ray    transformRay(const Ray &ray) const;
	Transformation transformTransformation(const Transformation &transformation) const;

	Matrix inverseTransformMatrix(const Matrix &matrix) const;
	Vector inverseTransformPoint(const Vector &point) const;
	Vector inverseTransformDirection(const Vector &direction) const;
	Vector inverseTransformNormal(const Vector &normal) const;
	Ray    inverseTransformRay(const Ray &ray) const;
	Transformation inverseTransformTransformation(const Transformation &transformation) const;

protected:
	Matrix mMatrix;
	Matrix mInverseMatrix;
	Matrix mMatrixTranspose;
	Matrix mInverseMatrixTranspose;

	Vector mOrigin;
	Vector mInverseOrigin;
	Vector mTransposeOrigin;
	Vector mInverseTransposeOrigin;
};

#endif