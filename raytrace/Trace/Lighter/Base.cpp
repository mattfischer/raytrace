#include "Trace/Lighter/Base.hpp"

#include "Trace/Lighter/Direct.hpp"
#include "Trace/Lighter/Radiant.hpp"
#include "Trace/Lighter/Specular.hpp"

namespace Trace {
namespace Lighter {

std::vector<std::unique_ptr<Base>> Base::createLighters()
{
	std::vector<std::unique_ptr<Base>> ret;

	ret.push_back(std::make_unique<Direct>());
	ret.push_back(std::make_unique<Radiant>());
	ret.push_back(std::make_unique<Specular>());

	return ret;
}

}
}