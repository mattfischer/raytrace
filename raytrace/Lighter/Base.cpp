#include "Lighter/Base.hpp"

#include "Lighter/Direct.hpp"

namespace Lighter {

LighterVector Base::createLighters()
{
	LighterVector ret;

	ret.push_back(new Direct);

	return ret;
}

}