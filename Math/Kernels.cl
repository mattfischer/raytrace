typedef float3 Point;
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
