#include "primitive.h"

Primitive::Primitive()
{
	mTexture = 0;
}

Primitive::~Primitive()
{
	if(mTexture)
		delete mTexture;
}

Texture *Primitive::texture() const 
{ 
	return mTexture; 
}

void Primitive::setTexture(Texture *texture)
{
	if(mTexture)
		delete mTexture;

	mTexture = texture;
}

void Primitive::intersect(const Ray &ray, std::vector<Intersection> &intersections) const
{
	Ray transformedRay = mTransformation.inverseTransformRay(ray);

	doIntersect(transformedRay, intersections);
}
