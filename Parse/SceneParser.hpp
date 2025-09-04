#ifndef PARSE_SCENE_PARSER_HPP
#define PARSE_SCENE_PARSER_HPP

#include "Object/Scene.hpp"

#include "Object/Impl/Shape/Sphere.hpp"
#include "Object/Impl/Shape/Quad.hpp"
#include "Object/Impl/Shape/TriangleMesh.hpp"

#include <string>
#include <fstream>
#include <memory>

namespace Parse {
    class SceneParser {
    public:
        SceneParser(const std::string &filename);

        std::unique_ptr<Object::Scene> parse();

    private:
        void skipWhitespace();
        void throwUnexpected();

        float parseFloat();
        std::string parseString();

        bool matchLiteral(const std::string &literal);
        void expectLiteral(const std::string &literal);

        bool matchEnd();

        void expectLeftBrace();
        void expectRightBrace();
        bool matchRightBrace();

        void parseFloatTriple(float items[]);
        Math::Point parsePoint();
        Math::Vector parseVector();
        Math::Radiance parseRadiance();
        Math::Color parseColor();

        std::unique_ptr<Object::Scene> parseScene();
        std::unique_ptr<Object::Camera> tryParseCamera();
        std::unique_ptr<Object::Light> tryParseLight();

        std::unique_ptr<Object::Primitive> tryParsePrimitive();

        std::unique_ptr<Object::Surface> tryParseSurface();
        std::unique_ptr<Object::Albedo> tryParseAlbedo();
        bool tryParseBrdfs(std::vector<std::unique_ptr<Object::Brdf>> &brdfs, float &transmitIor);
        std::unique_ptr<Object::NormalMap> tryParseNormalMap();

        bool tryParseTransformation(Math::Transformation &transformation);

        std::ifstream mFile;
        std::string mData;
        int mPos;
        int mLine;
    };
}
#endif