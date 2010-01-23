#include "buildscene.h"

#include "sphere.h"
#include "box.h"
#include "plane.h"
#include "cone.h"
#include "disk.h"
#include "cylinder.h"
#include "csg.h"

#include "transformation.h"

#include "texture.h"

Scene *buildScene(int screenX, int screenY)
{
	Scene *scene = new Scene;

	Camera *camera = new Camera(45, (double)screenY / (double)screenX);
	camera->transform(Transformation::translate(1, 1, -10));
	camera->transform(Transformation::rotate(0, 10, 0));
	camera->transform(Transformation::rotate(10, 0, 0));
	scene->setCamera(camera);

	Light *light = new Light(Color(1, 1, 1));
	light->transform(Transformation::translate(3, 2, -3));
	scene->addLight(light);

	Plane *plane = new Plane;
	plane->setTexture(new Texture(new PigmentChecker(Color(0, 0, 1), Color(1, 0, 0)), new Finish));
	plane->transform(Transformation::translate(0, -2, 0));
	scene->addPrimitive(plane);

	Sphere *sphere1 = new Sphere;
	sphere1->setTexture(new Texture(new PigmentSolid(Color(1, 0, 0)), new Finish));
	sphere1->transform(Transformation::translate(.5, 0, 0));
	//sphere1->transform(Transformation::uniformScale(.5));
	//scene->addPrimitive(sphere1);

	Sphere *sphere2 = new Sphere;
	sphere2->setTexture(new Texture(new PigmentSolid(Color(1, 0, 1)), new Finish));
	sphere2->texture()->finish()->setReflection(.75);
	sphere2->texture()->finish()->setSpecular(1);
	sphere2->transform(Transformation::translate(-.5, 0, 0));
	//sphere2->transform(Transformation::uniformScale(2));
	//scene->addPrimitive(sphere2);

	Csg *csg = new Csg;
	csg->setPrimitive1(sphere2);
	csg->setPrimitive2(sphere1);
	csg->setType(Csg::TypeIntersection);
	csg->transform(Transformation::rotate(0, 45, 0));
	scene->addPrimitive(csg);

	return scene;
}