#include "trace.h"
#include "object.h"

#include "photon.h"

#include <algorithm>

#include <math.h>

#define MAX_RAY_GENERATION 2

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

/*
		if(ray.generation() == 1)
		{
#define NUM_PATH_TRACE_RAYS 100
			for(int i=0; i<NUM_PATH_TRACE_RAYS; i++)
			{
				Vector v(rand() * (rand() % 2 - 0.5), rand() * (rand() % 2 - 0.5), rand() * (rand() % 2 - 0.5));
				v = v.normalize();
				Vector normal = intersection.normal();
				if(v * normal < 0)
				{
					continue;
				}

				Ray reflectRay(point, v);
				reflectRay.setGeneration(ray.generation() + 1);

				Color c = traceRay(reflectRay);
				
				totalColor = totalColor + (c * (v * normal) / (double)NUM_PATH_TRACE_RAYS) * 5;
			}
		}*/
	}

	#define NUM_PHOTONS 30

	Photon photons[NUM_PHOTONS];
	Vector normal = intersection.normal();

	int num = mScene->photonMap().nearestPhotons(point, normal, photons, NUM_PHOTONS);
	double distance2 = (photons[num - 1].position - point).magnitude2();

	double photon = 0.01 * (double)num / (3.14 * distance2);
	totalColor += pointColor.scale(photon);

	if(texture->finish()->reflection() > 0 && ray.generation() < MAX_RAY_GENERATION)
	{
		Vector incident = ray.direction();
		Vector reflect = incident + intersection.normal() * (2 * (-intersection.normal() * incident));
		
		Ray reflectRay(intersection.point(), reflect);
		reflectRay.setGeneration(ray.generation() + 1);

		Color c = traceRay(reflectRay);

		totalColor = totalColor * (1 - texture->finish()->reflection()) + c * texture->finish()->reflection();
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