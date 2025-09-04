#include "Parse/BptLoader.hpp"

#include "Object/Impl/Shape/BezierPatch.hpp"
#include "Object/Impl/Shape/Group.hpp"
#include <fstream>

namespace Parse {
    std::unique_ptr<Object::Shape> BptLoader::load(const std::string &filename)
    {
        std::vector<std::unique_ptr<Object::Shape>> patches;

        std::ifstream file(filename.c_str());
        int numPatches;

        file >> numPatches;
        for (int i = 0; i < numPatches; i++) {
            int dimx, dimy;
            file >> dimx >> dimy;
            std::vector<Math::Point> controlPoints;
            for (int j = 0; j < 16; j++) {
                float x, y, z;
                file >> x >> y >> z;
                controlPoints.push_back(Math::Point(x, y, z));
            }
            patches.push_back(std::make_unique<Object::Impl::Shape::BezierPatch>(16, 16, std::move(controlPoints)));
        }

        if (patches.size() == 1) {
            return std::move(patches[0]);
        }
        else {
            return std::make_unique<Object::Impl::Shape::Group>(std::move(patches));
        }
    }
}