#ifndef OBJECT_PRIMITIVE_MODEL_HPP
#define OBJECT_PRIMITIVE_MODEL_HPP

#include "Object/Primitive/BezierPatch.hpp"

namespace Object {
namespace Primitive {

class Model
{
public:
	Model(const std::string &filename);

	const std::vector<std::unique_ptr<BezierPatch>> &patches() const;

private:
	std::vector<std::unique_ptr<BezierPatch>> mPatches;
};

}
}
#endif