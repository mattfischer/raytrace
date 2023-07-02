typedef float3 Point;
typedef float3 Vector;
typedef float3 Normal;

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
