#include "Math/Bivector.hpp"

namespace Math {
	Bivector::Bivector(const Math::Vector &u, const Math::Vector &v)
		: mU(u), mV(v)
	{
	}

	const Math::Vector &Bivector::u() const
	{
		return mU;
	}

	const Math::Vector &Bivector::v() const
	{
		return mV;
	}

	Bivector Bivector::operator*(float b) const
	{
		return Bivector(mU * b, mV * b);
	}

	Bivector Bivector::operator+(const Bivector &b) const
	{
		return Bivector(mU + b.mU, mV + b.mV);
	}

	Math::Vector Bivector::operator*(const Math::Vector2D &b) const
	{
		return mU * b.u() + mV * b.v();
	}
}