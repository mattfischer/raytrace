typedef struct {
    int width;
    int height;
    int samples;
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
    SamplerState samplerState;
} Item;

typedef struct {
    global int *data;
} WorkQueue;

typedef struct {
    Scene scene;
    Settings settings;
    Item *items;
    unsigned int currentPixel;
    Sampler sampler;

    WorkQueue generateCameraRayQueue;
    WorkQueue intersectRaysQueue;
    WorkQueue directLightAreaQueue;
    WorkQueue directLightPointQueue;
    WorkQueue extendPathQueue;
    WorkQueue commitRadianceQueue;
} Context;

void Queue_addItem(WorkQueue *queue, int key)
{
    int write = atomic_inc(&queue->data[1]);
    
    queue->data[write + 2] = key;
}

int Queue_getKey(WorkQueue *queue, int idx)
{
    return queue->data[idx + 2];
}

int Queue_numQueued(WorkQueue *queue)
{
    return queue->data[1] - queue->data[0];
}

void Queue_clear(WorkQueue *queue)
{
    queue->data[0] = 0;
    queue->data[1] = 0;
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
    float phi = 2 * M_PI_F * aperturePoint.y;
    float2 apertureDiscPoint = (float2)(r * cos(phi), r * sin(phi));
    Point q = camera->position + (camera->imagePlane.u * apertureDiscPoint.x + camera->imagePlane.v * apertureDiscPoint.y) * camera->apertureSize;
    direction = normalize(p - q);

    beam->ray.origin = q;
    beam->ray.direction = direction;
    
    float pixelSize = 2.0f / width;

    beam->originDifferential.u = (Vector)(0, 0, 0);
    beam->originDifferential.v = (Vector)(0, 0, 0);
    beam->directionDifferential.u = camera->imagePlane.u * pixelSize / len;
    beam->directionDifferential.v = camera->imagePlane.v * pixelSize / len;
}

kernel void generateCameraRays(global Context *context)
{
    int key = Queue_getKey(&context->generateCameraRayQueue, get_global_id(0));
    Item *item = &context->items[key];

    unsigned int cp = atomic_inc(&context->currentPixel);

    int sample = cp / (context->settings.width * context->settings.height);
    if(sample >= context->settings.samples) {
        return;
    }

    item->y = (cp / context->settings.width) % context->settings.height;
    item->x = cp % context->settings.width;
    Sampler_startSample(&context->sampler, &item->samplerState, item->x, item->y, sample);

    float2 imagePoint = (float2)(item->x, item->y) + Sampler_getValue2D(&context->sampler, &item->samplerState);
    float2 aperturePoint = Sampler_getValue2D(&context->sampler, &item->samplerState);
    createPixelBeam(&context->scene.camera, imagePoint, context->settings.width, context->settings.height, aperturePoint, &item->beam);
    item->specularBounce = false;
    item->generation = 0;
    item->radiance = (Radiance)(0, 0, 0);
    item->throughput = (Color)(1, 1, 1);

    Queue_addItem(&context->intersectRaysQueue, key);
}

kernel void intersectRays(global Context *context)
{
    int key = Queue_getKey(&context->intersectRaysQueue, get_global_id(0));
    Item *item = &context->items[key];

    Scene_intersect(&context->scene, &item->beam, &item->isect, MAXFLOAT, true);

    if(item->isect.primitive == NULL) {
        Radiance rad2 = context->scene.skyRadiance;
        item->radiance += rad2 * item->throughput;
        Queue_addItem(&context->commitRadianceQueue, key);
    } else {
        Radiance rad2 = item->isect.primitive->surface.radiance;
        float misWeight = 1.0f;
        if(length(rad2) > 0 && !item->specularBounce && item->generation > 0) {
            Normal nrmFacing = item->isect.facingNormal;
            float dot2 = -dot(nrmFacing, item->beam.ray.direction);
            float d = item->isect.shapeIntersection.distance;
            float pdfArea = item->pdf * dot2 / (d * d);
            float pdfLight = Shape_samplePdf(&item->isect.primitive->shape, item->isect.point);
            misWeight = pdfArea * pdfArea / (pdfArea * pdfArea + pdfLight * pdfLight);
        }

        item->radiance += rad2 * item->throughput * misWeight;        
    
        int totalLights = context->scene.numAreaLights + context->scene.numPointLights;
        int lightIndex = (int)floor(Sampler_getValue(&context->sampler, &item->samplerState) * totalLights);

        if(lightIndex < context->scene.numAreaLights) {
            item->lightIndex = lightIndex;
            Queue_addItem(&context->directLightAreaQueue, key);
        } else {
            item->lightIndex = lightIndex - context->scene.numAreaLights;
            Queue_addItem(&context->directLightPointQueue, key);
        }
    }
}

kernel void directLightArea(global Context *context)
{
    int key = Queue_getKey(&context->directLightAreaQueue, get_global_id(0));
    Item *item = &context->items[key];

    Intersection *isect = &item->isect;
    Normal nrmFacing = isect->facingNormal;
    Point pntOffset = isect->point + nrmFacing * 0.01f;

    Primitive *light = context->scene.areaLights[item->lightIndex];

    float2 rand = Sampler_getValue2D(&context->sampler, &item->samplerState);
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
            Scene_intersect(&context->scene, &shadowBeam, &shadowIsect, d, false);

            if(shadowIsect.primitive == NULL || shadowIsect.primitive == light) {
                Radiance rad2 = light->surface.radiance;
                Radiance irad = rad2 * dot2 * dt / (d * d * pdf);
                float pdfBrdf = Surface_pdf(isect, dirIn) * dot2 / (d * d);
                float misWeight = pdf * pdf / (pdf * pdf + pdfBrdf * pdfBrdf);
                Radiance rad = irad * Surface_reflected(isect, dirIn);
                item->radiance += rad * item->throughput * misWeight;
            }
        }
    }

    Queue_addItem(&context->extendPathQueue, key);
}

kernel void directLightPoint(global Context *context)
{
    int key = Queue_getKey(&context->directLightPointQueue, get_global_id(0));
    Item *item = &context->items[key];

    Intersection *isect = &item->isect;
    Normal nrmFacing = isect->facingNormal;
    Point pntOffset = isect->point + nrmFacing * 0.01f;
    PointLight *pointLight = &context->scene.pointLights[item->lightIndex];

    Vector dirIn = pointLight->position - pntOffset;
    float d = length(dirIn);
    dirIn = dirIn / d;

    float dt = dot(dirIn, nrmFacing);
    if(dt > 0) {
        Beam shadowBeam;
        shadowBeam.ray.origin = pntOffset;
        shadowBeam.ray.direction = dirIn;

        Intersection shadowIsect;
        Scene_intersect(&context->scene, &shadowBeam, &shadowIsect, d, false);

        if(shadowIsect.primitive == NULL) {
            Radiance irad = pointLight->radiance * dt / (d * d);
            Radiance rad = irad * Surface_reflected(isect, dirIn);
            item->radiance += rad * item->throughput;
        }
    }

    Queue_addItem(&context->extendPathQueue, key);
}

kernel void extendPath(global Context *context)
{
    int key = Queue_getKey(&context->extendPathQueue, get_global_id(0));
    Item *item = &context->items[key];
    Intersection *isect = &item->isect;
    Normal nrmFacing = isect->facingNormal;
    
    Vector dirIn;
    float pdf;
    bool pdfDelta;
    
    Color reflected = Surface_sample(isect, &context->sampler, &item->samplerState, &dirIn, &pdf, &pdfDelta);

    float dt = dot(dirIn, nrmFacing);
    float reverse = (dt > 0) ? 1.0f : -1.0f;
    dt *= reverse;
    Point pntOffset = isect->point + nrmFacing * 0.01f * reverse;
    
    item->pdf = pdf;
    item->specularBounce = pdfDelta;
    if(dt > 0) {
        item->throughput = item->throughput * reflected * dt / pdf;

        float threshold = 0.0f;
        float roulette = Sampler_getValue(&context->sampler, &item->samplerState);
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
            Queue_addItem(&context->intersectRaysQueue, key);
        } else {
            Queue_addItem(&context->commitRadianceQueue, key);
        }
    } else {
        Queue_addItem(&context->commitRadianceQueue, key);
    }
}
