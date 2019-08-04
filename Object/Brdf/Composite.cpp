#include "Object/Brdf/Composite.hpp"

#include "Math/Normal.hpp"
#include "Math/Vector.hpp"

#include "Object/Brdf/Lambert.hpp"
#include "Object/Brdf/Phong.hpp"

#include <vector>

namespace Object {
	namespace Brdf {
		Composite::Composite(std::unique_ptr<Base> diffuse, std::unique_ptr<Base> specular, float transmitIor)
		: mDiffuse(std::move(diffuse)), mSpecular(std::move(specular))
		{
			mTransmitIor = transmitIor;
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

		bool Composite::hasTransmit() const
		{
			return mTransmitIor != 0;
		}

		float Composite::transmitIor() const
		{
			return mTransmitIor;
		}
	}
}