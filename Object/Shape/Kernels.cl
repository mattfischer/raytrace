typedef struct {
    Point position;
    Vector side1;
    Vector side2;
    Normal normal;
} QuadShape;

typedef struct {
    Point position;
    float radius;
} SphereShape;

typedef enum {
    ShapeTypeNone,
    ShapeTypeQuad,
    ShapeTypeSphere
} ShapeType;

typedef struct {
    ShapeType type;
    union {
        QuadShape quad;
        SphereShape sphere;
    };
} Shape;

typedef struct {
    float distance;
    Normal normal;
} ShapeIntersection;

float length2(float3 v)
{
    return dot(v, v);
}

bool intersectQuad(Ray *ray, QuadShape *quad, ShapeIntersection *shapeIntersection)
{
    float distance = dot(ray->origin - quad->position, quad->normal) / dot(ray->direction, -quad->normal);
    if(distance >= 0 && distance < shapeIntersection->distance) {
        Point point = ray->origin + ray->direction * distance;
        float u = dot(point - quad->position, quad->side1) / length2(quad->side1);
        float v = dot(point - quad->position, quad->side2) / length2(quad->side2);
        if(u >= 0 && u <= 1 && v >= 0 && v <= 1) {
            shapeIntersection->distance = distance;
            shapeIntersection->normal = quad->normal;
            return true;
        } 
    }

    return false;
}

bool intersectSphere(Ray *ray, SphereShape *sphere, ShapeIntersection *shapeIntersection)
{
    float a, b, c;
    float disc;

    a = length2(ray->direction);
    b = 2 * dot(ray->origin - sphere->position, ray->direction);
    c = length2(ray->origin - sphere->position) - sphere->radius * sphere->radius;

    disc = b * b - 4 * a * c;
    if(disc >= 0) {
        float distance = (-b - sqrt(disc)) / (2 * a);
        if(distance >= 0 && distance < shapeIntersection->distance) {
            shapeIntersection->distance = distance;
            Point point = ray->origin + ray->direction * distance;
            shapeIntersection->normal = (point - sphere->position) / sphere->radius;
            return true;
        }

        distance = (-b + sqrt(disc)) / (2 * a);
        if(distance >= 0 && distance < shapeIntersection->distance) {
            shapeIntersection->distance = distance;
            Point point = ray->origin + ray->direction * distance;
            shapeIntersection->normal = (point - sphere->position) / sphere->radius;
            return true;
        }
    }

    return false;
}

bool intersectShape(Ray *ray, Shape *shape, ShapeIntersection *shapeIntersection)
{
    switch(shape->type) {
    case ShapeTypeQuad:
        return intersectQuad(ray, &shape->quad, shapeIntersection);
    
    case ShapeTypeSphere:
        return intersectSphere(ray, &shape->sphere, shapeIntersection);
    
    default:
        return false;
    }
}

float shapeSamplePdfQuad(QuadShape *quad, Point point)
{
    float surfaceArea = length(cross(quad->side1, quad->side2));
    return 1.0f / surfaceArea;
}

float shapeSamplePdf(Shape *shape, Point point)
{
    switch(shape->type) {
    case ShapeTypeQuad:
        return shapeSamplePdfQuad(&shape->quad, point);
    
    default:
        return 0;
    }
}

bool shapeSampleQuad(QuadShape *quad, float2 random, Point *point, Normal *normal, float *pdf)
{
    *point = quad->position + quad->side1 * random.x + quad->side2 * random.y;
    *normal = quad->normal;

    float surfaceArea = length(cross(quad->side1, quad->side2));
    *pdf = 1.0f / surfaceArea;

    return true;
}

bool shapeSample(Shape *shape, float2 random, Point *point, Normal *normal, float *pdf)
{
    switch(shape->type) {
    case ShapeTypeQuad:
        return shapeSampleQuad(&shape->quad, random, point, normal, pdf);
    
    default:
        return false;
    }
}
