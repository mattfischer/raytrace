#include "Trace/Lighter/Direct.hpp"

#include "Trace/Tracer.hpp"
#include "Object/Scene.hpp"
#include "Object/Light.hpp"

namespace Trace {
namespace Lighter {

void Direct::light(const Trace::Intersection &intersection, Trace::Tracer &tracer, Accumulator &accumulator) const
{
	Math::Point point(intersection.point());

	for(const std::unique_ptr<Object::Light> &light : tracer.scene().lights())
	{
		Trace::Ray shadowRay = Trace::Ray::createFromPoints(point, light->transformation().origin());

		Trace::IntersectionVector::iterator begin, end;
		tracer.intersect(shadowRay, begin, end);

		Math::Vector lightVector = light->transformation().origin() - point;
		float lightMagnitude = lightVector.magnitude();
		Math::Vector lightDir = lightVector / lightMagnitude;

		if(begin == end || begin->distance() >= lightMagnitude)
		{
			accumulator.accumulate(light->color(), lightDir);
		}

		tracer.popTrace();
	}
}

}
}