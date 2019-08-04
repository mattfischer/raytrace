#ifndef PARSE_BVH_FILE_HPP
#define PARSE_BVH_FILE_HPP

#include "Object/BoundingVolumeHierarchy.hpp"

#include <vector>
#include <string>

namespace Parse {
	class BvhFile
	{
	public:
		static Object::BoundingVolumeHierarchy load(const std::string &filename);
		static void save(const std::string &filename, const Object::BoundingVolumeHierarchy &boundingVolumeHierarchy);
	};
}
#endif