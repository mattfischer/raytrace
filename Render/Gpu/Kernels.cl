typedef struct {
    int width;
    int height;
    int minSamples;
} Settings;

typedef struct {
    Beam beam;
    Intersection isect;
    bool specularBounce;
    int generation;
    float pdf;
    Color throughput;
    Radiance radiance;
    int lightIndex;
    int x;
    int y;
} Item;

typedef struct {
    global int *data;
} WorkQueue;

typedef struct {
    WorkQueue generateCameraRayQueue;
    WorkQueue intersectRaysQueue;
    WorkQueue directLightAreaQueue;
    WorkQueue directLightPointQueue;
    WorkQueue extendPathQueue;
    WorkQueue commitRadianceQueue;
} Queues;

void Queue_addItem(WorkQueue *queue, int key)
{
    int write = atomic_inc(&queue->data[1]);
    
    queue->data[write + 2] = key;
}

int Queue_getNextKey(WorkQueue *queue)
{
    int read = atomic_inc(&queue->data[0]);

    return queue->data[read + 2];
}
        
void createPixelBeam(Camera *camera, float2 imagePoint, int width, int height, float2 aperturePoint, Beam *beam)
{
    float cx = (2 * imagePoint.x - width) / width;
    float cy = (2 * imagePoint.y - height) / width;
    float2 imagePointTransformed = (float2)(cx, -cy);

    Vector direction = camera->direction + (camera->imagePlane.u * imagePointTransformed.x + camera->imagePlane.v * imagePointTransformed.y) * camera->imageSize;
    float len = length(direction);
    direction = direction / len;

    Point p = camera->position + direction * camera->focalLength;
    float r = sqrt(aperturePoint.x);
    float phi = 2 * 3.14f * aperturePoint.y;
    float2 apertureDiscPoint = (float2)(r * cos(phi), r * sin(phi));
    Point q = camera->position + (camera->imagePlane.u * apertureDiscPoint.x + camera->imagePlane.v * apertureDiscPoint.y) * camera->apertureSize;
    direction = normalize(p - q);

    beam->ray.origin = q;
    beam->ray.direction = direction;
    
    float pixelSize = 2.0f / width;

    beam->originDifferential.u = (float3)(0, 0, 0);
    beam->originDifferential.v = (float3)(0, 0, 0);
    beam->directionDifferential.u = camera->imagePlane.u * pixelSize / len;
    beam->directionDifferential.v = camera->imagePlane.v * pixelSize / len;
}

kernel void generateCameraRays(global Scene *scene, global Settings *settings, global Item *items, global float* random, global Queues *queues, global unsigned int *currentPixel)
{
    int key = Queue_getNextKey(&queues->generateCameraRayQueue);
    Item *item = &items[key];

    unsigned int cp = atomic_inc(currentPixel);

    int sample = cp / (settings->width * settings->height);
    if(sample >= settings->minSamples) {
        return;
    }

    item->y = (cp / settings->width) % settings->height;
    item->x = cp % settings->width;

    float *r = random + key * 10;

    float2 imagePoint = (float2)(item->x, item->y) + (float2)(r[0], r[1]);
    float2 aperturePoint = (float2)(r[2], r[3]);
    createPixelBeam(&scene->camera, imagePoint, settings->width, settings->height, aperturePoint, &item->beam);
    item->specularBounce = false;
    item->generation = 0;
    item->radiance = (float3)(0, 0, 0);
    item->throughput = (float3)(1, 1, 1);

    Queue_addItem(&queues->intersectRaysQueue, key);
}

kernel void intersectRays(global Scene *scene, global Item *items, global float *random, global Queues *queues)
{
    int key = Queue_getNextKey(&queues->intersectRaysQueue);
    Item *item = &items[key];

    Scene_intersect(scene, &item->beam, &item->isect);

    if(item->isect.primitive != 0) {
        Radiance rad2 = item->isect.primitive->surface.radiance;
        float misWeight = 1.0f;
        if(length(rad2) > 0 && !item->specularBounce && item->generation > 0) {
            Normal nrmFacing = facingNormal(&item->isect);
            float dot2 = -dot(nrmFacing, item->beam.ray.direction);
            float d = item->isect.shapeIntersection.distance;
            float pdfArea = item->pdf * dot2 / (d * d);
            float pdfLight = Shape_samplePdf(&item->isect.primitive->shape, item->isect.point);
            misWeight = pdfArea * pdfArea / (pdfArea * pdfArea + pdfLight * pdfLight);
        }

        item->radiance += rad2 * item->throughput * misWeight;        
    
        int totalLights = scene->numAreaLights + scene->numPointLights;
        float *r = random + key * 10;
        int lightIndex = (int)floor(r[4] * totalLights);

        if(lightIndex < scene->numAreaLights) {
            item->lightIndex = lightIndex;
            Queue_addItem(&queues->directLightAreaQueue, key);
        } else {
            item->lightIndex = lightIndex - scene->numAreaLights;
            Queue_addItem(&queues->directLightPointQueue, key);
        }
    }

    if(item->isect.primitive == 0) {
        Radiance rad2 = scene->skyRadiance;
        item->radiance += rad2 * item->throughput;
        Queue_addItem(&queues->commitRadianceQueue, key);
    }
}

kernel void directLightArea(global Scene *scene, global Item *items, global float *random, global Queues *queues)
{
    int key = Queue_getNextKey(&queues->directLightAreaQueue);
    Item *item = &items[key];

    Intersection *isect = &item->isect;
    Normal nrmFacing = facingNormal(isect);
    Point pntOffset = isect->point + nrmFacing * 0.01f;

    Primitive *light = scene->areaLights[item->lightIndex];
    
    float *r = random + key * 10;

    float2 rand = (float2)(r[5], r[6]);
    Point pnt2;
    Normal nrm2;
    float pdf;
    if(Shape_sample(&light->shape, rand, &pnt2, &nrm2, &pdf)) {
        Vector dirIn = pnt2 - pntOffset;
        float d = length(dirIn);
        dirIn = dirIn / d;
        float dot2 = fabs(dot(dirIn, nrm2));
        float dt = dot(dirIn, nrmFacing);
        if(dt > 0) {    
            Beam shadowBeam;
            shadowBeam.ray.origin = pntOffset;
            shadowBeam.ray.direction = dirIn;

            Intersection shadowIsect;
            Scene_intersect(scene, &shadowBeam, &shadowIsect);

            if(shadowIsect.primitive == light) {
                Radiance rad2 = light->surface.radiance;
                Radiance irad = rad2 * dot2 * dt / (d * d * pdf);
                float pdfBrdf = Surface_pdf(isect, dirIn) * dot2 / (d * d);
                float misWeight = pdf * pdf / (pdf * pdf + pdfBrdf * pdfBrdf);
                Radiance rad = irad * Surface_reflected(isect, dirIn);
                item->radiance += rad * item->throughput * misWeight;
            }
        }
    }

    Queue_addItem(&queues->extendPathQueue, key);
}

kernel void directLightPoint(global Scene *scene, global Item *items, global Queues *queues)
{
    int key = Queue_getNextKey(&queues->directLightPointQueue);
    Item *item = &items[key];

    Intersection *isect = &item->isect;
    Normal nrmFacing = facingNormal(isect);
    Point pntOffset = isect->point + nrmFacing * 0.01f;
    PointLight *pointLight = &scene->pointLights[item->lightIndex];

    Vector dirIn = pointLight->position - pntOffset;
    float d = length(dirIn);
    dirIn = dirIn / d;

    float dt = dot(dirIn, nrmFacing);
    if(dt > 0) {
        Beam shadowBeam;
        shadowBeam.ray.origin = pntOffset;
        shadowBeam.ray.direction = dirIn;

        Intersection shadowIsect;
        Scene_intersect(scene, &shadowBeam, &shadowIsect);

        if(shadowIsect.primitive == 0 || shadowIsect.shapeIntersection.distance >= 0) {
            Radiance irad = pointLight->radiance * dt / (d * d);
            Radiance rad = irad * Surface_reflected(isect, dirIn);
            item->radiance += rad * item->throughput;
        }
    }

    Queue_addItem(&queues->extendPathQueue, key);
}

kernel void extendPath(global Scene *scene, global Item *items, global float *random, global Queues *queues)
{
    int key = Queue_getNextKey(&queues->extendPathQueue);
    Item *item = &items[key];
    Intersection *isect = &item->isect;
    Normal nrmFacing = facingNormal(isect);
    
    Vector dirIn;
    float pdf;
    bool pdfDelta;
    
    float *r = random + key * 10;

    float2 rand = (float2)(r[7], r[8]);
    Color reflected = Surface_sample(isect, rand, &dirIn, &pdf, &pdfDelta);
    float dt = dot(dirIn, nrmFacing);
    Point pntOffset = isect->point + nrmFacing * 0.01f;
    
    item->pdf = pdf;
    item->specularBounce = pdfDelta;
    if(dt > 0) {
        item->throughput = item->throughput * reflected * dt / pdf;

        float threshold = 0.0f;
        float roulette = r[9];
        if(item->generation == 0) {
            threshold = 1.0f;
        } else if(item->generation < 10) {
            float tmax = max(item->throughput.x, max(item->throughput.y, item->throughput.z));
            threshold = min(1.0f, tmax);
        }

        if(roulette < threshold) {
            item->beam.ray.origin = pntOffset;
            item->beam.ray.direction = dirIn;
            item->throughput = item->throughput / threshold;
            item->generation++;
            Queue_addItem(&queues->intersectRaysQueue, key);
        } else {
            Queue_addItem(&queues->commitRadianceQueue, key);
        }
    } else {
        Queue_addItem(&queues->commitRadianceQueue, key);
    }
}