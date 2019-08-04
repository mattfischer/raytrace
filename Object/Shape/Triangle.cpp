#include "Object/Shape/Triangle.hpp"

namespace Object {
	namespace Shape {
		bool Triangle::intersect(const Math::Ray &ray, const Math::Point &p, const Math::Point &pu, const Math::Point &pv, float &distance, float &u, float &v)
		{
			Math::Vector E1 = pu - p;
			Math::Vector E2 = pv - p;
			Math::Vector P = ray.direction() % E2;

			float den = P * E1;
			if (den > -1.0e-10f && den < 1.0e-10f) {
				return false;
			}

			float iden = 1.0f / den;

			Math::Vector T = ray.origin() - p;
			float uu = (P * T) * iden;
			if (uu < 0 || uu > 1) {
				return false;
			}

			Math::Vector Q = T % E1;
			float vv = (Q * ray.direction()) * iden;
			if (vv < 0 || uu + vv > 1) {
				return false;
			}

			float d = (Q * E2) * iden;
			if (d < 0 || d >= distance) {
				return false;
			}

			distance = d;
			u = uu;
			v = vv;
			return true;
		}
	}
}