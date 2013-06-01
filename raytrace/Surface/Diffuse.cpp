#include "Surface/Diffuse.hpp"
#include "Object/Scene.hpp"
#include "Trace/Tracer.hpp"

#define AMBIENT_DEFAULT 0
#define LAMBERT_DEFAULT 0.8
#define SPECULAR_DEFAULT 0.0
#define SPECULARPOWER_DEFAULT 50

namespace Surface {

Diffuse::Diffuse(Surface::Albedo::Base *albedo, float ambient, float lambert, float specular, float specularPower)
{
	mAmbient = ambient;
	mAlbedo = albedo;
	mLambert = lambert;
	mSpecular = specular;
	mSpecularPower = specularPower;
}

Diffuse::~Diffuse()
{
	if(mAlbedo) {
		delete mAlbedo;
	}
}

Diffuse *Diffuse::fromAst(AST *ast)
{
	float ambient = AMBIENT_DEFAULT;
	float lambert = LAMBERT_DEFAULT;
	float specular = SPECULAR_DEFAULT;
	float specularPower = SPECULARPOWER_DEFAULT;
	Albedo::Base *albedo;

	for(int i=0; i<ast->numChildren; i++) {
		switch(ast->children[i]->type) {
			case AstAlbedo:
				albedo = Albedo::Base::fromAst(ast->children[i]);
				break;

			case AstAmbient:
				ambient = ast->children[i]->data._float;
				break;

			case AstLambert:
				lambert = ast->children[i]->data._float;
				break;

			case AstSpecular:
				specular = ast->children[i]->data._float;
				break;

			case AstSpecularPower:
				specularPower = ast->children[i]->data._float;
				break;
		}
	}

	return new Diffuse(albedo, ambient, lambert, specular, specularPower);
}

Object::Color Diffuse::color(const Trace::Intersection &intersection, const Trace::Tracer &tracer) const
{
	Math::Point point(intersection.point());
	Object::Color pointColor = mAlbedo->pointColor(intersection.objectPoint());
	Object::Color totalColor;

	Object::Color ambient = pointColor.scale(mAmbient);
	totalColor += ambient;

	for(int i=0; i<tracer.scene()->lights().size(); i++)
	{
		Object::Light *light = tracer.scene()->lights()[i];
		float lambert_coeff = 0;
		float specular_coeff = 0;
		
		Trace::Ray lightRay = Trace::Ray::createFromPoints(point, light->transformation().origin());

		Trace::IntersectionVector &intersections = tracer.intersections();
		int startSize = intersections.size();
		tracer.scene()->findIntersections(lightRay, intersections);
		
		Math::Vector lightVector = light->transformation().origin() - point;
		float lightMagnitude = lightVector.magnitude();
		Math::Vector lightDir = lightVector / lightMagnitude;

		if(intersections.size() == startSize || intersections[startSize].distance() >= lightMagnitude)
		{
			lambert_coeff = abs(intersection.normal() * lightDir);

			Math::Vector incident = -lightDir;
			Math::Vector reflect = incident + Math::Vector(intersection.normal()) * (2 * (-intersection.normal() * incident));

			float dot = reflect * (tracer.scene()->camera()->transformation().origin() - point).normalize();

			if(mSpecular > 0 && dot>0)
				specular_coeff = pow(dot, mSpecularPower);
		}

		intersections.erase(intersections.begin() + startSize, intersections.end());

		Object::Color lambert = light->color() * pointColor.scale(mLambert * lambert_coeff);
		Object::Color specular = light->color().scale(mSpecular * specular_coeff);

		totalColor += lambert + specular;
	}

	return totalColor;
}

}