#include "Object/Scene.hpp"
#include "Parse/Parser.hpp"

#include "Render/Cpu/Renderer.hpp"
#include "Render/Cpu/Lighter/Direct.hpp"
#include "Render/Cpu/Lighter/UniPath.hpp"
#include "Render/Cpu/Lighter/IrradianceCached.hpp"

#include "Render/Gpu/Renderer.hpp"

#include "Render/LightProbe.hpp"

#include "Math/Sampler/Random.hpp"

#include "App/PythonInterface.hpp"

#include <structmember.h>

namespace App {
    PyTypeObject EngineType;
    PyTypeObject SettingsType;
    PyTypeObject SceneType;
    PyTypeObject FramebufferType;

    struct SceneObject {
        PyObject_HEAD
        Object::Scene *scene;
    };

    struct FramebufferObject;
    class Listener;
    struct EngineObject {
        PyObject_HEAD
        SceneObject *sceneObject;
        FramebufferObject *renderFramebufferObject;
        FramebufferObject *sampleStatusFramebufferObject;
        Listener *listener;
        Render::Renderer *renderer;
    };

    struct SettingsObject {
        PyObject_HEAD
        unsigned int width;
        unsigned int height;
        PyObject *lighting;
        unsigned int minSamples;
        unsigned int maxSamples;
        float sampleThreshold;
        unsigned int irradianceCacheSamples;
        float irradianceCacheThreshold;
        PyObject *renderer;
    };

    struct FramebufferObject {
        PyObject_HEAD
        unsigned int width;
        unsigned int height;
        void *bits;
    };

    FramebufferObject *wrapFramebuffer(Render::Framebuffer &framebuffer)
    {
        FramebufferObject *framebufferObject = PyObject_New(FramebufferObject, &FramebufferType);
        framebufferObject->width = framebuffer.width();
        framebufferObject->height = framebuffer.height();
        framebufferObject->bits = (void*)framebuffer.bits();

        return framebufferObject;
    }

    class Listener : public Render::Renderer::Listener
    {
    public:
        Listener(PyObject *listenerObject)
        {
            mListenerObject = listenerObject;
            Py_XINCREF(mListenerObject);
        }
        ~Listener()
        {
            Py_XDECREF(mListenerObject);
        }

        void onRendererDone(float totalTimeSeconds) override
        {
            PyGILState_STATE state = PyGILState_Ensure();
            PyObject_CallMethod(mListenerObject, "on_render_done", "f", totalTimeSeconds);            
            PyGILState_Release(state);
        }

    private:
        PyObject *mListenerObject;
    };

    static int Engine_init(PyObject *self, PyObject *args, PyObject *kwds)
    {
        EngineObject *engineObject = (EngineObject*)self;
        SettingsObject *settingsObject;

        if (!PyArg_ParseTuple(args, "OO", &engineObject->sceneObject, &settingsObject)) {
            return -1;
        }

        Py_INCREF(engineObject->sceneObject);

        wchar_t *renderer = PyUnicode_AsWideCharString(settingsObject->renderer, NULL);
        if(!wcscmp(renderer, L"cpu")) {
            Render::Cpu::Renderer::Settings settings;
            settings.width = settingsObject->width;
            settings.height = settingsObject->height;
            settings.minSamples = settingsObject->minSamples;
            settings.maxSamples = settingsObject->maxSamples;
            settings.sampleThreshold = settingsObject->sampleThreshold;

            wchar_t *lighting = PyUnicode_AsWideCharString(settingsObject->lighting, NULL);
            std::unique_ptr<Render::Cpu::Lighter::Base> lighter;
            if(!wcscmp(lighting, L"none")) {
                lighter = nullptr;
            } else if(!wcscmp(lighting, L"direct")) {
                lighter = std::make_unique<Render::Cpu::Lighter::Direct>();
            } else if(!wcscmp(lighting, L"pathTracing")) {
                lighter = std::make_unique<Render::Cpu::Lighter::UniPath>();
            } else if(!wcscmp(lighting, L"irradianceCaching")) {
                Render::Cpu::Lighter::IrradianceCached::Settings lighterSettings;

                lighterSettings.indirectSamples = settingsObject->irradianceCacheSamples;
                lighterSettings.cacheThreshold = settingsObject->irradianceCacheThreshold;

                lighter = std::make_unique<Render::Cpu::Lighter::IrradianceCached>(lighterSettings);
            }
            
            engineObject->renderer = new Render::Cpu::Renderer(*engineObject->sceneObject->scene, settings, std::move(lighter));
        } else if(!wcscmp(renderer, L"gpu")) {
            Render::Gpu::Renderer::Settings settings;
            settings.width = settingsObject->width;
            settings.height = settingsObject->height;
            settings.minSamples = settingsObject->minSamples;
            settings.maxSamples = settingsObject->maxSamples;
            settings.sampleThreshold = settingsObject->sampleThreshold;

            engineObject->renderer = new Render::Gpu::Renderer(*engineObject->sceneObject->scene, settings);
        }

        engineObject->renderFramebufferObject = wrapFramebuffer(engineObject->renderer->renderFramebuffer());
        engineObject->sampleStatusFramebufferObject = wrapFramebuffer(engineObject->renderer->sampleStatusFramebuffer());

        return 0;
    }

    static void Engine_dealloc(EngineObject *engineObject)
    {
        Py_XDECREF(engineObject->sceneObject);
        Py_XDECREF(engineObject->renderFramebufferObject);
        Py_XDECREF(engineObject->sampleStatusFramebufferObject);

        if(engineObject->listener) {
            delete engineObject->listener;
        }

        delete engineObject->renderer;
    }

    static PyObject *Engine_startRender(PyObject *self, PyObject *args)
    {
        EngineObject *engineObject = (EngineObject*)self;

        PyObject *listenerObject;
        if (!PyArg_ParseTuple(args, "O", &listenerObject)) {
            return NULL;
        }

        if(engineObject->listener) {
            delete engineObject->listener;
        }
        engineObject->listener = new Listener(listenerObject);
        engineObject->renderer->start(engineObject->listener);

        Py_RETURN_NONE;
    }

    static PyObject *Engine_stop(PyObject *self, PyObject *Py_UNUSED(ignored))
    {
        EngineObject *engineObject = (EngineObject*)self;

        engineObject->renderer->stop();

        Py_RETURN_NONE;
    }

    static PyObject *Engine_rendering(PyObject *self, PyObject *Py_UNUSED(ignored))
    {
        EngineObject *engineObject = (EngineObject*)self;

        return PyBool_FromLong(engineObject->renderer->running());
    }

    static PyObject *Engine_renderProbe(PyObject *self, PyObject *args)
    {
        EngineObject *engineObject = (EngineObject*)self;
        Render::Renderer &renderer = *engineObject->renderer;

        int x, y;
        if (!PyArg_ParseTuple(args, "II", &x, &y))
            return NULL;

        Object::Scene &scene = *engineObject->sceneObject->scene;
        Math::Beam beam = scene.camera().createPixelBeam(Math::Point2D((float)x, (float)y), renderer.renderFramebuffer().width(), renderer.renderFramebuffer().height(), Math::Point2D());
        Object::Intersection isect = scene.intersect(beam);

        if (isect.valid()) {
            Render::LightProbe probe(isect);
            PyObject *ret = PyList_New(1000);
            for(int i=0; i<1000; i++) {
                Object::Color color;
                float azimuth;
                float elevation;
                probe.getSample(azimuth, elevation, color);
                PyObject *colorTuple = Py_BuildValue("(fff)", color.red(), color.green(), color.blue());
                PyObject *sampleTuple = Py_BuildValue("(Off)", colorTuple, azimuth, elevation);
                PyList_SetItem(ret, i, sampleTuple);
            }
            return ret;
        } else {
            Py_RETURN_NONE;
        }
    }

    static PyMethodDef Engine_methods[] = {
        {"start_render", (PyCFunction) Engine_startRender, METH_VARARGS, ""},
        {"stop", (PyCFunction) Engine_stop, METH_NOARGS, ""},
        {"rendering", (PyCFunction) Engine_rendering, METH_NOARGS, ""},
        {"renderProbe", (PyCFunction) Engine_renderProbe, METH_VARARGS, ""},
        {NULL}
    };

    static PyMemberDef Engine_members[] = {
        {"render_framebuffer", T_OBJECT_EX, offsetof(EngineObject, renderFramebufferObject), 0},
        {"sample_status_framebuffer", T_OBJECT_EX, offsetof(EngineObject, sampleStatusFramebufferObject), 0},
        {NULL}
    };

    static PyMemberDef Settings_members[] = {
        {"width", T_UINT, offsetof(SettingsObject, width), 0},
        {"height", T_UINT, offsetof(SettingsObject, height), 0},
        {"lighting", T_OBJECT, offsetof(SettingsObject, lighting), 0},
        {"min_samples", T_UINT, offsetof(SettingsObject, minSamples), 0},
        {"max_samples", T_UINT, offsetof(SettingsObject, maxSamples), 0},
        {"sample_threshold", T_FLOAT, offsetof(SettingsObject, sampleThreshold), 0},
        {"irradiance_cache_samples", T_UINT, offsetof(SettingsObject, irradianceCacheSamples), 0},
        {"irradiance_cache_threshold", T_FLOAT, offsetof(SettingsObject, irradianceCacheThreshold), 0},
        {"renderer", T_OBJECT, offsetof(SettingsObject, renderer), 0},
        {NULL}
    };

    static int Scene_init(PyObject *self, PyObject *args, PyObject *kwds)
    {
        SceneObject *sceneObject = (SceneObject*)self;

        char *filename;
        if (!PyArg_ParseTuple(args, "s", &filename))
            return -1;

        std::unique_ptr<Object::Scene> scene = Parse::Parser::parse(filename);
        sceneObject->scene = scene.release();

        return 0;
    }

    static void Scene_dealloc(SceneObject *sceneObject)
    {
        if(sceneObject->scene) {
            delete sceneObject->scene;
        }
    }

    int Framebuffer_getBuffer(PyObject *exporter, Py_buffer *view, int flags)
    {
        FramebufferObject *framebufferObject = (FramebufferObject*)exporter;

        return PyBuffer_FillInfo(view, exporter, framebufferObject->bits, framebufferObject->width*framebufferObject->height*3, 1, flags);
    }

    static PyBufferProcs Framebuffer_as_buffer = {
        Framebuffer_getBuffer,
        NULL
    };

    static PyMemberDef Framebuffer_members[] = {
        {"width", T_UINT, offsetof(FramebufferObject, width), 0},
        {"height", T_UINT, offsetof(FramebufferObject, height), 0},
        {"bits", T_OBJECT, offsetof(FramebufferObject, bits), 0},
        {NULL}
    };

    static struct PyModuleDef raytracemodule = {
        PyModuleDef_HEAD_INIT,
        "raytrace",
        NULL,
        -1
    };

    PyObject *InitPythonInterface()
    {
        EngineType = { PyVarObject_HEAD_INIT(NULL, 0) };
        EngineType.tp_name = "raytrace.Engine";
        EngineType.tp_basicsize = sizeof(EngineObject);
        EngineType.tp_flags = Py_TPFLAGS_DEFAULT;
        EngineType.tp_new = PyType_GenericNew;
        EngineType.tp_init = Engine_init;
        EngineType.tp_methods = Engine_methods;
        EngineType.tp_members = Engine_members;
        EngineType.tp_dealloc = (destructor)Engine_dealloc;

        if(PyType_Ready(&EngineType)) {
            return NULL;
        }

        SettingsType = { PyVarObject_HEAD_INIT(NULL, 0) };
        SettingsType.tp_name = "raytrace.Settings";
        SettingsType.tp_basicsize = sizeof(SettingsObject);
        SettingsType.tp_flags = Py_TPFLAGS_DEFAULT;
        SettingsType.tp_new = PyType_GenericNew;
        SettingsType.tp_members = Settings_members;

        if(PyType_Ready(&SettingsType)) {
            return NULL;
        }

        SceneType = { PyVarObject_HEAD_INIT(NULL, 0) };
        SceneType.tp_name = "raytrace.Scene";
        SceneType.tp_basicsize = sizeof(SceneObject);
        SceneType.tp_flags = Py_TPFLAGS_DEFAULT;
        SceneType.tp_new = PyType_GenericNew;
        SceneType.tp_init = Scene_init;
        SceneType.tp_dealloc = (destructor)Scene_dealloc;

        if(PyType_Ready(&SceneType)) {
            return NULL;
        }

        FramebufferType = { PyVarObject_HEAD_INIT(NULL, 0) };
        FramebufferType.tp_name = "raytrace.Framebuffer";
        FramebufferType.tp_basicsize = sizeof(FramebufferObject);
        FramebufferType.tp_flags = Py_TPFLAGS_DEFAULT;
        FramebufferType.tp_new = PyType_GenericNew;
        FramebufferType.tp_members = Framebuffer_members;
        FramebufferType.tp_as_buffer = &Framebuffer_as_buffer;

        if(PyType_Ready(&FramebufferType)) {
            return NULL;
        }

        PyObject *m = PyModule_Create(&raytracemodule);
        if(!m) {
            return NULL;
        }

        Py_INCREF(&EngineType);
        if(PyModule_AddObject(m, "Engine", (PyObject*)&EngineType) < 0) {
            Py_DECREF(&EngineType);
            Py_DECREF(m);
            return NULL;
        }

        Py_INCREF(&SettingsType);
        if(PyModule_AddObject(m, "Settings", (PyObject*)&SettingsType) < 0) {
            Py_DECREF(&SettingsType);
            Py_DECREF(m);
            return NULL;
        }

        Py_INCREF(&SceneType);
        if(PyModule_AddObject(m, "Scene", (PyObject*)&SceneType) < 0) {
            Py_DECREF(&SceneType);
            Py_DECREF(m);
            return NULL;
        }

        Py_INCREF(&FramebufferType);
        if(PyModule_AddObject(m, "Framebuffer", (PyObject*)&FramebufferType) < 0) {
            Py_DECREF(&FramebufferType);
            Py_DECREF(m);
            return NULL;
        }

        return m;
    }
}
