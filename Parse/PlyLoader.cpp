#include "Parse/PlyLoader.hpp"

#include "Parse/BvhFile.hpp"

#include "Object/Impl/Shape/TriangleMesh.hpp"

#include "Math/Point.hpp"

#include <fstream>
#include <string>

namespace Parse {
    struct Property {
        std::string name;
        std::string type;
        std::string listType;
    };
    struct Element {
        std::string name;
        int count;
        std::vector<Property> properties;
    };

    std::vector<Element> parseElements(std::ifstream &file)
    {
        std::vector<Element> elements;
        Element newElement;
        std::string keyword;
        file >> keyword;

        while (keyword != "end_header") {
            if (keyword == "comment") {
                std::string line;
                std::getline(file, line);
                file >> keyword;
                continue;
            }
            else if (keyword == "element") {
                file >> newElement.name >> newElement.count;

                file >> keyword;
                while (keyword == "property") {
                    Property property;
                    file >> property.type;
                    if (property.type == "list") {
                        std::string indexType;
                        file >> indexType >> property.listType;
                    }
                    file >> property.name;
                    newElement.properties.push_back(std::move(property));
                    file >> keyword;
                }
                elements.push_back(std::move(newElement));
            }
        }

        return elements;
    }

    void skipProperty(std::ifstream &file, const Property &property) {
        std::string text;
        if (property.type == "list") {
            int count;
            file >> count;
            for (int i = 0; i < count; i++) {
                file >> text;
            }
        }
        else {
            file >> text;
        }
    }

    void skipElement(std::ifstream &file, const Element &element) {
        for (int i = 0; i < element.count; i++) {
            for (const Property &property : element.properties) {
                skipProperty(file, property);
            }
        }
    }

    void loadPly(const std::string &filename, std::vector<Object::Impl::Shape::TriangleMesh::Vertex> &vertices, std::vector<Object::Impl::Shape::TriangleMesh::Triangle> &triangles)
    {
        std::ifstream file(filename.c_str());
        std::string ply;

        file >> ply;
        if (ply != "ply") {
            return;
        }

        std::string format, type, version;
        file >> format >> type >> version;
        if (format != "format" || type != "ascii") {
            return;
        }

        std::vector<Element> elements = parseElements(file);
        for (const Element &element : elements) {
            if (element.name == "vertex") {
                int idxX = -1;
                int idxY = -1;
                int idxZ = -1;
                for (int i = 0; i < element.properties.size(); i++) {
                    if (element.properties[i].name == "x") {
                        idxX = i;
                    }
                    else if (element.properties[i].name == "y") {
                        idxY = i;
                    }
                    else if (element.properties[i].name == "z") {
                        idxZ = i;
                    }
                }

                for (int i = 0; i < element.count; i++) {
                    float x = 0;
                    float y = 0;
                    float z = 0;
                    for (int j = 0; j < element.properties.size(); j++) {
                        if (j == idxX) {
                            file >> x;
                        }
                        else if (j == idxY) {
                            file >> y;
                        }
                        else if (j == idxZ) {
                            file >> z;
                        }
                        else {
                            skipProperty(file, element.properties[j]);
                        }
                    }

                    Object::Impl::Shape::TriangleMesh::Vertex vertex;
                    vertex.point = Math::Point(x, y, z);
                    vertices.push_back(vertex);
                }
            }
            else if (element.name == "face") {
                int idxVertices = -1;
                for (int i = 0; i < element.properties.size(); i++) {
                    if (element.properties[i].name == "vertex_indices") {
                        idxVertices = i;
                    }
                }

                for (int i = 0; i < element.count; i++) {
                    Object::Impl::Shape::TriangleMesh::Triangle triangle;
                    for (int j = 0; j < element.properties.size(); j++) {
                        if (j == idxVertices) {
                            int count;
                            file >> count;
                            for (int k = 0; k < count; k++) {
                                if (k < 3) {
                                    file >> triangle.vertices[k];
                                }
                                else {
                                    Math::Vector u = vertices[triangle.vertices[1]].point - vertices[triangle.vertices[0]].point;
                                    Math::Vector v = vertices[triangle.vertices[2]].point - vertices[triangle.vertices[0]].point;
                                    triangle.normal = Math::Normal(u % v).normalize();
                                    triangles.push_back(triangle);
                                    triangle.vertices[1] = triangle.vertices[2];
                                    file >> triangle.vertices[2];
                                }
                            }
                            Math::Vector u = vertices[triangle.vertices[1]].point - vertices[triangle.vertices[0]].point;
                            Math::Vector v = vertices[triangle.vertices[2]].point - vertices[triangle.vertices[0]].point;
                            triangle.normal = Math::Normal(u % v).normalize();
                            triangles.push_back(triangle);
                        }
                        else {
                            skipProperty(file, element.properties[j]);
                        }
                    }
                }
            }
            else {
                skipElement(file, element);
            }
        }
    }

    void loadBin(const std::string &filename, std::vector<Object::Impl::Shape::TriangleMesh::Vertex> &vertices, std::vector<Object::Impl::Shape::TriangleMesh::Triangle> &triangles)
    {
        std::ifstream file(filename.c_str(), std::ios_base::binary);

        std::size_t size;

        file.read((char*)&size, sizeof(size));
        vertices.resize(size);
        file.read((char*)&vertices[0], vertices.size() * sizeof(Object::Impl::Shape::TriangleMesh::Vertex));

        file.read((char*)&size, sizeof(size));
        triangles.resize(size);
        file.read((char*)&triangles[0], triangles.size() * sizeof(Object::Impl::Shape::TriangleMesh::Triangle));
    }

    void saveBin(const std::string &filename, const std::vector<Object::Impl::Shape::TriangleMesh::Vertex> &vertices, const std::vector<Object::Impl::Shape::TriangleMesh::Triangle> &triangles)
    {
        std::ofstream file(filename.c_str(), std::ios_base::binary);

        std::size_t size = vertices.size();
        file.write((const char *)&size, sizeof(size));
        file.write((const char*)&vertices[0], vertices.size() * sizeof(Object::Impl::Shape::TriangleMesh::Vertex));

        size = triangles.size();
        file.write((const char *)&size, sizeof(size));
        file.write((const char*)&triangles[0], triangles.size() * sizeof(Object::Impl::Shape::TriangleMesh::Triangle));
    }

    std::unique_ptr<Object::Shape> PlyLoader::load(const std::string &filename)
    {
        std::vector<Object::Impl::Shape::TriangleMesh::Vertex> vertices;
        std::vector<Object::Impl::Shape::TriangleMesh::Triangle> triangles;

        std::string binFilename = filename + ".bin";
        std::ifstream binFile(binFilename.c_str());
        if (binFile.good()) {
            loadBin(binFilename, vertices, triangles);
        }
        else {
            loadPly(filename, vertices, triangles);
            saveBin(binFilename, vertices, triangles);
        }

        std::string bvhFilename = filename + ".bvh";
        std::ifstream bvhFile(bvhFilename);
        if (bvhFile.good()) {
            Object::BoundingVolumeHierarchy boundingVolumeHierarchy = BvhFile::load(bvhFilename);
            return std::make_unique<Object::Impl::Shape::TriangleMesh>(std::move(vertices), std::move(triangles), std::move(boundingVolumeHierarchy));
        }
        else {
            std::unique_ptr<Object::Impl::Shape::TriangleMesh> mesh = std::make_unique<Object::Impl::Shape::TriangleMesh>(std::move(vertices), std::move(triangles));
            BvhFile::save(bvhFilename, mesh->boundingVolumeHierarchy());
            return std::move(mesh);
        }

    }

}
