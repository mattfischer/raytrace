#ifndef OBJECT_PRIMITIVE_SAMPLEABLE_HPP
#define OBJECT_PRIMITIVE_SAMPLEABLE_HPP

#include "Object/Primitive/Base.hpp"

namespace Object {
namespace Primitive {

class Sampleable : public Base
{
public:
	void sample(float u, float v, Math::Point &point, Math::Vector &du, Math::Vector &dv, Math::Normal &normal) const;

protected:
	virtual void doSample(float u, float v, Math::Point &point, Math::Vector &du, Math::Vector &dv, Math::Normal &normal) const = 0;
};
}
}
#endif