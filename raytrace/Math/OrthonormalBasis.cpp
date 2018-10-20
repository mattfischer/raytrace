#include "Math/OrthonormalBasis.hpp"

namespace Math {
	OrthonormalBasis::OrthonormalBasis(const Math::Vector &z)
	{
		Math::Vector x;
		Math::Vector y;

		Math::Vector vectors[] = { Math::Vector(1,0,0), Math::Vector(0,1,0), Math::Vector(0,0,1) };

		for (const Math::Vector &v : vectors) {
			Math::Vector p = v - z * (v * z);
			if (p.magnitude2() > x.magnitude2()) {
				y = x;
				x = p;
			}
			else if (p.magnitude2() > y.magnitude2()) {
				y = p;
			}
		}

		x = x.normalize();
		y = y - x * (x * y);

		y = y.normalize();

		mMatrix = Math::Matrix(x.x(), y.x(), z.x(), 0,
							   x.y(), y.y(), z.y(), 0,
							   x.z(), y.z(), z.z(), 0,
							   0,     0,     0,     1);
	}

	OrthonormalBasis::OrthonormalBasis(const Math::Normal &normal)
		: OrthonormalBasis(Math::Vector(normal))
	{
	}

	Math::Vector OrthonormalBasis::worldToLocal(const Math::Vector &world) const
	{
		return Math::Vector(world * mMatrix);
	}

	Math::Vector OrthonormalBasis::localToWorld(const Math::Vector &local) const
	{
		return Math::Vector(mMatrix * local);
	}
}