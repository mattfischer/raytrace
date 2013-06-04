#include "Surface/Diffuse.hpp"
#include "Object/Scene.hpp"
#include "Object/Light.hpp"
#include "Object/Camera.hpp"
#include "Trace/Tracer.hpp"

#include "Surface/Albedo/Base.hpp"
#include "Surface/Brdf/Base.hpp"

namespace Surface {

Diffuse::Diffuse(const Surface::Albedo::Base *albedo, const Brdf::Base *brdf)
{
	mAlbedo = albedo;
	mBrdf = brdf;
}

Diffuse::~Diffuse()
{
	if(mAlbedo) {
		delete mAlbedo;
	}

	if(mBrdf) {
		delete mBrdf;
	}
}

Diffuse *Diffuse::fromAst(AST *ast)
{
	const Albedo::Base *albedo;
	const Brdf::Base *brdf;

	for(int i=0; i<ast->numChildren; i++) {
		switch(ast->children[i]->type) {
			case AstAlbedo:
				albedo = Albedo::Base::fromAst(ast->children[i]);
				break;

			case AstBrdf:
				brdf = Brdf::Base::fromAst(ast->children[i]);
				break;
		}
	}

	return new Diffuse(albedo, brdf);
}

class Accumulator : public Lighter::Base::Accumulator
{
public:
	Accumulator(const Math::Normal &normal, const Math::Vector &viewDirection, const Object::Color &albedo, const Brdf::Base *brdf)
		: mNormal(normal), mViewDirection(viewDirection), mAlbedo(albedo)
	{
		mBrdf = brdf;
	}

	const Object::Color &totalColor()
	{
		return mTotalColor;
	}

	virtual void accumulate(const Object::Color &color, const Math::Vector &direction)
	{
		mTotalColor += mBrdf->color(color, direction, mNormal, mViewDirection, mAlbedo);
	}

private:
	const Math::Normal &mNormal;
	const Math::Vector &mViewDirection;
	const Object::Color &mAlbedo;
	const Brdf::Base *mBrdf;
	Object::Color mTotalColor;
};

Object::Color Diffuse::color(const Trace::Intersection &intersection, Trace::Tracer &tracer) const
{
	Math::Vector viewDirection = (tracer.scene()->camera()->transformation().origin() - intersection.point()).normalize();
	Object::Color albedo = mAlbedo->color(intersection.objectPoint());

	Accumulator accumulator(intersection.normal(), viewDirection, albedo, mBrdf);

	const Lighter::LighterVector &lighters = tracer.lighters();
	for(int i=0; i<lighters.size(); i++) {
		lighters[i]->light(intersection, tracer, accumulator);
	}

	return accumulator.totalColor();
}

}