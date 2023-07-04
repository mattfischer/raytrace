typedef struct {
    float strength;
} BrdfLambert;

typedef struct {
    float strength;
    float roughness;
} BrdfOrenNayar;

typedef struct {
    float strength;
    float power;
} BrdfPhong;

typedef enum {
    BrdfTypeLambert,
    BrdfTypeOrenNayar,
    BrdfTypePhong
} BrdfType;

typedef struct {
    BrdfType type;
    union {
        BrdfLambert lambert;
        BrdfOrenNayar orenNayar;
        BrdfPhong phong;
    };
} Brdf;

Color BrdfLambert_reflected(BrdfLambert *lambert, Vector dirIn, Normal nrm, Vector dirOut, Color albedo)
{
    return albedo * lambert->strength / M_PI_F;        
}

float BrdfLambert_pdf(BrdfLambert *lambert, Vector dirIn, Normal nrm, Vector dirOut)
{
    float cosTheta = max(dot(dirIn, nrm), 0.0f);
    return cosTheta / M_PI_F;
}

Vector BrdfLambert_sample(BrdfLambert *lambert, float2 random, Vector nrm, Vector dirOut)
{
    float phi = 2 * M_PI_F * random.x;
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

Color BrdfOrenNayar_reflected(BrdfOrenNayar *orenNayar, Vector dirIn, Normal nrm, Vector dirOut, Color albedo)
{
    float cosThetaI = dot(dirIn, nrm);
    float sinThetaI = sqrt(max(0.0f, 1 - cosThetaI * cosThetaI));
    float tanThetaI = sinThetaI / cosThetaI;

    float cosThetaR = dot(dirOut, nrm);
    float sinThetaR = sqrt(max(0.0f, 1 - cosThetaR * cosThetaR));
    float tanThetaR = sinThetaR / cosThetaR;

    float cosPhi;
    if(sinThetaI < 0.001f || sinThetaR < 0.001f) {
        cosPhi = 1;
    } else {
        Vector projectedIncident = (dirIn - nrm * dot(dirIn, nrm)) / sinThetaI;
        Vector projectedOutgoing = (dirOut - nrm * dot(dirOut, nrm)) / sinThetaR;
        cosPhi = dot(projectedIncident, projectedOutgoing);
    }

    float sigma2 = orenNayar->roughness * orenNayar->roughness;

    float A = 1.0f - 0.5f * sigma2 / (sigma2 + 0.33f);
    float B = 0.45f * sigma2 / (sigma2 + 0.09f);

    float sinAlpha = max(sinThetaI, sinThetaR);
    float tanBeta = min(tanThetaI, tanThetaR);

    return albedo * orenNayar->strength * (A + B * max(0.0f, cosPhi) * sinAlpha * tanBeta) / M_PI_F;
}

float BrdfOrenNayar_pdf(BrdfOrenNayar *orenNayar, Vector dirIn, Normal nrm, Vector dirOut)
{
    float cosTheta = max(dot(dirIn, nrm), 0.0f);
    return cosTheta / M_PI_F;
}

Vector BrdfOrenNayar_sample(BrdfOrenNayar *orenNayar, float2 random, Vector nrm, Vector dirOut)
{
    float phi = 2 * M_PI_F * random.x;
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

Color BrdfPhong_reflected(BrdfPhong *phong, Vector dirIn, Normal nrm, Vector dirOut, Color albedo)
{
    Vector dirReflect = -(dirIn - nrm * 2 * dot(nrm, dirIn));
    float dt = dot(dirReflect, dirOut);
    float coeff = 0;
    if(dt > 0) {
        coeff = pow(dt, phong->power);
    }

    return (Color)(1, 1, 1) * phong->strength * coeff * (phong->power + 1) / (2 * M_PI_F);
}

float BrdfPhong_pdf(BrdfPhong *phong, Vector dirIn, Normal nrm, Vector dirOut)
{
    float coeff = 0;
    Vector dirReflect = -(dirIn - nrm * dot(dirIn, nrm) * 2);
    float dt = dot(dirReflect, dirOut);
    if(dt > 0) {
        coeff = pow(dt, phong->power);
    }

    float pdf = coeff * (phong->power + 1) / (2 * M_PI_F);
    return min(pdf, 1000.0f);
}

Vector BrdfPhong_sample(BrdfPhong *phong, float2 random, Vector nrm, Vector dirOut)
{
    float phi = 2 * M_PI_F * random.x;
    float theta = acos(pow(random.y, 1.0f / (phong->power + 1)));

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

    Vector dirReflect = x * cos(phi) * cos(theta) + y * sin(phi) * cos(theta) + z * sin(theta);
    Vector dirIn = -(dirReflect - nrm * 2 * dot(dirReflect, nrm));

    return dirIn;
}

Color Brdf_reflected(Brdf *brdf, Vector dirIn, Normal nrm, Vector dirOut, Color albedo)
{
    switch(brdf->type) {
        case BrdfTypeLambert:
            return BrdfLambert_reflected(&brdf->lambert, dirIn, nrm, dirOut, albedo);

        case BrdfTypeOrenNayar:
            return BrdfOrenNayar_reflected(&brdf->orenNayar, dirIn, nrm, dirOut, albedo);

        case BrdfTypePhong:
            return BrdfPhong_reflected(&brdf->phong, dirIn, nrm, dirOut, albedo);

        default:
            return (Color)(0, 0, 0);
    }
}

float Brdf_pdf(Brdf *brdf, Vector dirIn, Normal nrm, Vector dirOut)
{
    switch(brdf->type) {
        case BrdfTypeLambert:
            return BrdfLambert_pdf(&brdf->lambert, dirIn, nrm, dirOut);

        case BrdfTypeOrenNayar:
            return BrdfOrenNayar_pdf(&brdf->orenNayar, dirIn, nrm, dirOut);

        case BrdfTypePhong:
            return BrdfPhong_pdf(&brdf->phong, dirIn, nrm, dirOut);

        default:
            return 0;
    }
}

Vector Brdf_sample(Brdf *brdf, float2 random, Vector nrm, Vector dirOut)
{
    switch(brdf->type) {
        case BrdfTypeLambert:
            return BrdfLambert_sample(&brdf->lambert, random, nrm, dirOut);

        case BrdfTypeOrenNayar:
            return BrdfOrenNayar_sample(&brdf->orenNayar, random, nrm, dirOut);

        case BrdfTypePhong:
            return BrdfPhong_sample(&brdf->phong, random, nrm, dirOut);

        default:
            return (Vector)(0, 0, 0);
    }
}