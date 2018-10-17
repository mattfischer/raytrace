#include "Object/Surface.hpp"

#include "Object/Albedo/Base.hpp"
#include "Object/Brdf/Base.hpp"

#include "Parse/AST.h"

namespace Object {
	Surface::Surface(std::unique_ptr<Albedo::Base> albedo, std::unique_ptr<Brdf::Composite> brdf, const Object::Radiance &radiance)
	{
		mAlbedo = std::move(albedo);
		mBrdf = std::move(brdf);
		mRadiance = radiance;
	}

	const Albedo::Base &Surface::albedo() const
	{
		return *mAlbedo;
	}

	const Brdf::Composite &Surface::brdf() const
	{
		return *mBrdf;
	}

	const Object::Radiance &Surface::radiance() const
	{
		return mRadiance;
	}
}