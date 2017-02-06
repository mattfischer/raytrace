#include "Object/Brdf/Base.hpp"

#include "Parse/AST.h"

#include "Object/Brdf/Ambient.hpp"
#include "Object/Brdf/Lambert.hpp"
#include "Object/Brdf/Phong.hpp"
#include "Object/Brdf/Specular.hpp"
#include "Object/Brdf/Composite.hpp"

#include <vector>

namespace Object {
namespace Brdf {

Object::Color Base::color(const Object::Color &incidentColor, const Math::Vector &incidentDirection, const Math::Normal &normal, const Math::Vector &outgoingDirection, const Object::Color &albedo) const
{
	return Object::Color(0, 0, 0);
}

bool Base::specular() const
{
	return false;
}

Object::Color Base::specularColor(const Object::Color &incidentColor, const Object::Color &albedo) const
{
	return Object::Color(0, 0, 0);
}

std::unique_ptr<Base> Base::fromAst(AST *ast)
{
	std::vector<std::unique_ptr<Base>> brdfs;

	for(int i=0; i<ast->numChildren; i++) {
		switch(ast->children[i]->type) {
			case AstAmbient:
				brdfs.push_back(Ambient::fromAst(ast->children[i]));
				break;

			case AstLambert:
				brdfs.push_back(Lambert::fromAst(ast->children[i]));
				break;

			case AstPhong:
				brdfs.push_back(Phong::fromAst(ast->children[i]));
				break;

			case AstSpecular:
				brdfs.push_back(Specular::fromAst(ast->children[i]));
				break;
		}
	}

	if(brdfs.size() == 1) {
		return std::move(brdfs[0]);
	} else {
		return std::make_unique<Composite>(std::move(brdfs));
	}
}

}
}