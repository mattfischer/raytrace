#ifndef OBJECT_BRDF_PHONG_HPP
#define OBJECT_BRDF_PHONG_HPP

#include "Object/Brdf/Base.hpp"

namespace Object {
	namespace Brdf {
		class Phong : public Base
		{
		public:
			Phong(float strength, float power);

			virtual Object::Radiance radiance(const Object::Radiance &incidentRadiance, const Math::Vector &incidentDirection, const Math::Normal &normal, const Math::Vector &outgoingDirection, const Object::Color &albedo) const;

			virtual Math::Vector sample(float u, float v, const Math::Normal &normal, const Math::Vector &outgoingDirection, float &pdf) const;

			static std::unique_ptr<Phong> fromAst(AST *ast);

		private:
			float mStrength;
			float mPower;
		};
	}
}

#endif