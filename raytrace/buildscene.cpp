#include "buildscene.h"

#include "sphere.h"
#include "box.h"
#include "plane.h"
#include "cone.h"
#include "disk.h"
#include "cylinder.h"

#include "transformation.h"

#include "texture.h"

Scene *buildScene(int screenX, int screenY)
{
	Scene *scene = new Scene;

	Camera *camera = new Camera(45, (double)screenY / (double)screenX);
	camera->transform(Transformation::translate(0, 0, -10));
	scene->setCamera(camera);

	Light *light = new Light(Color(1, 1, 1));
	light->transform(Transformation::translate(3, 2, -3));
	scene->addLight(light);

	Plane *plane = new Plane;
	plane->setTexture(new Texture(new PigmentChecker(Color(0, 0, 1), Color(1, 0, 0)), new Finish));
	plane->transform(Transformation::translate(0, -2, 0));
	scene->addPrimitive(plane);

	Box *box = new Box;
	box->setTexture(new Texture(new PigmentSolid(Color(0, 1, 0)), new Finish));
	box->transform(Transformation::translate(2, -1, 0));
	box->transform(Transformation::rotate(0, 35, 45));
	box->transform(Transformation::uniformScale(.5));
	
	scene->addPrimitive(box);

	Sphere *sphere1 = new Sphere;
	sphere1->setTexture(new Texture(new PigmentSolid(Color(1, 0, 0)), new Finish));
	sphere1->transform(Transformation::translate(2, 1, 0));
	sphere1->transform(Transformation::uniformScale(.5));
	scene->addPrimitive(sphere1);

	Sphere *sphere2 = new Sphere;
	sphere2->setTexture(new Texture(new PigmentSolid(Color(1, 0, 1)), new Finish));
	sphere2->texture()->finish()->setSpecular(1);
	sphere2->transform(Transformation::translate(-1, 0, 1));
	sphere2->transform(Transformation::uniformScale(2));
	scene->addPrimitive(sphere2);

	Cone *cone = new Cone;
	cone->setTexture(new Texture(new PigmentSolid(Color(1, 1, 0)), new Finish));
	cone->transform(Transformation::rotate(90, 0, 0));
	cone->transform(Transformation::scale(.5, .5, 2));
	cone->transform(Transformation::translate(0, -3, 0));
	scene->addPrimitive(cone);

	return scene;
}