#include "Parse/BvhFile.hpp"

#include <fstream>

namespace Parse {
    Object::BoundingVolumeHierarchy BvhFile::load(const std::string &filename)
    {
        std::vector<Object::BoundingVolumeHierarchy::Node> nodes;

        std::ifstream file(filename.c_str(), std::ios_base::binary);
        file.seekg(0, std::ios_base::end);
        int size = file.tellg();
        file.seekg(std::ios_base::beg);
        nodes.resize(size / sizeof(Object::BoundingVolumeHierarchy::Node));
        file.read((char*)&nodes[0], size);

        return Object::BoundingVolumeHierarchy(std::move(nodes));
    }

    void BvhFile::save(const std::string &filename, const Object::BoundingVolumeHierarchy &boundingVolumeHierarchy)
    {
        std::ofstream file(filename.c_str(), std::ios_base::binary);
        file.write((const char*)&boundingVolumeHierarchy.nodes()[0], boundingVolumeHierarchy.nodes().size() * sizeof(Object::BoundingVolumeHierarchy::Node));
    }
}