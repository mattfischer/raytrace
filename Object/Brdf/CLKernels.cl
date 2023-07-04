typedef struct {
    float strength;
} BrdfLambert;

typedef enum {
    BrdfTypeLambert
} BrdfType;

typedef struct {
    BrdfType type;
    union {
        BrdfLambert lambert;
    };
} Brdf;

Color BrdfLambert_reflected(BrdfLambert *lambert, Vector dirIn, Normal nrm, Vector dirOut, Color albedo)
{
    return albedo * lambert->strength / 3.14f;        
}

float BrdfLambert_pdf(BrdfLambert *lambert, Vector dirIn, Normal nrm, Vector dirOut)
{
    float cosTheta = max(dot(dirIn, nrm), 0.0f);
    return cosTheta / 3.14f;
}

Vector BrdfLambert_sample(Brdf *brdf, float2 random, Vector nrm, Vector dirOut)
{
    float phi = 2 * 3.14f * random.x;
    float theta = asin(sqrt(random.y));

    Vector x;
    Vector y;
    Vector z = nrm;
    if(fabs(nrm.z) > fabs(nrm.x) && fabs(nrm.z) > fabs(nrm.y)) {
        x = (Vector)(1, 0, 0);
        y = (Vector)(0, 1, 0);
    } else if(fabs(nrm.x) > fabs(nrm.y) && fabs(nrm.x) > fabs(nrm.z)) {
        x = (Vector)(0, 1, 0);
        y = (Vector)(0, 0, 1);
    } else {
        x = (Vector)(1, 0, 0);
        y = (Vector)(0, 0, 1);
    }
    
    x = normalize(x - z * dot(x, z));
    y = normalize(y - z * dot(y, z));

    return x * cos(phi) * cos(theta) + y * sin(phi) * cos(theta) + z * sin(theta);
}

Color Brdf_reflected(Brdf *brdf, Vector dirIn, Normal nrm, Vector dirOut, Color albedo)
{
    switch(brdf->type) {
        case BrdfTypeLambert:
            return BrdfLambert_reflected(&brdf->lambert, dirIn, nrm, dirOut, albedo);

        default:
            return (Color)(0, 0, 0);
    }
}

float Brdf_pdf(Brdf *brdf, Vector dirIn, Normal nrm, Vector dirOut)
{
    switch(brdf->type) {
        case BrdfTypeLambert:
            return BrdfLambert_pdf(&brdf->lambert, dirIn, nrm, dirOut);

        default:
            return 0;
    }
}

Vector Brdf_sample(Brdf *brdf, float2 random, Vector nrm, Vector dirOut)
{
    switch(brdf->type) {
        case BrdfTypeLambert:
            return BrdfLambert_sample(&brdf->lambert, random, nrm, dirOut);

        default:
            return (Vector)(0, 0, 0);
    }
}