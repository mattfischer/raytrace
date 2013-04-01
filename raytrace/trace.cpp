#include "trace.h"
#include "Object/Object.hpp"

#include <algorithm>

#include <math.h>

Tracer::Tracer(Scene *scene, const Settings &settings)
{
	mScene = scene;
	mSettings = settings;
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

Tracer::Settings &Tracer::settings()
{
	return mSettings;
}

void Tracer::setScene(Scene *scene)
{
	mScene = scene;
}

Color Tracer::doLighting(const Math::Ray &ray, const Intersection &intersection) const
{
	Math::Vector point(intersection.point());
	Texture *texture = intersection.primitive()->texture();
	Color pointColor = texture->pointColor(intersection.objectPoint());

	if(!mSettings.lighting) {
		return pointColor;
	}

	Color totalColor;

	Color ambient = pointColor.scale(texture->finish()->ambient());

	totalColor += ambient;
	
	for(int i=0; i<mScene->lights().size(); i++)
	{
		Object::Light *light = mScene->lights()[i];
		double diffuse_coeff = 0;
		double specular_coeff = 0;
		
		Math::Ray lightRay = Math::Ray::createFromPoints(point, light->transformation().origin());

		mLightIntersections.clear();
		mScene->findIntersections(lightRay, mLightIntersections);
		
		Math::Vector lightVector = light->transformation().origin() - point;
		double lightMagnitude = lightVector.magnitude();
		Math::Vector lightDir = lightVector / lightMagnitude;

		if(mLightIntersections.size() == 0 || mLightIntersections[0].distance() >= lightMagnitude)
		{
			diffuse_coeff = abs(intersection.normal() * lightDir);

			Math::Vector incident = -lightDir;
			Math::Vector reflect = incident + intersection.normal() * (2 * (-intersection.normal() * incident));

			double dot = reflect * (mScene->camera()->transformation().origin() - point).normalize();

			if(texture->finish()->specular() > 0 && dot>0)
				specular_coeff = pow(dot, texture->finish()->specularPower());
		}

		Color diffuse = light->color() * pointColor.scale(texture->finish()->diffuse() * diffuse_coeff);
		Color specular = light->color().scale(texture->finish()->specular() * specular_coeff);

		totalColor += diffuse + specular;
	}

	if(texture->finish()->reflection() > 0 && ray.generation() < mSettings.maxRayGeneration)
	{
		Math::Vector incident = ray.direction();
		Math::Vector reflect = incident + intersection.normal() * (2 * (-intersection.normal() * incident));

		Math::Ray reflectRay(intersection.point(), reflect);
		reflectRay.setGeneration(ray.generation() + 1);

		Color c = traceRay(reflectRay);

		totalColor = totalColor * (1 - texture->finish()->reflection()) + c * texture->finish()->reflection();
 	}

	return totalColor.clamp();
}

Color Tracer::traceRay(const Math::Ray &ray) const
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

Color Tracer::tracePixel(int x, int y) const
{
	double aspectRatio = (double)mSettings.height / (double)mSettings.width;
	Color color;

	for(int i=0; i<mSettings.antialias; i++)
		for(int j=0; j<mSettings.antialias; j++)
		{
			Math::Ray ray = mScene->camera()->createRay((x + (double)i / mSettings.antialias) / mSettings.width, (y + (double)j / mSettings.antialias) / mSettings.height, mSettings.hFOV, mSettings.hFOV * aspectRatio);
			color = color + traceRay(ray);
		}

	return color / (mSettings.antialias * mSettings.antialias);
}
