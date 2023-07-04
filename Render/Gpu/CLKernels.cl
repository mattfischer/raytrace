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
    Scene scene;
    Settings settings;
    Item *items;
    float *random;
    unsigned int currentPixel;

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

int Queue_getNextKey(WorkQueue *queue)
{
    int read = atomic_inc(&queue->data[0]);

    return queue->data[read + 2];
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

void generateCameraRays(global Context *context)
{
    int key = Queue_getNextKey(&context->generateCameraRayQueue);
    Item *item = &context->items[key];

    unsigned int cp = atomic_inc(&context->currentPixel);

    int sample = cp / (context->settings.width * context->settings.height);
    if(sample >= context->settings.minSamples) {
        return;
    }

    item->y = (cp / context->settings.width) % context->settings.height;
    item->x = cp % context->settings.width;

    float *r = context->random + key * 11;

    float2 imagePoint = (float2)(item->x, item->y) + (float2)(r[0], r[1]);
    float2 aperturePoint = (float2)(r[2], r[3]);
    createPixelBeam(&context->scene.camera, imagePoint, context->settings.width, context->settings.height, aperturePoint, &item->beam);
    item->specularBounce = false;
    item->generation = 0;
    item->radiance = (Radiance)(0, 0, 0);
    item->throughput = (Color)(1, 1, 1);

    Queue_addItem(&context->intersectRaysQueue, key);
}

void intersectRays(global Context *context)
{
    int key = Queue_getNextKey(&context->intersectRaysQueue);
    Item *item = &context->items[key];

    Scene_intersect(&context->scene, &item->beam, &item->isect);

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
    
        int totalLights = context->scene.numAreaLights + context->scene.numPointLights;
        float *r = context->random + key * 11;
        int lightIndex = (int)floor(r[4] * totalLights);

        if(lightIndex < context->scene.numAreaLights) {
            item->lightIndex = lightIndex;
            Queue_addItem(&context->directLightAreaQueue, key);
        } else {
            item->lightIndex = lightIndex - context->scene.numAreaLights;
            Queue_addItem(&context->directLightPointQueue, key);
        }
    }

    if(item->isect.primitive == 0) {
        Radiance rad2 = context->scene.skyRadiance;
        item->radiance += rad2 * item->throughput;
        Queue_addItem(&context->commitRadianceQueue, key);
    }
}

void directLightArea(global Context *context)
{
    int key = Queue_getNextKey(&context->directLightAreaQueue);
    Item *item = &context->items[key];

    Intersection *isect = &item->isect;
    Normal nrmFacing = facingNormal(isect);
    Point pntOffset = isect->point + nrmFacing * 0.01f;

    Primitive *light = context->scene.areaLights[item->lightIndex];
    
    float *r = context->random + key * 11;

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
            Scene_intersect(&context->scene, &shadowBeam, &shadowIsect);

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

    Queue_addItem(&context->extendPathQueue, key);
}

void directLightPoint(global Context *context)
{
    int key = Queue_getNextKey(&context->directLightPointQueue);
    Item *item = &context->items[key];

    Intersection *isect = &item->isect;
    Normal nrmFacing = facingNormal(isect);
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
        Scene_intersect(&context->scene, &shadowBeam, &shadowIsect);

        if(shadowIsect.primitive == 0 || shadowIsect.shapeIntersection.distance >= 0) {
            Radiance irad = pointLight->radiance * dt / (d * d);
            Radiance rad = irad * Surface_reflected(isect, dirIn);
            item->radiance += rad * item->throughput;
        }
    }

    Queue_addItem(&context->extendPathQueue, key);
}

void extendPath(global Context *context)
{
    int key = Queue_getNextKey(&context->extendPathQueue);
    Item *item = &context->items[key];
    Intersection *isect = &item->isect;
    Normal nrmFacing = facingNormal(isect);
    
    Vector dirIn;
    float pdf;
    bool pdfDelta;
    
    float *r = context->random + key * 11;

    float3 rand = (float3)(r[7], r[8], r[9]);
    Color reflected = Surface_sample(isect, rand, &dirIn, &pdf, &pdfDelta);
    float dt = dot(dirIn, nrmFacing);
    Point pntOffset = isect->point + nrmFacing * 0.01f;
    
    item->pdf = pdf;
    item->specularBounce = pdfDelta;
    if(dt > 0) {
        item->throughput = item->throughput * reflected * dt / pdf;

        float threshold = 0.0f;
        float roulette = r[10];
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

kernel void extendPathDone(global Context *context)
{
    Queue_clear(&context->extendPathQueue);
}

kernel void directLightPointDone(global Context *context)
{
    Queue_clear(&context->directLightPointQueue);

    clk_event_t evt;
    ndrange_t ndr = ndrange_1D(Queue_numQueued(&context->extendPathQueue));
    enqueue_kernel(get_default_queue(),
                    CLK_ENQUEUE_FLAGS_NO_WAIT,
                    ndr,
                    0, NULL, &evt,
                    ^{ extendPath(context); });

    ndrange_t nd1 = ndrange_1D(1);
    enqueue_kernel(get_default_queue(),
                    CLK_ENQUEUE_FLAGS_NO_WAIT,
                    nd1,
                    1, &evt, NULL,
                    ^{ extendPathDone(context); });
    release_event(evt);
}

kernel void directLightAreaDone(global Context *context)
{
    Queue_clear(&context->directLightAreaQueue);

    clk_event_t evt;
    ndrange_t ndr = ndrange_1D(Queue_numQueued(&context->directLightPointQueue));
    enqueue_kernel(get_default_queue(),
                    CLK_ENQUEUE_FLAGS_NO_WAIT,
                    ndr,
                    0, NULL, &evt,
                    ^{ directLightPoint(context); });

    ndrange_t nd1 = ndrange_1D(1);
    enqueue_kernel(get_default_queue(),
                    CLK_ENQUEUE_FLAGS_NO_WAIT,
                    nd1,
                    1, &evt, NULL,
                    ^{ directLightPointDone(context); });
    release_event(evt);
}

kernel void intersectRaysDone(global Context *context)
{
    Queue_clear(&context->intersectRaysQueue);

    clk_event_t evt;
    ndrange_t ndr = ndrange_1D(Queue_numQueued(&context->directLightAreaQueue));
    enqueue_kernel(get_default_queue(),
                    CLK_ENQUEUE_FLAGS_NO_WAIT,
                    ndr,
                    0, NULL, &evt,
                    ^{ directLightArea(context); });

    ndrange_t nd1 = ndrange_1D(1);
    enqueue_kernel(get_default_queue(),
                    CLK_ENQUEUE_FLAGS_NO_WAIT,
                    nd1,
                    1, &evt, NULL,
                    ^{ directLightAreaDone(context); });
    release_event(evt);
}

kernel void generateCameraRaysDone(global Context *context)
{
    Queue_clear(&context->generateCameraRayQueue);

    clk_event_t evt;
    ndrange_t ndr = ndrange_1D(Queue_numQueued(&context->intersectRaysQueue));
    enqueue_kernel(get_default_queue(),
                    CLK_ENQUEUE_FLAGS_NO_WAIT,
                    ndr,
                    0, NULL, &evt,
                    ^{ intersectRays(context); });

    ndrange_t nd1 = ndrange_1D(1);
    enqueue_kernel(get_default_queue(),
                    CLK_ENQUEUE_FLAGS_NO_WAIT,
                    nd1,
                    1, &evt, NULL,
                    ^{ intersectRaysDone(context); });
    release_event(evt);
}

kernel void runIteration(global Context *context)
{
    clk_event_t evt;
    ndrange_t ndr = ndrange_1D(Queue_numQueued(&context->generateCameraRayQueue));
    enqueue_kernel(get_default_queue(),
                    CLK_ENQUEUE_FLAGS_NO_WAIT,
                    ndr,
                    0, NULL, &evt,
                    ^{ generateCameraRays(context); });

    ndrange_t nd1 = ndrange_1D(1);
    enqueue_kernel(get_default_queue(),
                    CLK_ENQUEUE_FLAGS_NO_WAIT,
                    nd1,
                    1, &evt, NULL,
                    ^{ generateCameraRaysDone(context); });
    release_event(evt);
}