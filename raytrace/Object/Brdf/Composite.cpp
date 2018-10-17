#include "Object/Brdf/Composite.hpp"

#include "Math/Normal.hpp"
#include "Math/Vector.hpp"

#include "Object/Brdf/Lambert.hpp"
#include "Object/Brdf/Phong.hpp"

#include <vector>

namespace Object {
	namespace Brdf {
		Composite::Composite(std::unique_ptr<Base> diffuse, std::unique_ptr<Base> specular)
		: mDiffuse(std::move(diffuse)), mSpecular(std::move(specular))
		{
		}

		bool Composite::hasDiffuse() const
		{
			return mDiffuse.get();
		}

		const Base &Composite::diffuse() const
		{
			return *mDiffuse;
		}

		bool Composite::hasSpecular() const
		{
			return mSpecular.get();
		}

		const Base &Composite::specular() const
		{
			return *mSpecular;
		}
	}
}