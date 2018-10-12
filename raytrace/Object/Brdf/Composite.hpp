#ifndef OBJECT_BRDF_COMPOSITE_HPP
#define OBJECT_BRDF_COMPOSITE_HPP

#include "Object/Brdf/Base.hpp"

#include <vector>

namespace Object {
namespace Brdf {

class Composite
{
public:
	static std::unique_ptr<Composite> fromAst(AST *ast);

	Composite(std::unique_ptr<Base> diffuse, std::unique_ptr<Base> specular);

	bool hasDiffuse() const;
	const Base &diffuse() const;

	bool hasSpecular() const;
	const Base &specular() const;

private:
	std::unique_ptr<Base> mDiffuse;
	std::unique_ptr<Base> mSpecular;
};

}
}

#endif