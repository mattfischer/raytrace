# Created by and for Qt Creator. This file was created for editing the project sources only.
# You may attempt to use it for building too, by modifying this file here.

QT = core gui widgets

HEADERS = \
    Lighter/Base.hpp \
    Lighter/IndirectCached.hpp \
    Lighter/IrradianceCache.hpp \
    Lighter/UniPath.hpp \
    Math/Beam.hpp \
    Math/Bivector.hpp \
    Math/Bivector2D.hpp \
    Math/Matrix.hpp \
    Math/Normal.hpp \
    Math/OrthonormalBasis.hpp \
    Math/Point.hpp \
    Math/Point2D.hpp \
    Math/Ray.hpp \
    Math/Transformation.hpp \
    Math/Vector.hpp \
    Math/Vector2D.hpp \
    Object/Albedo/Base.hpp \
    Object/Albedo/Solid.hpp \
    Object/Albedo/Texture.hpp \
    Object/Brdf/Base.hpp \
    Object/Brdf/Composite.hpp \
    Object/Brdf/Lambert.hpp \
    Object/Brdf/OrenNayar.hpp \
    Object/Brdf/Phong.hpp \
    Object/Brdf/TorranceSparrow.hpp \
    Object/Shape/Base.hpp \
    Object/Shape/BezierPatch.hpp \
    Object/Shape/Grid.hpp \
    Object/Shape/Group.hpp \
    Object/Shape/Quad.hpp \
    Object/Shape/Sphere.hpp \
    Object/Shape/Transformed.hpp \
    Object/Shape/Triangle.hpp \
    Object/Shape/TriangleMesh.hpp \
    Object/BoundingVolume.hpp \
    Object/BoundingVolumeHierarchy.hpp \
    Object/Camera.hpp \
    Object/Color.hpp \
    Object/Intersection.hpp \
    Object/NormalMap.hpp \
    Object/PointLight.hpp \
    Object/Primitive.hpp \
    Object/Radiance.hpp \
    Object/Scene.hpp \
    Object/Surface.hpp \
    Object/Texture.hpp \
    Parse/AST.h \
    Parse/BmpLoader.hpp \
    Parse/BptLoader.hpp \
    Parse/BvhFile.hpp \
    Parse/Parser.hpp \
    Parse/PlyLoader.hpp \
    Render/Engine.hpp \
    Render/Framebuffer.hpp \
    Render/Job.hpp \
    Render/Raster.hpp \
    Render/RenderJob.hpp \
    Render/Sampler.hpp \
    Render/Settings.hpp \
    Render/TileJob.hpp \
    Render/TileJobSimple.hpp \
    App/MainWindow.hpp

SOURCES = \
    Lighter/Base.cpp \
    Lighter/IndirectCached.cpp \
    Lighter/IrradianceCache.cpp \
    Lighter/UniPath.cpp \
    Math/Beam.cpp \
    Math/Bivector.cpp \
    Math/Bivector2D.cpp \
    Math/Matrix.cpp \
    Math/Normal.cpp \
    Math/OrthonormalBasis.cpp \
    Math/Point.cpp \
    Math/Point2D.cpp \
    Math/Ray.cpp \
    Math/Transformation.cpp \
    Math/Vector.cpp \
    Math/Vector2D.cpp \
    Object/Albedo/Solid.cpp \
    Object/Albedo/Texture.cpp \
    Object/Brdf/Base.cpp \
    Object/Brdf/Composite.cpp \
    Object/Brdf/Lambert.cpp \
    Object/Brdf/OrenNayar.cpp \
    Object/Brdf/Phong.cpp \
    Object/Brdf/TorranceSparrow.cpp \
    Object/Shape/BezierPatch.cpp \
    Object/Shape/Grid.cpp \
    Object/Shape/Group.cpp \
    Object/Shape/Quad.cpp \
    Object/Shape/Sphere.cpp \
    Object/Shape/Transformed.cpp \
    Object/Shape/Triangle.cpp \
    Object/Shape/TriangleMesh.cpp \
    Object/BoundingVolume.cpp \
    Object/BoundingVolumeHierarchy.cpp \
    Object/Camera.cpp \
    Object/Color.cpp \
    Object/Intersection.cpp \
    Object/NormalMap.cpp \
    Object/PointLight.cpp \
    Object/Primitive.cpp \
    Object/Radiance.cpp \
    Object/Scene.cpp \
    Object/Surface.cpp \
    Object/Texture.cpp \
    Parse/BmpLoader.cpp \
    Parse/BptLoader.cpp \
    Parse/BvhFile.cpp \
    Parse/Parser.cpp \
    Parse/PlyLoader.cpp \
    Render/Engine.cpp \
    Render/Framebuffer.cpp \
    Render/Job.cpp \
    Render/RenderJob.cpp \
    Render/Sampler.cpp \
    Render/TileJob.cpp \
    Render/TileJobSimple.cpp \
    App/MainWindow.cpp \
    Main.cpp

LEXSOURCES = Parse/Scene.l
YACCSOURCES = Parse/Scene.y

INCLUDEPATH = \
    raytrace \
    Parse

CONFIG += object_parallel_to_source

FORMS += \
    App/MainWindow.ui
