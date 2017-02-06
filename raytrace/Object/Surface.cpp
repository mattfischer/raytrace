#include "Object/Surface.hpp"

#include "Object/Scene.hpp"
#include "Object/Light.hpp"
#include "Object/Camera.hpp"
#include "Trace/Tracer.hpp"

#include "Object/Albedo/Base.hpp"
#include "Object/Brdf/Base.hpp"

#include "Parse/AST.h"

namespace Object {

	Surface::Surface(const Albedo::Base *albedo, const Brdf::Base *brdf)
	{
		mAlbedo = albedo;
		mBrdf = brdf;
	}

	Surface::~Surface()
	{
		if (mAlbedo) {
			delete mAlbedo;
		}

		if (mBrdf) {
			delete mBrdf;
		}
	}

	Surface *Surface::fromAst(AST *ast)
	{
		const Albedo::Base *albedo;
		const Brdf::Base *brdf;

		for (int i = 0; i < ast->numChildren; i++) {
			switch (ast->children[i]->type) {
			case AstAlbedo:
				albedo = Albedo::Base::fromAst(ast->children[i]);
				break;

			case AstBrdf:
				brdf = Brdf::Base::fromAst(ast->children[i]);
				break;
			}
		}

		return new Surface(albedo, brdf);
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

	Object::Color Surface::color(const Trace::Intersection &intersection, Trace::Tracer &tracer) const
	{
		Math::Vector viewDirection = (tracer.scene()->camera()->transformation().origin() - intersection.point()).normalize();
		Object::Color albedo = mAlbedo->color(intersection.objectPoint());

		Accumulator accumulator(intersection.normal(), viewDirection, albedo, mBrdf);

		const Lighter::LighterVector &lighters = tracer.lighters();
		for (int i = 0; i < lighters.size(); i++) {
			lighters[i]->light(intersection, tracer, accumulator);
		}

		Object::Color color = accumulator.totalColor();

		if (mBrdf->specular()) {
			const Trace::Ray &ray = intersection.ray();
			if (ray.generation() < tracer.settings().maxRayGeneration) {
				Math::Vector incident = ray.direction();
				Math::Vector reflect = incident + Math::Vector(intersection.normal()) * (2 * (-intersection.normal() * incident));

				Trace::Ray reflectRay(intersection.point(), reflect, ray.generation() + 1);
				Object::Color reflectColor = tracer.traceRay(reflectRay);

				color += mBrdf->specularColor(reflectColor, albedo);
			}
		}

		return color;
	}
}