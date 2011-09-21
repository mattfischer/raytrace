#include "trace.h"
#include "object.h"

#include <algorithm>

#include <math.h>

#define MAX_RAY_GENERATION 2
#define ANTIALIAS 1
#define FOV 45

Tracer::Tracer(Scene *scene)
{
	mScene = scene;
}

Tracer::~Tracer()
{
	if(mScene)
		delete mScene;
}

Scene *Tracer::scene() const
{
	return mScene;
}

void Tracer::setScene(Scene *scene)
{
	mScene = scene;
}

Color Tracer::doLighting(const Ray &ray, const Intersection &intersection) const
{
	Vector point(intersection.point());
	Texture *texture = intersection.primitive()->texture();
	Color pointColor = texture->pointColor(intersection.objectPoint());
	Color totalColor;

	Color ambient = pointColor.scale(texture->finish()->ambient());

	totalColor += ambient;
	
	for(int i=0; i<mScene->lights().size(); i++)
	{
		Light *light = mScene->lights()[i];
		double diffuse_coeff = 0;
		double specular_coeff = 0;
		
		Ray lightRay = Ray::createFromPoints(point, light->transformation().origin());

		mLightIntersections.clear();
		mScene->findIntersections(lightRay, mLightIntersections);
		
		Vector lightVector = light->transformation().origin() - point;
		double lightMagnitude = lightVector.magnitude();
		Vector lightDir = lightVector / lightMagnitude;

		if(mLightIntersections.size() == 0 || mLightIntersections[0].distance() >= lightMagnitude)
		{
			diffuse_coeff = abs(intersection.normal() * lightDir);

			Vector incident = -lightDir;
			Vector reflect = incident + intersection.normal() * (2 * (-intersection.normal() * incident));

			double dot = reflect * (mScene->camera()->transformation().origin() - point).normalize();

			if(texture->finish()->specular() > 0 && dot>0)
				specular_coeff = pow(dot, texture->finish()->specularPower());
		}

		Color diffuse = light->color() * pointColor.scale(texture->finish()->diffuse() * diffuse_coeff);
		Color specular = light->color().scale(texture->finish()->specular() * specular_coeff);

		totalColor += diffuse + specular;
	}

	return totalColor.clamp();
}

Color Tracer::traceRay(const Ray &ray) const
{
	mIntersections.clear();
	mScene->findIntersections(ray, mIntersections);

	Color color(.2, .2, .2);

	if(mIntersections.size() > 0)
	{
		color = doLighting(ray, mIntersections[0]);
	}
	return color;
}

Color Tracer::tracePixel(int x, int y, int width, int height) const
{
	double aspectRatio = (double)height / (double)width;
	Color color;

	for(int i=0; i<ANTIALIAS; i++)
		for(int j=0; j<ANTIALIAS; j++)
		{
			Ray ray = mScene->camera()->createRay((x + (double)i / ANTIALIAS) / width, (y + (double)j / ANTIALIAS) / height, FOV, FOV * aspectRatio);
			color = color + traceRay(ray);
		}

	return color / (ANTIALIAS * ANTIALIAS);
}
