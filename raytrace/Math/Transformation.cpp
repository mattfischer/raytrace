#include "Math/Transformation.hpp"

#include <math.h>

namespace Math {

Transformation Transformation::translate(const Vector &vector)
{
	return translate(vector.x(), vector.y(), vector.z());
}

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

Transformation Transformation::scale(const Vector &vector)
{
	return scale(vector.x(), vector.y(), vector.z());
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

Transformation Transformation::rotate(const Vector &vector)
{
	return rotate(vector.x(), vector.y(), vector.z());
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
				  cos(rz), -sin(rz), 0, 0,
				  sin(rz), cos(rz), 0, 0,
				  0, 0, 1, 0,
				  0, 0, 0, 1);

	return Transformation(xRotate * yRotate * zRotate, zRotateInverse * yRotateInverse * xRotateInverse);
}

Transformation::Transformation()
{
}

Transformation::Transformation(const Matrix &matrix, const Matrix &inverseMatrix)
: mMatrix(matrix), mInverseMatrix(inverseMatrix)
{
}

Transformation Transformation::fromAst(AST *ast)
{
	Transformation t;

	for(int i=0; i<ast->numChildren; i++)
	{
		switch(ast->children[i]->type)
		{
		case AstTranslate:
			t = translate(Math::Vector(ast->children[i]->data._vector)) * t;
			break;
		case AstRotate:
			t = rotate(Math::Vector(ast->children[i]->data._vector)) * t;
			break;
		case AstScale:
			t = scale(Math::Vector(ast->children[i]->data._vector)) *t;
			break;
		}
	}

	return t;
}

Point Transformation::origin() const
{
	return Point(mMatrix(3,0), mMatrix(3,1), mMatrix(3,2));
}

Transformation operator*(const Transformation &a, const Transformation &b)
{
	return Transformation(b.matrix() * a.matrix(), a.inverseMatrix() * b.inverseMatrix());
}

}