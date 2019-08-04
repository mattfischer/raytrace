# Created by and for Qt Creator. This file was created for editing the project sources only.
# You may attempt to use it for building too, by modifying this file here.

#TARGET = Raytrace

HEADERS = \
   $$PWD/Lighter/Base.hpp \
   $$PWD/Lighter/DiffuseIndirect.hpp \
   $$PWD/Lighter/Direct.hpp \
   $$PWD/Lighter/IrradianceCache.hpp \
   $$PWD/Lighter/Master.hpp \
   $$PWD/Lighter/RadianceGradient.hpp \
   $$PWD/Lighter/Radiant.hpp \
   $$PWD/Lighter/Specular.hpp \
   $$PWD/Lighter/Transmit.hpp \
   $$PWD/Math/Beam.hpp \
   $$PWD/Math/Bivector.hpp \
   $$PWD/Math/Bivector2D.hpp \
   $$PWD/Math/Matrix.hpp \
   $$PWD/Math/Normal.hpp \
   $$PWD/Math/OrthonormalBasis.hpp \
   $$PWD/Math/Point.hpp \
   $$PWD/Math/Point2D.hpp \
   $$PWD/Math/Ray.hpp \
   $$PWD/Math/Transformation.hpp \
   $$PWD/Math/Vector.hpp \
   $$PWD/Math/Vector2D.hpp \
   $$PWD/Object/Albedo/Base.hpp \
   $$PWD/Object/Albedo/Solid.hpp \
   $$PWD/Object/Albedo/Texture.hpp \
   $$PWD/Object/Brdf/Base.hpp \
   $$PWD/Object/Brdf/Composite.hpp \
   $$PWD/Object/Brdf/Lambert.hpp \
   $$PWD/Object/Brdf/OrenNayar.hpp \
   $$PWD/Object/Brdf/Phong.hpp \
   $$PWD/Object/Brdf/TorranceSparrow.hpp \
   $$PWD/Object/Shape/Base.hpp \
   $$PWD/Object/Shape/BezierPatch.hpp \
   $$PWD/Object/Shape/Grid.hpp \
   $$PWD/Object/Shape/Group.hpp \
   $$PWD/Object/Shape/Quad.hpp \
   $$PWD/Object/Shape/Sphere.hpp \
   $$PWD/Object/Shape/Transformed.hpp \
   $$PWD/Object/Shape/Triangle.hpp \
   $$PWD/Object/Shape/TriangleMesh.hpp \
   $$PWD/Object/BoundingVolume.hpp \
   $$PWD/Object/BoundingVolumeHierarchy.hpp \
   $$PWD/Object/Camera.hpp \
   $$PWD/Object/Color.hpp \
   $$PWD/Object/Intersection.hpp \
   $$PWD/Object/NormalMap.hpp \
   $$PWD/Object/PointLight.hpp \
   $$PWD/Object/Primitive.hpp \
   $$PWD/Object/Radiance.hpp \
   $$PWD/Object/Scene.hpp \
   $$PWD/Object/Surface.hpp \
   $$PWD/Object/Texture.hpp \
   $$PWD/Parse/AST.h \
   $$PWD/Parse/BmpLoader.hpp \
   $$PWD/Parse/BptLoader.hpp \
   $$PWD/Parse/BvhFile.hpp \
   $$PWD/Parse/Parser.hpp \
   $$PWD/Parse/PlyLoader.hpp \
   $$PWD/Render/Engine.hpp \
   $$PWD/Render/Framebuffer.hpp \
   $$PWD/Render/Job.hpp \
   $$PWD/Render/Raster.hpp \
   $$PWD/Render/RenderJob.hpp \
   $$PWD/Render/Sampler.hpp \
   $$PWD/Render/Settings.hpp \
   $$PWD/Render/TileJob.hpp \
   $$PWD/Render/TileJobSimple.hpp \
   $$PWD/App.hpp \
   $$PWD/LightProbeDlg.hpp \
   $$PWD/RenderControlDlg.hpp \
   $$PWD/resource.h

SOURCES = \
   $$PWD/Lighter/Base.cpp \
   $$PWD/Lighter/DiffuseIndirect.cpp \
   $$PWD/Lighter/Direct.cpp \
   $$PWD/Lighter/IrradianceCache.cpp \
   $$PWD/Lighter/Master.cpp \
   $$PWD/Lighter/RadianceGradient.cpp \
   $$PWD/Lighter/Radiant.cpp \
   $$PWD/Lighter/Specular.cpp \
   $$PWD/Lighter/Transmit.cpp \
   $$PWD/Math/Beam.cpp \
   $$PWD/Math/Bivector.cpp \
   $$PWD/Math/Bivector2D.cpp \
   $$PWD/Math/Matrix.cpp \
   $$PWD/Math/Normal.cpp \
   $$PWD/Math/OrthonormalBasis.cpp \
   $$PWD/Math/Point.cpp \
   $$PWD/Math/Point2D.cpp \
   $$PWD/Math/Ray.cpp \
   $$PWD/Math/Transformation.cpp \
   $$PWD/Math/Vector.cpp \
   $$PWD/Math/Vector2D.cpp \
   $$PWD/Object/Albedo/Solid.cpp \
   $$PWD/Object/Albedo/Texture.cpp \
   $$PWD/Object/Brdf/Base.cpp \
   $$PWD/Object/Brdf/Composite.cpp \
   $$PWD/Object/Brdf/Lambert.cpp \
   $$PWD/Object/Brdf/OrenNayar.cpp \
   $$PWD/Object/Brdf/Phong.cpp \
   $$PWD/Object/Brdf/TorranceSparrow.cpp \
   $$PWD/Object/Shape/BezierPatch.cpp \
   $$PWD/Object/Shape/Grid.cpp \
   $$PWD/Object/Shape/Group.cpp \
   $$PWD/Object/Shape/Quad.cpp \
   $$PWD/Object/Shape/Sphere.cpp \
   $$PWD/Object/Shape/Transformed.cpp \
   $$PWD/Object/Shape/Triangle.cpp \
   $$PWD/Object/Shape/TriangleMesh.cpp \
   $$PWD/Object/BoundingVolume.cpp \
   $$PWD/Object/BoundingVolumeHierarchy.cpp \
   $$PWD/Object/Camera.cpp \
   $$PWD/Object/Color.cpp \
   $$PWD/Object/Intersection.cpp \
   $$PWD/Object/NormalMap.cpp \
   $$PWD/Object/PointLight.cpp \
   $$PWD/Object/Primitive.cpp \
   $$PWD/Object/Radiance.cpp \
   $$PWD/Object/Scene.cpp \
   $$PWD/Object/Surface.cpp \
   $$PWD/Object/Texture.cpp \
   $$PWD/Parse/BmpLoader.cpp \
   $$PWD/Parse/BptLoader.cpp \
   $$PWD/Parse/BvhFile.cpp \
   $$PWD/Parse/Parser.cpp \
   $$PWD/Parse/PlyLoader.cpp \
   $$PWD/Render/Engine.cpp \
   $$PWD/Render/Framebuffer.cpp \
   $$PWD/Render/Job.cpp \
   $$PWD/Render/RenderJob.cpp \
   $$PWD/Render/Sampler.cpp \
   $$PWD/Render/TileJob.cpp \
   $$PWD/Render/TileJobSimple.cpp \
   $$PWD/App.cpp \
   $$PWD/LightProbeDlg.cpp \
   $$PWD/Main.cpp \
   $$PWD/RenderControlDlg.cpp

LEXSOURCES = $$PWD/Parse/Scene.l
YACCSOURCES = $$PWD/Parse/Scene.y

INCLUDEPATH = \
    $$PWD/raytrace \
    $$PWD/Parse

RC_FILE = $$PWD/resources.rc

DEFINES -= UNICODE

CONFIG += object_parallel_to_source
LIBS += -luser32 -lgdi32 -lkernel32 -lcomctl32
