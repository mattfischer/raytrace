typedef struct {
    Point position;
    Vector side1;
    Vector side2;
    Normal normal;
} ShapeQuad;

typedef struct {
    Point position;
    float radius;
} ShapeSphere;

typedef enum {
    ShapeTypeNone,
    ShapeTypeQuad,
    ShapeTypeSphere,
} ShapeType;

typedef struct {
    ShapeType type;
    Transformation *transformation;
    union {
        ShapeQuad quad;
        ShapeSphere sphere;
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

bool ShapeQuad_intersect(Ray *ray, ShapeQuad *quad, ShapeIntersection *shapeIntersection)
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

float ShapeQuad_samplePdf(ShapeQuad *quad, Point point)
{
    float surfaceArea = length(cross(quad->side1, quad->side2));
    return 1.0f / surfaceArea;
}

bool ShapeQuad_sample(ShapeQuad *quad, float2 random, Point *point, Normal *normal, float *pdf)
{
    *point = quad->position + quad->side1 * random.x + quad->side2 * random.y;
    *normal = quad->normal;

    float surfaceArea = length(cross(quad->side1, quad->side2));
    *pdf = 1.0f / surfaceArea;

    return true;
}

bool ShapeSphere_intersect(Ray *ray, ShapeSphere *sphere, ShapeIntersection *shapeIntersection)
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

bool Shape_intersect(Ray *ray, Shape *shape, ShapeIntersection *shapeIntersection)
{
    Ray rayTrans;
    if(shape->transformation == NULL) {
        rayTrans.origin = ray->origin;
        rayTrans.direction = ray->direction;
    } else {
        rayTrans.origin = Matrix_multiplyPoint(&shape->transformation->inverseMatrix, &ray->origin);
        rayTrans.direction = Matrix_multiplyVector(&shape->transformation->inverseMatrix, &ray->direction);
    }

    bool result = false;

    switch(shape->type) {
    case ShapeTypeQuad:
        result = ShapeQuad_intersect(&rayTrans, &shape->quad, shapeIntersection);
        break;

    case ShapeTypeSphere:
        result = ShapeSphere_intersect(&rayTrans, &shape->sphere, shapeIntersection);
        break;

    default:
        break;
    }

    if(result && shape->transformation != NULL) {
        shapeIntersection->normal = normalize(Matrix_multiplyNormal(&shape->transformation->matrix, &shapeIntersection->normal));   
    }
    return result;
}

float Shape_samplePdf(Shape *shape, Point point)
{
    switch(shape->type) {
    case ShapeTypeQuad:
        return ShapeQuad_samplePdf(&shape->quad, point);
    
    default:
        return 0;
    }
}

bool Shape_sample(Shape *shape, float2 random, Point *point, Normal *normal, float *pdf)
{
    switch(shape->type) {
    case ShapeTypeQuad:
        return ShapeQuad_sample(&shape->quad, random, point, normal, pdf);
    
    default:
        return false;
    }
}
