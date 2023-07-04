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

typedef struct {
    float strength;
    float roughness;
    float ior;
} BrdfTorranceSparrow;

typedef enum {
    BrdfTypeLambert,
    BrdfTypeOrenNayar,
    BrdfTypePhong,
    BrdfTypeTorranceSparrow
} BrdfType;

typedef struct {
    BrdfType type;
    union {
        BrdfLambert lambert;
        BrdfOrenNayar orenNayar;
        BrdfPhong phong;
        BrdfTorranceSparrow torranceSparrow;
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

Color BrdfTorranceSparrow_reflected(BrdfTorranceSparrow *torranceSparrow, Vector dirIn, Normal nrm, Vector dirOut, Color albedo)
{
    Vector dirHalf = normalize(dirIn + dirOut);
    float alpha = acos(min(1.0f, dot(nrm, dirHalf)));
    float cosAlpha = cos(alpha);
    float tanAlpha = tan(alpha);
    float m2 = torranceSparrow->roughness * torranceSparrow->roughness;
    
    float D = exp(-tanAlpha * tanAlpha / m2) / (M_PI_F * m2 * cosAlpha * cosAlpha * cosAlpha * cosAlpha);

    float cosThetaI = dot(dirIn, nrm);
    float oneMinusCos = 1 - cosThetaI;
    float R0 = (1 - torranceSparrow->ior) / (1 + torranceSparrow->ior);
    R0 = R0 * R0;

    float F = R0 + (1 - R0) * oneMinusCos * oneMinusCos * oneMinusCos * oneMinusCos * oneMinusCos;

    float HN = dot(dirHalf, nrm);
    float VH = dot(dirHalf, dirOut);
    float VN = dot(nrm, dirOut);
    float LN = dot(nrm, dirIn);
    float G = min(1.0f, min(2 * HN * VN / VH, 2 * HN * LN / VH));

    return (Color)(1, 1, 1) * torranceSparrow->strength * D * F * G / (4 * VN * LN);
}

float BrdfTorranceSparrow_pdf(BrdfTorranceSparrow *torranceSparrow, Vector dirIn, Normal nrm, Vector dirOut)
{
    Vector axis = normalize(dirIn + dirOut);

    float cosTheta = dot(axis, nrm);
    float sinTheta = sqrt(max(0.0f, 1 - cosTheta * cosTheta));
    float tanTheta = sinTheta / cosTheta;

    float m2 = torranceSparrow->roughness * torranceSparrow->roughness;
    float pdf = exp(-tanTheta * tanTheta / m2) / (M_PI_F * m2 * cosTheta * cosTheta * cosTheta * cosTheta);
    pdf = pdf / (4 * dot(dirOut, axis));
    return min(pdf, 1000.0f);
}

Vector BrdfTorranceSparrow_sample(BrdfTorranceSparrow *torranceSparrow, float2 random, Vector nrm, Vector dirOut)
{
    float phi = 2 * M_PI_F * random.x;
    float tanTheta = sqrt(-torranceSparrow->roughness * torranceSparrow->roughness * log(1 - random.y));
    float theta = atan(tanTheta);

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

    Vector axis = x * cos(phi) * cos(theta) + y * sin(phi) * cos(theta) + z * sin(theta);
    Vector dirIn = -(dirOut - axis * 2 * dot(dirOut, axis));

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

        case BrdfTypeTorranceSparrow:
            return BrdfTorranceSparrow_reflected(&brdf->torranceSparrow, dirIn, nrm, dirOut, albedo);

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

        case BrdfTypeTorranceSparrow:
            return BrdfTorranceSparrow_pdf(&brdf->torranceSparrow, dirIn, nrm, dirOut);

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

        case BrdfTypeTorranceSparrow:
            return BrdfTorranceSparrow_sample(&brdf->torranceSparrow, random, nrm, dirOut);

        default:
            return (Vector)(0, 0, 0);
    }
}