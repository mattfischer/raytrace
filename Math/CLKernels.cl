typedef float3 Point;
typedef float2 Point2D;
typedef float3 Vector;
typedef float3 Normal;
typedef float3 Color;
typedef float3 Radiance;

typedef struct {
    Vector u;
    Vector v;
} Bivector;

typedef struct {
    Point origin;
    Vector direction;
} Ray;

typedef struct {
    Ray ray;
    Bivector originDifferential;
    Bivector directionDifferential;
} Beam;

typedef struct {
    float4 rows[4];
} Matrix;

typedef struct {
    Matrix matrix;
    Matrix inverseMatrix;
} Transformation;

float3 Matrix_multiplyPoint(Matrix *matrix, Point *point)
{
    float4 v = (float4)(*point, 1);
    return (float3)(dot(matrix->rows[0], v), dot(matrix->rows[1], v), dot(matrix->rows[2], v));
}

float3 Matrix_multiplyVector(Matrix *matrix, Vector *vector)
{
    float4 v = (float4)(*vector, 0);
    return (float3)(dot(matrix->rows[0], v), dot(matrix->rows[1], v), dot(matrix->rows[2], v));
}

float3 Matrix_multiplyNormal(Matrix *matrix, Normal *normal)
{
    float3 v = *normal;
    return (float3)(dot((float3)(matrix->rows[0].x, matrix->rows[1].x, matrix->rows[2].x), v),
                    dot((float3)(matrix->rows[0].y, matrix->rows[1].y, matrix->rows[2].y), v),
                    dot((float3)(matrix->rows[0].z, matrix->rows[1].z, matrix->rows[2].z), v));
}