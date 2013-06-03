#include "Surface/Diffuse.hpp"
#include "Object/Scene.hpp"
#include "Object/Light.hpp"
#include "Object/Camera.hpp"
#include "Trace/Tracer.hpp"

#include "Surface/Albedo/Base.hpp"

#include <cmath>

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

class Accumulator : public Lighter::Base::Accumulator
{
public:
	Accumulator(const Math::Normal &normal, const Math::Vector &viewDirection, const Object::Color &albedo, float lambert, float specular, float specularPower)
		: mNormal(normal), mViewDirection(viewDirection), mAlbedo(albedo)
	{
		mLambert = lambert;
		mSpecular = specular;
		mSpecularPower = specularPower;
	}

	const Object::Color &totalColor()
	{
		return mTotalColor;
	}

	virtual void accumulate(const Object::Color &color, const Math::Vector &direction)
	{
		float lambertCoeff = abs(mNormal * direction);
		float specularCoeff = 0;

		Math::Vector incident = -direction;
		Math::Vector reflect = incident + Math::Vector(mNormal) * (2 * (-mNormal * incident));

		float dot = reflect * mViewDirection;

		if(mSpecular > 0 && dot>0)
			specularCoeff = std::pow(dot, mSpecularPower);

		Object::Color lambert = color * mAlbedo.scale(mLambert * lambertCoeff);
		Object::Color specular = color.scale(mSpecular * specularCoeff);

		mTotalColor += lambert + specular;
	}

private:
	const Math::Normal &mNormal;
	const Math::Vector &mViewDirection;
	const Object::Color &mAlbedo;
	float mLambert;
	float mSpecular;
	float mSpecularPower;
	Object::Color mTotalColor;
};

Object::Color Diffuse::color(const Trace::Intersection &intersection, Trace::Tracer &tracer) const
{
	Math::Vector viewDirection = (tracer.scene()->camera()->transformation().origin() - intersection.point()).normalize();
	Object::Color albedo = mAlbedo->color(intersection.objectPoint());

	Accumulator accumulator(intersection.normal(), viewDirection, albedo, mLambert, mSpecular, mSpecularPower);

	const Lighter::LighterVector &lighters = tracer.lighters();
	for(int i=0; i<lighters.size(); i++) {
		lighters[i]->light(intersection, tracer, accumulator);
	}

	Object::Color totalColor;
	totalColor += accumulator.totalColor();
	totalColor += albedo.scale(mAmbient);

	return totalColor;
}

}