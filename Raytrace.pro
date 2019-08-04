# Created by and for Qt Creator. This file was created for editing the project sources only.
# You may attempt to use it for building too, by modifying this file here.

#TARGET = Raytrace

HEADERS = \
   $$PWD/raytrace/Lighter/Base.hpp \
   $$PWD/raytrace/Lighter/DiffuseIndirect.hpp \
   $$PWD/raytrace/Lighter/Direct.hpp \
   $$PWD/raytrace/Lighter/IrradianceCache.hpp \
   $$PWD/raytrace/Lighter/Master.hpp \
   $$PWD/raytrace/Lighter/RadianceGradient.hpp \
   $$PWD/raytrace/Lighter/Radiant.hpp \
   $$PWD/raytrace/Lighter/Specular.hpp \
   $$PWD/raytrace/Lighter/Transmit.hpp \
   $$PWD/raytrace/Math/Beam.hpp \
   $$PWD/raytrace/Math/Bivector.hpp \
   $$PWD/raytrace/Math/Bivector2D.hpp \
   $$PWD/raytrace/Math/Matrix.hpp \
   $$PWD/raytrace/Math/Normal.hpp \
   $$PWD/raytrace/Math/OrthonormalBasis.hpp \
   $$PWD/raytrace/Math/Point.hpp \
   $$PWD/raytrace/Math/Point2D.hpp \
   $$PWD/raytrace/Math/Ray.hpp \
   $$PWD/raytrace/Math/Transformation.hpp \
   $$PWD/raytrace/Math/Vector.hpp \
   $$PWD/raytrace/Math/Vector2D.hpp \
   $$PWD/raytrace/Object/Albedo/Base.hpp \
   $$PWD/raytrace/Object/Albedo/Solid.hpp \
   $$PWD/raytrace/Object/Albedo/Texture.hpp \
   $$PWD/raytrace/Object/Brdf/Base.hpp \
   $$PWD/raytrace/Object/Brdf/Composite.hpp \
   $$PWD/raytrace/Object/Brdf/Lambert.hpp \
   $$PWD/raytrace/Object/Brdf/OrenNayar.hpp \
   $$PWD/raytrace/Object/Brdf/Phong.hpp \
   $$PWD/raytrace/Object/Brdf/TorranceSparrow.hpp \
   $$PWD/raytrace/Object/Shape/Base.hpp \
   $$PWD/raytrace/Object/Shape/BezierPatch.hpp \
   $$PWD/raytrace/Object/Shape/Grid.hpp \
   $$PWD/raytrace/Object/Shape/Group.hpp \
   $$PWD/raytrace/Object/Shape/Quad.hpp \
   $$PWD/raytrace/Object/Shape/Sphere.hpp \
   $$PWD/raytrace/Object/Shape/Transformed.hpp \
   $$PWD/raytrace/Object/Shape/Triangle.hpp \
   $$PWD/raytrace/Object/Shape/TriangleMesh.hpp \
   $$PWD/raytrace/Object/BoundingVolume.hpp \
   $$PWD/raytrace/Object/BoundingVolumeHierarchy.hpp \
   $$PWD/raytrace/Object/Camera.hpp \
   $$PWD/raytrace/Object/Color.hpp \
   $$PWD/raytrace/Object/Intersection.hpp \
   $$PWD/raytrace/Object/NormalMap.hpp \
   $$PWD/raytrace/Object/PointLight.hpp \
   $$PWD/raytrace/Object/Primitive.hpp \
   $$PWD/raytrace/Object/Radiance.hpp \
   $$PWD/raytrace/Object/Scene.hpp \
   $$PWD/raytrace/Object/Surface.hpp \
   $$PWD/raytrace/Object/Texture.hpp \
   $$PWD/raytrace/Parse/AST.h \
   $$PWD/raytrace/Parse/BmpLoader.hpp \
   $$PWD/raytrace/Parse/BptLoader.hpp \
   $$PWD/raytrace/Parse/BvhFile.hpp \
   $$PWD/raytrace/Parse/Parser.hpp \
   $$PWD/raytrace/Parse/PlyLoader.hpp \
   $$PWD/raytrace/Render/Engine.hpp \
   $$PWD/raytrace/Render/Framebuffer.hpp \
   $$PWD/raytrace/Render/Job.hpp \
   $$PWD/raytrace/Render/Raster.hpp \
   $$PWD/raytrace/Render/RenderJob.hpp \
   $$PWD/raytrace/Render/Sampler.hpp \
   $$PWD/raytrace/Render/Settings.hpp \
   $$PWD/raytrace/Render/TileJob.hpp \
   $$PWD/raytrace/Render/TileJobSimple.hpp \
   $$PWD/raytrace/App.hpp \
   $$PWD/raytrace/LightProbeDlg.hpp \
   $$PWD/raytrace/RenderControlDlg.hpp \
   $$PWD/raytrace/resource.h

SOURCES = \
   $$PWD/raytrace/Lighter/Base.cpp \
   $$PWD/raytrace/Lighter/DiffuseIndirect.cpp \
   $$PWD/raytrace/Lighter/Direct.cpp \
   $$PWD/raytrace/Lighter/IrradianceCache.cpp \
   $$PWD/raytrace/Lighter/Master.cpp \
   $$PWD/raytrace/Lighter/RadianceGradient.cpp \
   $$PWD/raytrace/Lighter/Radiant.cpp \
   $$PWD/raytrace/Lighter/Specular.cpp \
   $$PWD/raytrace/Lighter/Transmit.cpp \
   $$PWD/raytrace/Math/Beam.cpp \
   $$PWD/raytrace/Math/Bivector.cpp \
   $$PWD/raytrace/Math/Bivector2D.cpp \
   $$PWD/raytrace/Math/Matrix.cpp \
   $$PWD/raytrace/Math/Normal.cpp \
   $$PWD/raytrace/Math/OrthonormalBasis.cpp \
   $$PWD/raytrace/Math/Point.cpp \
   $$PWD/raytrace/Math/Point2D.cpp \
   $$PWD/raytrace/Math/Ray.cpp \
   $$PWD/raytrace/Math/Transformation.cpp \
   $$PWD/raytrace/Math/Vector.cpp \
   $$PWD/raytrace/Math/Vector2D.cpp \
   $$PWD/raytrace/Object/Albedo/Solid.cpp \
   $$PWD/raytrace/Object/Albedo/Texture.cpp \
   $$PWD/raytrace/Object/Brdf/Base.cpp \
   $$PWD/raytrace/Object/Brdf/Composite.cpp \
   $$PWD/raytrace/Object/Brdf/Lambert.cpp \
   $$PWD/raytrace/Object/Brdf/OrenNayar.cpp \
   $$PWD/raytrace/Object/Brdf/Phong.cpp \
   $$PWD/raytrace/Object/Brdf/TorranceSparrow.cpp \
   $$PWD/raytrace/Object/Shape/BezierPatch.cpp \
   $$PWD/raytrace/Object/Shape/Grid.cpp \
   $$PWD/raytrace/Object/Shape/Group.cpp \
   $$PWD/raytrace/Object/Shape/Quad.cpp \
   $$PWD/raytrace/Object/Shape/Sphere.cpp \
   $$PWD/raytrace/Object/Shape/Transformed.cpp \
   $$PWD/raytrace/Object/Shape/Triangle.cpp \
   $$PWD/raytrace/Object/Shape/TriangleMesh.cpp \
   $$PWD/raytrace/Object/BoundingVolume.cpp \
   $$PWD/raytrace/Object/BoundingVolumeHierarchy.cpp \
   $$PWD/raytrace/Object/Camera.cpp \
   $$PWD/raytrace/Object/Color.cpp \
   $$PWD/raytrace/Object/Intersection.cpp \
   $$PWD/raytrace/Object/NormalMap.cpp \
   $$PWD/raytrace/Object/PointLight.cpp \
   $$PWD/raytrace/Object/Primitive.cpp \
   $$PWD/raytrace/Object/Radiance.cpp \
   $$PWD/raytrace/Object/Scene.cpp \
   $$PWD/raytrace/Object/Surface.cpp \
   $$PWD/raytrace/Object/Texture.cpp \
   $$PWD/raytrace/Parse/BmpLoader.cpp \
   $$PWD/raytrace/Parse/BptLoader.cpp \
   $$PWD/raytrace/Parse/BvhFile.cpp \
   $$PWD/raytrace/Parse/Parser.cpp \
   $$PWD/raytrace/Parse/PlyLoader.cpp \
   $$PWD/raytrace/Render/Engine.cpp \
   $$PWD/raytrace/Render/Framebuffer.cpp \
   $$PWD/raytrace/Render/Job.cpp \
   $$PWD/raytrace/Render/RenderJob.cpp \
   $$PWD/raytrace/Render/Sampler.cpp \
   $$PWD/raytrace/Render/TileJob.cpp \
   $$PWD/raytrace/Render/TileJobSimple.cpp \
   $$PWD/raytrace/App.cpp \
   $$PWD/raytrace/LightProbeDlg.cpp \
   $$PWD/raytrace/Main.cpp \
   $$PWD/raytrace/RenderControlDlg.cpp

LEXSOURCES = $$PWD/raytrace/Parse/Scene.l
YACCSOURCES = $$PWD/raytrace/Parse/Scene.y

INCLUDEPATH = \
    $$PWD/raytrace \
    $$PWD/raytrace/Parse

RC_FILE = $$PWD/raytrace/resources.rc

DEFINES -= UNICODE

CONFIG += object_parallel_to_source
LIBS += -luser32 -lgdi32 -lkernel32 -lcomctl32
