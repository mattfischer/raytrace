#include "Trace/Lighter/Base.hpp"

#include "Trace/Lighter/Direct.hpp"

namespace Trace {
namespace Lighter {

LighterVector Base::createLighters()
{
	LighterVector ret;

	ret.push_back(new Direct);

	return ret;
}

}
}