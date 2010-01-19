#include "trace.h"
#include "object.h"

#include <math.h>

#define ANTIALIAS 3

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
			diffuse_coeff = intersection.normal() * lightDir;
			if(diffuse_coeff < 0) diffuse_coeff = 0;

			Vector incident = -lightDir;
			Vector reflect = incident + intersection.normal() * (2 * (-intersection.normal() * incident));

			double dot = reflect * (mScene->camera()->transformation().origin() - point).normalize();

			if(dot>0)
				specular_coeff = pow(dot, texture->finish()->specularPower());
		}

		Color diffuse = light->color() * pointColor.scale(texture->finish()->diffuse() * diffuse_coeff);
		Color specular = light->color().scale(texture->finish()->specular() * specular_coeff);

		totalColor += diffuse + specular;
	}

	return totalColor.clamp();
}

Color Tracer::tracePixel(double x, double y, double width, double height) const
{
	Color totalColor;

	for(int i=0; i<ANTIALIAS; i++)
		for(int j=0; j<ANTIALIAS; j++)
		{
			Ray ray = mScene->camera()->createRay(x + (double)i / ANTIALIAS, y + (double)j / ANTIALIAS, width, height);

			mIntersections.clear();
			mScene->findIntersections(ray, mIntersections);

			Color color(.2, .2, .2);

			if(mIntersections.size() > 0)
				color = doLighting(ray, mIntersections[0]);

			totalColor = totalColor + color;
		}

	return totalColor / (ANTIALIAS * ANTIALIAS);
}
