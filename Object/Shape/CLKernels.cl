typedef struct {
    float mins[3];
    float maxes[3];
} BoundingVolume;

typedef struct {
    BoundingVolume volume;
    int index;
} BVHNode;

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

typedef struct {
    int vertices[3];
    Normal normal;
} Triangle;

typedef struct {
    Point *vertices;
    Triangle *triangles;
    BVHNode *bvh;
} ShapeTriangleMesh;

typedef struct {
    Point point;
    Normal normal;
} GridVertex;

typedef struct {
    int width;
    int height;
    GridVertex *vertices;
    BVHNode *bvh;
} ShapeGrid;

struct _Shape;
typedef struct _Shape Shape;

typedef struct {
    int numShapes;
    Shape *shapes;
    BoundingVolume *volumes;
} ShapeGroup;

typedef enum {
    ShapeTypeNone,
    ShapeTypeQuad,
    ShapeTypeSphere,
    ShapeTypeTriangleMesh,
    ShapeTypeGrid,
    ShapeTypeGroup
} ShapeType;

struct _Shape {
    ShapeType type;
    Transformation *transformation;
    union {
        ShapeQuad quad;
        ShapeSphere sphere;
        ShapeTriangleMesh triangleMesh;
        ShapeGrid grid;
        ShapeGroup group;
    };
};

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

bool Triangle_intersect(Ray *ray, Point p, Point pu, Point pv, float *distance)
{
    Vector E1 = pu - p;
    Vector E2 = pv - p;
    Vector P = cross(ray->direction, E2);

    float den = dot(P, E1);
    if(den > -1.0e-10f && den < 1.0e-10f) {
        return false;
    }

    float iden = 1.0f / den;
    Vector T = ray->origin - p;
    float uu = dot(P, T) * iden;
    if(uu < 0 || uu > 1) {
        return false;
    }

    Vector Q = cross(T, E1);
    float vv = dot(Q, ray->direction) * iden;
    if(vv < 0 || uu + vv > 1) {
        return false;
    }

    float d = dot(Q, E2) * iden;
    if(d < 0 || d >= *distance) {
        return false;
    }

    *distance = d;
    return true;
}

bool BoundingVolume_intersect(BoundingVolume *volume, Ray *ray, float *minDistance, float *maxDistance)
{
    float currentMinDist = -MAXFLOAT;
    float currentMaxDist = MAXFLOAT;

    float offsets[3];
    float dots[3];

    offsets[0] = ray->origin.x; offsets[1] = ray->origin.y; offsets[2] = ray->origin.z;
    dots[0] = ray->direction.x; dots[1] = ray->direction.y; dots[2] = ray->direction.z;

    for(int i=0; i<3; i++) {
        float offset = offsets[i];
        float dt = dots[i];

        float mn;
        float mx;

        if(dt < 0) {
            mx = (volume->mins[i] - offset) / dt;
            mn = (volume->maxes[i] - offset) / dt;
        } else if(dt == 0) {
            if(offset > volume->maxes[i] || offset < volume->mins[i]) {
                return false;
            } else {
                continue;
            }
        } else {
            mn = (volume->mins[i] - offset) / dt;
            mx = (volume->maxes[i] - offset) / dt;
        }

        currentMinDist = max(currentMinDist, mn);
        currentMaxDist = min(currentMaxDist, mx);

        if(currentMinDist > currentMaxDist || currentMaxDist < 0) {
            return false;
        }
    }

    if(currentMinDist > currentMaxDist || currentMaxDist < 0) {
        return false;
    }
    
    *minDistance = currentMinDist;
    *maxDistance = currentMaxDist;
    return true;
}

typedef struct {
    int nodeIndex;
    float minDistance;
} StackEntry;

bool ShapeTriangleMesh_intersect(Ray *ray, ShapeTriangleMesh *triangleMesh, ShapeIntersection *shapeIntersection)
{
    StackEntry stack[64];

    bool ret = false;
    int n = 0;
    stack[n].nodeIndex = 0;
    stack[n].minDistance = 0;
    n++;

    do {
        n--;
        int nodeIndex = stack[n].nodeIndex;
        BVHNode *bvhNode = &triangleMesh->bvh[nodeIndex];
        float nodeMinimum = stack[n].minDistance;

        if(nodeMinimum > shapeIntersection->distance) {
            continue;
        }

        if(bvhNode->index <= 0) {
            int index = -bvhNode->index;
            Triangle *triangle = &triangleMesh->triangles[index];
            Point vertex0 = triangleMesh->vertices[triangle->vertices[0]];
            Point vertex1 = triangleMesh->vertices[triangle->vertices[1]];
            Point vertex2 = triangleMesh->vertices[triangle->vertices[2]];

            if(Triangle_intersect(ray, vertex0, vertex1, vertex2, &shapeIntersection->distance)) {
                shapeIntersection->normal = triangle->normal;
                ret = true;
            }
        } else {
            int indices[2] = { nodeIndex + 1, bvhNode->index };
            float minDistances[2];
            float maxDistances[2];
            for(int i=0; i<2; i++) {
                minDistances[i] = MAXFLOAT;
                maxDistances[i] = -MAXFLOAT;
                BoundingVolume_intersect(&bvhNode->volume, ray, &minDistances[i], &maxDistances[i]);
            }

            for(int i=0; i<2; i++) {
                int j = (minDistances[0] >= minDistances[1]) ? i : 1 - i;
                if(maxDistances[j] > 0) {
                    stack[n].nodeIndex = indices[j];
                    stack[n].minDistance = minDistances[j];
                    n++;
                }
            }
        }
    } while(n > 0);

    return ret;
}

bool ShapeGrid_intersect(Ray *ray, ShapeGrid *grid, ShapeIntersection *shapeIntersection)
{
    StackEntry stack[64];

    bool ret = false;
    int n = 0;
    stack[n].nodeIndex = 0;
    stack[n].minDistance = 0;
    n++;

    do {
        n--;
        int nodeIndex = stack[n].nodeIndex;
        BVHNode *bvhNode = &grid->bvh[nodeIndex];
        float nodeMinimum = stack[n].minDistance;

        if(nodeMinimum > shapeIntersection->distance) {
            continue;
        }

        if(bvhNode->index <= 0) {
            int index = -bvhNode->index;
            unsigned int u = index % grid->width;
            unsigned int v = index / grid->width;
            GridVertex *vertex0 = &grid->vertices[v * grid->width + u];
            GridVertex *vertex1 = &grid->vertices[v * grid->width + u + 1];
            GridVertex *vertex2 = &grid->vertices[(v + 1) * grid->width + u];
            GridVertex *vertex3 = &grid->vertices[(v + 1) * grid->width + u + 1];

            if(Triangle_intersect(ray, vertex0->point, vertex1->point, vertex2->point, &shapeIntersection->distance)) {
                shapeIntersection->normal = vertex0->normal;
                ret = true;
            }
            if(Triangle_intersect(ray, vertex3->point, vertex2->point, vertex1->point, &shapeIntersection->distance)) {
                shapeIntersection->normal = vertex2->normal;
                ret = true;
            }
        } else {
            int indices[2] = { nodeIndex + 1, bvhNode->index };
            float minDistances[2];
            float maxDistances[2];
            for(int i=0; i<2; i++) {
                minDistances[i] = MAXFLOAT;
                maxDistances[i] = -MAXFLOAT;
                BoundingVolume_intersect(&bvhNode->volume, ray, &minDistances[i], &maxDistances[i]);
            }

            for(int i=0; i<2; i++) {
                int j = (minDistances[0] >= minDistances[1]) ? i : 1 - i;
                if(maxDistances[j] > 0) {
                    stack[n].nodeIndex = indices[j];
                    stack[n].minDistance = minDistances[j];
                    n++;
                }
            }
        }
    } while(n > 0);

    return ret;
}

bool Shape_intersect(Ray *ray, Shape *shape, ShapeIntersection *shapeIntersection);

bool ShapeGroup_intersect(Ray *ray, ShapeGroup *group, ShapeIntersection *shapeIntersection)
{
    bool ret = false;

    for(int i=0; i<group->numShapes; i++) {
        float minDist;
        float maxDist;
        if(BoundingVolume_intersect(&group->volumes[i], ray, &minDist, &maxDist) && minDist < shapeIntersection->distance) {
            if(Shape_intersect(ray, &group->shapes[i], shapeIntersection)) {
                ret = true;
            }
        }
    }

    return ret;
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

    case ShapeTypeTriangleMesh:
        result = ShapeTriangleMesh_intersect(&rayTrans, &shape->triangleMesh, shapeIntersection);
        break;

    case ShapeTypeGrid:
        result = ShapeGrid_intersect(&rayTrans, &shape->grid, shapeIntersection);
        break;

    case ShapeTypeGroup:
        result = ShapeGroup_intersect(&rayTrans, &shape->group, shapeIntersection);
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
