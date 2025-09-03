#include "Parse/SceneParser.hpp"

#include "Object/Shape/Transformed.hpp"

#include "Object/Albedo/Solid.hpp"
#include "Object/Albedo/Texture.hpp"

#include "Object/Brdf/Lambert.hpp"
#include "Object/Brdf/Phong.hpp"
#include "Object/Brdf/OrenNayar.hpp"
#include "Object/Brdf/TorranceSparrow.hpp"

#include "Parse/BptLoader.hpp"
#include "Parse/PlyLoader.hpp"
#include "Parse/BmpLoader.hpp"

#include <sstream>
#include <iostream>

namespace Parse {
    struct ParseException {
        ParseException(const std::string &m) : message(m) {}
 
        std::string message;
    };

    SceneParser::SceneParser(const std::string &filename)
    : mFile(filename, std::ios_base::binary)
    {
    }

    std::unique_ptr<Object::Scene> SceneParser::parse()
    {
        if(!mFile.good()) {
            return nullptr;
        }

        mFile.seekg(0, std::ios_base::end);
        mData.resize(mFile.tellg());
        mFile.seekg(0);
        mFile.read(&mData[0], mData.size());
        mPos = 0;
        mLine = 1;

        skipWhitespace();

        try {
            return parseScene();
        } catch(ParseException e) {
            std::cout << "Error, line " << mLine << ": " << e.message << std::endl;
            return nullptr;
        }
    }

    void SceneParser::skipWhitespace()
    {
        while(mPos < mData.size()) {
            if(std::isblank(mData[mPos]) || mData[mPos] == '\r') {
                mPos++;
                continue;
            } else if(mData[mPos] == '\n') {
                mLine++;
                mPos++;
                continue;
            } else {
                break;
            }
        }
    }

    void SceneParser::throwUnexpected()
    {
        throw ParseException("Unexpected token");
    }

    float SceneParser::parseFloat()
    {
        if(matchEnd() || !(std::isdigit(mData[mPos]) || mData[mPos] == '-' || mData[mPos] == '.')) {
            throw ParseException("Expected <float>");
        }

        int pos = mPos + 1;
        while(pos < mData.size() && (std::isdigit(mData[pos]) || mData[pos] == '.' || mData[pos] == '-')) {
            pos++;
        }

        std::string s = mData.substr(mPos, pos - mPos);
        mPos = pos;

        skipWhitespace();

        return std::atof(s.c_str());
    }

    std::string SceneParser::parseString()
    {
        if(matchEnd() || mData[mPos] != '"') {
            throw ParseException("Expected <string>");
        }

        int pos = mPos + 1;
        while(mData[pos] != '"') {
            pos++;
            if(pos == mData.size()) {
                throw ParseException("Unterminated string constant");
            }
        }

        std::string ret = mData.substr(mPos + 1, pos - mPos - 1);
        mPos = pos + 1;

        skipWhitespace();

        return ret;
    }

    bool SceneParser::matchLiteral(const std::string &literal)
    {
        if(matchEnd()) {
            return false;
        }

        if(!strncmp(mData.data() + mPos, literal.c_str(), literal.size())) {
            mPos += literal.size();
            skipWhitespace();

            return true;
        }

        return false;
    }

    void SceneParser::expectLiteral(const std::string &literal)
    {
        if(!matchLiteral(literal)) {
            std::stringstream ss;
            ss << "Expected " << literal;
            throw ParseException(ss.str());
        }
    }

    bool SceneParser::matchEnd()
    {
        return mPos == mData.size();
    }

    void SceneParser::expectLeftBrace()
    {
        expectLiteral("{");
    }

    void SceneParser::expectRightBrace()
    {
        expectLiteral("}");
    }

    bool SceneParser::matchRightBrace()
    {
        return matchLiteral("}");
    }

    void SceneParser::parseFloatTriple(float items[3])
    {
        expectLiteral("<");
        items[0] = parseFloat();
        expectLiteral(",");
        items[1] = parseFloat();
        expectLiteral(",");
        items[2] = parseFloat();
        expectLiteral(">");
    }

    Math::Point SceneParser::parsePoint()
    {
        float items[3];
        parseFloatTriple(items);
        return Math::Point(items[0], items[1], items[2]);
    }

    Math::Vector SceneParser::parseVector()
    {
        float items[3];
        parseFloatTriple(items);
        return Math::Vector(items[0], items[1], items[2]);
    }

    Math::Radiance SceneParser::parseRadiance()
    {
        float items[3];
        parseFloatTriple(items);
        return Math::Radiance(items[0], items[1], items[2]);
    }

    Math::Color SceneParser::parseColor()
    {
        float items[3];
        parseFloatTriple(items);
        return Math::Color(items[0], items[1], items[2]);
    }

    std::unique_ptr<Object::Scene> SceneParser::parseScene()
    {
        std::unique_ptr<Object::Camera> camera;
        std::vector<std::unique_ptr<Object::Primitive>> primitives;
        std::vector<std::unique_ptr<Object::Light::Base>> lights;
        Math::Radiance skyRadiance;

        while(!matchEnd()) {
            if(auto cam = tryParseCamera()) {
                camera = std::move(cam);
                continue;
            }

            if(auto primitive = tryParsePrimitive()) {
                primitives.push_back(std::move(primitive));
                continue;
            }

            if(auto light = tryParseLight()) {
                lights.push_back(std::move(light));
                continue;
            }

            if(matchLiteral("sky")) {
                expectLeftBrace();
                skyRadiance = parseRadiance();
                expectRightBrace();
                continue;
            }

            throwUnexpected();
        }

        return std::make_unique<Object::Scene>(std::move(camera), std::move(primitives), std::move(lights), skyRadiance);
    }

    std::unique_ptr<Object::Camera> SceneParser::tryParseCamera()
    {
        if(!matchLiteral("camera")) {
            return nullptr;
        }
        expectLeftBrace();

        Math::Point position = parsePoint();
        Math::Point lookAt = parsePoint();
        float focalLength = parseFloat();
        float apertureSize = parseFloat();

        expectRightBrace();

        return std::make_unique<Object::Camera>(position, (lookAt - position).normalize(), Math::Vector(0, 1, 0), 60.0f, focalLength, apertureSize);
    }

    std::unique_ptr<Object::Light::Base> SceneParser::tryParseLight()
    {
        if(!matchLiteral("point_light")) {
            return nullptr;
        }
        expectLeftBrace();

        Math::Point position = parsePoint();
        Math::Radiance radiance = parseRadiance();

        expectRightBrace();

        return std::make_unique<Object::Light::Point>(position, radiance);
    }

    std::unique_ptr<Object::Primitive> SceneParser::tryParsePrimitive()
    {
        std::unique_ptr<Object::Shape::Base> shape;
        if(matchLiteral("sphere")) {
            expectLeftBrace();

            Math::Point position = parsePoint();
            float radius = parseFloat();
            shape = std::make_unique<Object::Shape::Sphere>(position, radius);
        } else if(matchLiteral("quad")) {
            expectLeftBrace();

            Math::Point position = parsePoint();
            Math::Vector side1 = parseVector();
            Math::Vector side2 = parseVector();
            shape = std::make_unique<Object::Shape::Quad>(position, side1, side2);
        } else if(matchLiteral("model")) {
            expectLeftBrace();
            
            std::string filename = parseString();
            std::string extension = filename.substr(filename.find_last_of('.'));
            
            if (extension == ".bpt") {
                shape = BptLoader::load(filename);
            } else if (extension == ".ply") {
                shape = PlyLoader::load(filename);
            } else {
                std::stringstream ss;
                ss << "Unknown model extension " << extension;
                throw ParseException(ss.str());
            }
        } else {
            return nullptr;
        }

        std::unique_ptr<Object::Surface> surface;
        Math::Transformation transformation;
        while(!matchRightBrace()) {
            if(auto newSurface = tryParseSurface()) {
                surface = std::move(newSurface);
                continue;
            }

            if(tryParseTransformation(transformation)) {
                shape = std::make_unique<Object::Shape::Transformed>(std::move(shape), transformation);
                continue;
            }

            throwUnexpected();
        }

        return std::make_unique<Object::Primitive>(std::move(shape), std::move(surface));
    }

    std::unique_ptr<Object::Surface> SceneParser::tryParseSurface()
    {
        if(!matchLiteral("surface")) {
            return nullptr;
        }
        expectLeftBrace();
        
        std::unique_ptr<Object::Albedo::Base> albedo;
        std::vector<std::unique_ptr<Object::Brdf::Base>> brdfs;
        float transmitIor = 0.0f;
        Math::Radiance radiance;
        std::unique_ptr<Object::NormalMap> normalMap;

        while(!matchRightBrace()) {
            if(auto newAlbedo = tryParseAlbedo()) {
                albedo = std::move(newAlbedo);
                continue;
            }

            if(tryParseBrdfs(brdfs, transmitIor)) {
                continue;
            }

            if(auto newNormalMap = tryParseNormalMap()) {
                normalMap = std::move(newNormalMap);
                continue;
            }

            if(matchLiteral("radiance")) {
                radiance = parseRadiance();
                continue;
            }

            throwUnexpected();
        }
        
        return std::make_unique<Object::Surface>(std::move(albedo), std::move(brdfs), transmitIor, radiance, std::move(normalMap));
    }

    bool SceneParser::tryParseTransformation(Math::Transformation &transformation)
    {
        if(!matchLiteral("transform")) {
            return false;
        }
        expectLeftBrace();

        while(!matchRightBrace()) {
            if(matchLiteral("translate")) {
                Math::Vector vector = parseVector();
                transformation = Math::Transformation::translate(vector) * transformation;
                continue;
            }

            if(matchLiteral("rotate")) {
                Math::Vector vector = parseVector();
                transformation = Math::Transformation::rotate(vector) * transformation;
                continue;
            }

            if(matchLiteral("scale")) {
                Math::Vector vector = parseVector();
                transformation = Math::Transformation::scale(vector) * transformation;
                continue;
            }

            if(matchLiteral("uniform_scale")) {
                float scale = parseFloat();
                transformation = Math::Transformation::uniformScale(scale) * transformation;
                continue;
            }

            throwUnexpected();
        }

        return true;
    }

    std::unique_ptr<Object::Albedo::Base> SceneParser::tryParseAlbedo()
    {
        if(!matchLiteral("albedo")) {
            return nullptr;
        }
        expectLeftBrace();

        std::unique_ptr<Object::Albedo::Base> albedo;

        if(matchLiteral("color")) {
            Math::Color color = parseColor();
            albedo = std::make_unique<Object::Albedo::Solid>(color);
        } else if(matchLiteral("texture")) {
            auto texture = BmpLoader::load(parseString());
            albedo = std::make_unique<Object::Albedo::Texture>(std::move(texture));
        } else {
            throwUnexpected();
        }

        expectRightBrace();

        return albedo;
    }

    bool SceneParser::tryParseBrdfs(std::vector<std::unique_ptr<Object::Brdf::Base>> &brdfs, float &transmitIor)
    {
        if(!matchLiteral("brdf")) {
            return false;
        }

        expectLeftBrace();

        while(!matchRightBrace()) {
            if(matchLiteral("lambert")) {
                float strength = parseFloat();
                brdfs.push_back(std::make_unique<Object::Brdf::Lambert>(strength));
                continue;
            }
    
            if(matchLiteral("phong")) {
                float strength = parseFloat();
                float power = parseFloat();
                brdfs.push_back(std::make_unique<Object::Brdf::Phong>(strength, power));
                continue;
            }

            if(matchLiteral("oren_nayar")) {
                float strength = parseFloat();
                float roughness = parseFloat();
                brdfs.push_back(std::make_unique<Object::Brdf::OrenNayar>(strength, roughness));
                continue;
            }
    
            if(matchLiteral("torrance_sparrow")) {
                float strength = parseFloat();
                float roughness = parseFloat();
                float ior = parseFloat();
                brdfs.push_back(std::make_unique<Object::Brdf::TorranceSparrow>(strength, roughness, ior));
                continue;
            }

            if(matchLiteral("transmit")) {
                transmitIor = parseFloat();
                continue;
            }

            throwUnexpected();
        }

        return true;
    }

    std::unique_ptr<Object::NormalMap> SceneParser::tryParseNormalMap()
    {
        if(!matchLiteral("normal_map")) {
            return nullptr;
        }
       
        auto texture = BmpLoader::load(parseString());
        float magnitude = parseFloat();

        return std::make_unique<Object::NormalMap>(std::move(texture), magnitude);
    }
}