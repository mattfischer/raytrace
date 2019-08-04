#include "Parse/Parser.hpp"

#include "Object/Camera.hpp"
#include "Object/PointLight.hpp"
#include "Object/Primitive.hpp"

#include "Object/Albedo/Solid.hpp"
#include "Object/Albedo/Texture.hpp"

#include "Object/Brdf/Lambert.hpp"
#include "Object/Brdf/Phong.hpp"
#include "Object/Brdf/OrenNayar.hpp"
#include "Object/Brdf/TorranceSparrow.hpp"

#include "Object/Shape/Quad.hpp"
#include "Object/Shape/Sphere.hpp"
#include "Object/Shape/Transformed.hpp"

#include "Parse/BmpLoader.hpp"
#include "Parse/BptLoader.hpp"
#include "Parse/PlyLoader.hpp"

#include "AST.h"

AST *parseScene(const char *filename);

namespace Parse {
	Math::Point parsePoint(const ASTVector &v)
	{
		return Math::Point(v.x, v.y, v.z);
	}

	Math::Vector parseVector(const ASTVector &v)
	{
		return Math::Vector(v.x, v.y, v.z);
	}

	Object::Color parseColor(const ASTVector &v)
	{
		return Object::Color(v.x, v.y, v.z);
	}

	Object::Radiance parseRadiance(const ASTVector &v)
	{
		return Object::Radiance(v.x, v.y, v.z);
	}

	std::unique_ptr<Object::Camera> parseCamera(AST *ast)
	{
		Math::Point position = parsePoint(ast->children[0]->data._vector);
		Math::Point lookAt = parsePoint(ast->children[1]->data._vector);
		float focalLength = ast->children[2]->data._float;
		float apertureSize = ast->children[3]->data._float;

		std::unique_ptr<Object::Camera> camera = std::make_unique<Object::Camera>(position, (lookAt - position).normalize(), Math::Vector(0, 1, 0), 60.0f, focalLength, apertureSize);

		return camera;
	}

	std::unique_ptr<Object::PointLight> parsePointLight(AST *ast)
	{
		Math::Point position = parsePoint(ast->children[0]->children[0]->data._vector);
		Object::Radiance radiance = parseRadiance(ast->children[0]->children[1]->data._vector);
		std::unique_ptr<Object::PointLight> pointLight = std::make_unique<Object::PointLight>(position, radiance);

		return pointLight;
	}

	std::unique_ptr<Object::Shape::Base> parseShapeModel(AST *ast)
	{
		std::string filename(ast->children[0]->data._string);
		std::unique_ptr<Object::Shape::Base> model;
		std::string extension = filename.substr(filename.find_last_of('.'));
		if (extension == ".bpt") {
			model = BptLoader::load(filename);
		}
		else if (extension == ".ply") {
			model = PlyLoader::load(filename);
		}

		return model;
	}

	std::unique_ptr<Object::Shape::Quad> parseShapeQuad(AST *ast)
	{
		Math::Point position = parsePoint(ast->children[0]->children[0]->data._vector);
		Math::Vector side1 = parseVector(ast->children[0]->children[1]->data._vector);
		Math::Vector side2 = parseVector(ast->children[0]->children[2]->data._vector);

		std::unique_ptr<Object::Shape::Quad> quad = std::make_unique<Object::Shape::Quad>(position, side1, side2);

		return quad;
	}

	std::unique_ptr<Object::Shape::Sphere> parseShapeSphere(AST *ast)
	{
		Math::Point position = parsePoint(ast->children[0]->children[0]->data._vector);
		float radius = ast->children[0]->children[1]->data._float;
		std::unique_ptr<Object::Shape::Sphere> sphere = std::make_unique<Object::Shape::Sphere>(position, radius);

		return sphere;
	}

	std::unique_ptr<Object::Shape::Base> parseShape(AST *ast)
	{
		std::unique_ptr<Object::Shape::Base> shape;

		switch (ast->type)
		{
		case AstSphere:
			shape = parseShapeSphere(ast);
			break;
		case AstQuad:
			shape = parseShapeQuad(ast);
			break;
		case AstModel:
			shape = parseShapeModel(ast);
			break;
		}

		return shape;
	}

	std::unique_ptr<Object::Albedo::Solid> parseAlbedoSolid(AST *ast)
	{
		return std::make_unique<Object::Albedo::Solid>(parseColor(ast->children[0]->data._vector));
	}

	std::unique_ptr<Object::Albedo::Texture> parseAlbedoTexture(AST *ast)
	{
		std::string filename(ast->data._string);
		std::unique_ptr<Object::Texture<3>> texture = BmpLoader::load(filename);
		return std::make_unique<Object::Albedo::Texture>(std::move(texture));
	}

	std::unique_ptr<Object::Albedo::Base> parseAlbedo(AST *ast)
	{
		AST *albedoAst = ast->children[0];
		switch (albedoAst->type)
		{
		case AstAlbedoSolid:
			return parseAlbedoSolid(albedoAst);
		case AstAlbedoTexture:
			return parseAlbedoTexture(albedoAst);
		}

		return 0;
	}

	std::unique_ptr<Object::Brdf::Lambert> parseBrdfLambert(AST *ast)
	{
		return std::make_unique<Object::Brdf::Lambert>(ast->data._float);
	}

	std::unique_ptr<Object::Brdf::OrenNayar> parseBrdfOrenNayar(AST *ast)
	{
		float strength = ast->children[0]->data._float;
		float roughness = ast->children[1]->data._float;
		return std::make_unique<Object::Brdf::OrenNayar>(strength, roughness);
	}

	std::unique_ptr<Object::Brdf::Phong> parseBrdfPhong(AST *ast)
	{
		float strength = ast->children[0]->data._float;
		float power = ast->children[1]->data._float;
		return std::make_unique<Object::Brdf::Phong>(strength, power);
	}

	std::unique_ptr<Object::Brdf::TorranceSparrow> parseBrdfTorranceSparrow(AST *ast)
	{
		float strength = ast->children[0]->data._float;
		float roughness = ast->children[1]->data._float;
		float ior = ast->children[2]->data._float;
		return std::make_unique<Object::Brdf::TorranceSparrow>(strength, roughness, ior);
	}

	std::unique_ptr<Object::Brdf::Composite> parseBrdfComposite(AST *ast)
	{
		std::unique_ptr<Object::Brdf::Base> diffuse;
		std::unique_ptr<Object::Brdf::Base> specular;
		float transmitIor = 0;

		for (int i = 0; i<ast->numChildren; i++) {
			switch (ast->children[i]->type) {
			case AstBrdfLambert:
				diffuse = parseBrdfLambert(ast->children[i]);
				break;
			case AstBrdfOrenNayar:
				diffuse = parseBrdfOrenNayar(ast->children[i]);
				break;
			case AstBrdfPhong:
				specular = parseBrdfPhong(ast->children[i]);
				break;
			case AstBrdfTorranceSparrow:
				specular = parseBrdfTorranceSparrow(ast->children[i]);
				break;
			case AstBrdfTransmit:
				transmitIor = ast->children[i]->data._float;
				break;
			}
		}

		return std::make_unique<Object::Brdf::Composite>(std::move(diffuse), std::move(specular), transmitIor);
	}

	std::unique_ptr<Object::NormalMap> parseNormalMap(AST *ast)
	{
		std::string filename(ast->children[0]->data._string);
		std::unique_ptr<Object::Texture<3>> texture = BmpLoader::load(filename);
		return std::make_unique<Object::NormalMap>(std::move(texture), ast->children[1]->data._float);
	}

	std::unique_ptr<Object::Surface> parseSurface(AST *ast)
	{
		std::unique_ptr<Object::Albedo::Base> albedo;
		std::unique_ptr<Object::Brdf::Composite> brdf;
		Object::Radiance radiance;
		std::unique_ptr<Object::NormalMap> normalMap;

		for (int i = 0; i < ast->numChildren; i++) {
			switch (ast->children[i]->type) {
			case AstAlbedo:
				albedo = parseAlbedo(ast->children[i]);
				break;

			case AstBrdf:
				brdf = parseBrdfComposite(ast->children[i]);
				break;

			case AstRadiance:
				radiance = parseRadiance(ast->children[i]->data._vector);
				break;

			case AstNormalMap:
				normalMap = parseNormalMap(ast->children[i]);
				break;
			}
		}

		return std::make_unique<Object::Surface>(std::move(albedo), std::move(brdf), radiance, std::move(normalMap));
	}

	Math::Transformation parseTransformation(AST *ast)
	{
		Math::Transformation t;

		for (int i = 0; i<ast->numChildren; i++)
		{
			switch (ast->children[i]->type)
			{
			case AstTranslate:
				t = Math::Transformation::translate(parseVector(ast->children[i]->data._vector)) * t;
				break;
			case AstRotate:
				t = Math::Transformation::rotate(parseVector(ast->children[i]->data._vector)) * t;
				break;
			case AstScale:
				t = Math::Transformation::scale(parseVector(ast->children[i]->data._vector)) * t;
				break;
			case AstUniformScale:
				t = Math::Transformation::uniformScale(ast->children[i]->data._float) * t;
				break;
			}
		}

		return t;
	}

	std::unique_ptr<Object::Primitive> parsePrimitive(AST *ast)
	{
		AST *sub = ast->children[0];
		std::unique_ptr<Object::Shape::Base> shape = parseShape(sub);

		std::unique_ptr<Object::Surface> surface;
		Math::Transformation transformation;

		AST *subsub = sub->children[1];
		for (int i = 0; i<subsub->numChildren; i++)
		{
			switch (subsub->children[i]->type)
			{
			case AstTransform:
				transformation = transformation * parseTransformation(subsub->children[i]);
				break;

			case AstSurface:
				surface = parseSurface(subsub->children[i]);
				break;
			}
		}

		if (!transformation.matrix().identity()) {
			shape = std::make_unique<Object::Shape::Transformed>(std::move(shape), transformation);
		}

		return std::make_unique<Object::Primitive>(std::move(shape), std::move(surface));
	}

	std::unique_ptr<Object::Scene> Parser::parse(const std::string &filename)
	{
		AST *ast = parseScene(filename.c_str());

		std::unique_ptr<Object::Camera> camera;
		std::vector<std::unique_ptr<Object::Primitive>> primitives;
		std::vector<std::unique_ptr<Object::PointLight>> pointLights;

		for (int i = 0; i<ast->numChildren; i++)
		{
			AST *child = ast->children[i];

			switch (child->type)
			{
			case AstPrimitive:
				primitives.push_back(parsePrimitive(child));
				break;
			case AstCamera:
				camera = parseCamera(child);
				break;
			case AstPointLight:
				pointLights.push_back(parsePointLight(child));
				break;
			}
		}

		std::unique_ptr<Object::Scene> scene = std::make_unique<Object::Scene>(std::move(camera), std::move(primitives), std::move(pointLights));

		return scene;
	}
}