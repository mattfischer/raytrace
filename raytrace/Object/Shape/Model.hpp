#ifndef OBJECT_SHAPE_MODEL_HPP
#define OBJECT_SHAPE_MODEL_HPP

#include "Object/Shape/Base.hpp"
#include "Object/Shape/BezierPatch.hpp"

namespace Object {
namespace Shape {

class Model : public Base
{
public:
	Model(const std::string &filename);

	virtual bool intersect(const Math::Ray &ray, float &distance, Math::Normal &normal) const;
	virtual BoundingVolume boundingVolume(const Math::Transformation &transformation) const;
	virtual bool sample(float u, float v, Math::Point &point, Math::Vector &du, Math::Vector &dv, Math::Normal &normal) const;

private:
	std::vector<std::unique_ptr<BezierPatch>> mPatches;
};

}
}
#endif