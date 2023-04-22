#include "Render/Engine.hpp"
#include "Object/Scene.hpp"
#include "Parse/Parser.hpp"

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
        Render::Engine *engine;
    };

    struct SettingsObject {
        PyObject_HEAD
        unsigned int width;
        unsigned int height;
        bool lighting;
        unsigned int minSamples;
        unsigned int maxSamples;
        float sampleThreshold;
        bool irradianceCaching;
        unsigned int indirectSamples;
        float irradianceCacheThreshold;
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

    class Listener : public Render::Engine::Listener
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

        void onRenderDone()
        {
            PyObject_CallMethod(mListenerObject, "on_render_done", NULL);
        }
        void onRenderStatus(const char *message)
        {
            PyObject_CallMethod(mListenerObject, "on_render_status", "s", message);
        }

    private:
        PyObject *mListenerObject;
    };

    static int Engine_init(PyObject *self, PyObject *args, PyObject *kwds)
    {
        EngineObject *engineObject = (EngineObject*)self;

        if (!PyArg_ParseTuple(args, "O", &engineObject->sceneObject)) {
            return -1;
        }

        Py_INCREF(engineObject->sceneObject);

        engineObject->engine = new Render::Engine(*engineObject->sceneObject->scene);

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

        if(engineObject->engine) {
            delete engineObject->engine;
        }
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

        engineObject->engine->startRender(engineObject->listener);

        Py_RETURN_NONE;
    }

    static PyObject *Engine_stop(PyObject *self, PyObject *Py_UNUSED(ignored))
    {
        EngineObject *engineObject = (EngineObject*)self;

        engineObject->engine->stop();

        Py_RETURN_NONE;
    }

    static PyObject *Engine_setSettings(PyObject *self, PyObject *args)
    {
        EngineObject *engineObject = (EngineObject*)self;
        SettingsObject *settingsObject;

        if (!PyArg_ParseTuple(args, "O", &settingsObject))
            return NULL;

        Render::Settings settings;

        settings.width = settingsObject->width;
        settings.height = settingsObject->height;
        settings.lighting = settingsObject->lighting;
        settings.minSamples = settingsObject->minSamples;
        settings.maxSamples = settingsObject->maxSamples;
        settings.sampleThreshold = settingsObject->sampleThreshold;
        settings.lighterSettings.irradianceCaching = settingsObject->irradianceCaching;
        settings.lighterSettings.indirectSamples = settingsObject->indirectSamples;
        settings.lighterSettings.irradianceCacheThreshold = settingsObject->irradianceCacheThreshold;

        engineObject->engine->setSettings(settings);

        Py_XDECREF(engineObject->renderFramebufferObject);
        engineObject->renderFramebufferObject = wrapFramebuffer(engineObject->engine->renderFramebuffer());

        Py_XDECREF(engineObject->sampleStatusFramebufferObject);
        engineObject->sampleStatusFramebufferObject = wrapFramebuffer(engineObject->engine->sampleStatusFramebuffer());

        Py_RETURN_NONE;
    }

    static PyObject *Engine_rendering(PyObject *self, PyObject *Py_UNUSED(ignored))
    {
        EngineObject *engineObject = (EngineObject*)self;

        return PyBool_FromLong(engineObject->engine->rendering());
    }

    static PyMethodDef Engine_methods[] = {
        {"start_render", (PyCFunction) Engine_startRender, METH_VARARGS, ""},
        {"stop", (PyCFunction) Engine_stop, METH_NOARGS, ""},
        {"set_settings", (PyCFunction) Engine_setSettings, METH_VARARGS, ""},
        {"rendering", (PyCFunction) Engine_rendering, METH_NOARGS, ""},
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
        {"lighting", T_BOOL, offsetof(SettingsObject, lighting), 0},
        {"min_samples", T_UINT, offsetof(SettingsObject, minSamples), 0},
        {"max_samples", T_UINT, offsetof(SettingsObject, maxSamples), 0},
        {"sample_threshold", T_FLOAT, offsetof(SettingsObject, sampleThreshold), 0},
        {"irradiance_caching", T_BOOL, offsetof(SettingsObject, irradianceCaching), 0},
        {"indirect_samples", T_UINT, offsetof(SettingsObject, indirectSamples), 0},
        {"irradiance_cache_threshold", T_FLOAT, offsetof(SettingsObject, irradianceCacheThreshold), 0},
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
