#include "Trace/Tracer.hpp"
#include "Object/Base.hpp"

#include <algorithm>

#include <math.h>

namespace Trace {

Tracer::Tracer(Object::Scene *scene, const Settings &settings)
{
	mScene = scene;
	mSettings = settings;
}

Tracer::~Tracer()
{
	if(mScene)
		delete mScene;
}

Object::Scene *Tracer::scene() const
{
	return mScene;
}

Tracer::Settings &Tracer::settings()
{
	return mSettings;
}

void Tracer::setScene(Object::Scene *scene)
{
	mScene = scene;
}

Object::Color Tracer::doLighting(const Trace::Ray &ray, const Intersection &intersection) const
{
	Math::Point point(intersection.point());
	Object::Texture *texture = intersection.primitive()->texture();
	Object::Color pointColor = texture->pointColor(intersection.objectPoint());

	if(!mSettings.lighting) {
		return pointColor;
	}

	Object::Color totalColor;

	Object::Color ambient = pointColor.scale(texture->finish()->ambient());

	totalColor += ambient;
	
	for(int i=0; i<mScene->lights().size(); i++)
	{
		Object::Light *light = mScene->lights()[i];
		float diffuse_coeff = 0;
		float specular_coeff = 0;
		
		Trace::Ray lightRay = Trace::Ray::createFromPoints(point, light->transformation().origin());

		mLightIntersections.clear();
		mScene->findIntersections(lightRay, mLightIntersections);
		
		Math::Vector lightVector = light->transformation().origin() - point;
		float lightMagnitude = lightVector.magnitude();
		Math::Vector lightDir = lightVector / lightMagnitude;

		if(mLightIntersections.size() == 0 || mLightIntersections[0].distance() >= lightMagnitude)
		{
			diffuse_coeff = abs(intersection.normal() * lightDir);

			Math::Vector incident = -lightDir;
			Math::Vector reflect = incident + Math::Vector(intersection.normal()) * (2 * (-intersection.normal() * incident));

			float dot = reflect * (mScene->camera()->transformation().origin() - point).normalize();

			if(texture->finish()->specular() > 0 && dot>0)
				specular_coeff = pow(dot, texture->finish()->specularPower());
		}

		Object::Color diffuse = light->color() * pointColor.scale(texture->finish()->diffuse() * diffuse_coeff);
		Object::Color specular = light->color().scale(texture->finish()->specular() * specular_coeff);

		totalColor += diffuse + specular;
	}

	if(texture->finish()->reflection() > 0 && ray.generation() < mSettings.maxRayGeneration)
	{
		Math::Vector incident = ray.direction();
		Math::Vector reflect = incident + Math::Vector(intersection.normal()) * (2 * (-intersection.normal() * incident));

		Trace::Ray reflectRay(intersection.point(), reflect);
		reflectRay.setGeneration(ray.generation() + 1);

		Object::Color c = traceRay(reflectRay);

		totalColor = totalColor * (1 - texture->finish()->reflection()) + c * texture->finish()->reflection();
 	}

	return totalColor.clamp();
}

Object::Color Tracer::traceRay(const Trace::Ray &ray) const
{
	mIntersections.clear();
	mScene->findIntersections(ray, mIntersections);

	Object::Color color(.2, .2, .2);

	if(mIntersections.size() > 0)
	{
		color = doLighting(ray, mIntersections[0]);
	}
	return color;
}

Object::Color Tracer::tracePixel(int x, int y) const
{
	float aspectRatio = (float)mSettings.height / (float)mSettings.width;
	Object::Color color;

	for(int i=0; i<mSettings.antialias; i++)
		for(int j=0; j<mSettings.antialias; j++)
		{
			Trace::Ray ray = mScene->camera()->createRay((x + (float)i / mSettings.antialias) / mSettings.width, (y + (float)j / mSettings.antialias) / mSettings.height, aspectRatio);
			color = color + traceRay(ray);
		}

	return color / (mSettings.antialias * mSettings.antialias);
}

}