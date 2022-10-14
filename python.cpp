#include "Object/Scene.hpp"
#include "Parse/Parser.hpp"

#undef _DEBUG

#include <Python.h>
#include <structmember.h>

struct EngineObject {
    PyObject_HEAD
    PyObject *settings;
    PyObject *render_framebuffer;
    PyObject *sample_status_framebuffer;
};

static PyObject *Engine_startRender(PyObject *self, PyObject *args)
{
    return self;
}

static PyObject *Engine_stop(PyObject *self, PyObject *Py_UNUSED(ignored))
{
    return self;
}


static PyMethodDef Engine_methods[] = {
    {"startRender", (PyCFunction) Engine_startRender, METH_VARARGS, ""},
    {"stop", (PyCFunction) Engine_stop, METH_NOARGS, ""},
    {NULL}  /* Sentinel */
};

static PyMemberDef Engine_members[] = {
    {"settings", T_OBJECT_EX, offsetof(EngineObject, settings), 0},
    {"render_framebuffer", T_OBJECT_EX, offsetof(EngineObject, render_framebuffer), 0},
    {"sample_status_framebuffer", T_OBJECT_EX, offsetof(EngineObject, settings), 0},
    {NULL}  /* Sentinel */
};

struct SettingsObject {
    PyObject_HEAD
    unsigned int width;
    unsigned int height;
    bool lighting;
    unsigned int minSamples;
    unsigned int maxSamples;
    float sampleThreshold;
};

static PyMemberDef Settings_members[] = {
    {"width", T_UINT, offsetof(SettingsObject, width), 0},
    {"height", T_UINT, offsetof(SettingsObject, height), 0},
    {"lighting", T_BOOL, offsetof(SettingsObject, lighting), 0},
    {"min_samples", T_UINT, offsetof(SettingsObject, minSamples), 0},
    {"max_samples", T_UINT, offsetof(SettingsObject, maxSamples), 0},
    {"sample_threshold", T_FLOAT, offsetof(SettingsObject, sampleThreshold), 0},
    {NULL}  /* Sentinel */
};

static int Settings_init(PyObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"width", "height", "lighting", "min_samples", "max_samples", "sample_threshold", NULL};
    SettingsObject *settings = (SettingsObject*)self;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|IIpIIf", kwlist,
                                     &settings->width, &settings->height, &settings->lighting, &settings->minSamples, &settings->maxSamples, &settings->sampleThreshold))
        return -1;

    return 0;
}

struct SceneObject {
    PyObject_HEAD
    Object::Scene *scene;
};

static int Scene_init(PyObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"filename", NULL};
    SceneObject *scene = (SceneObject*)self;

    char *filename;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist,
                                     &filename))
        return -1;

    printf("Filename: %s\n", filename);
    std::unique_ptr<Object::Scene> s = Parse::Parser::parse(filename);
    //scene->scene = s.release();

    return 0;
}

static struct PyModuleDef pyraymodule = {
    PyModuleDef_HEAD_INIT,
    "pyray",
    NULL,
    -1
};

extern "C" {
    PyMODINIT_FUNC
    PyInit_pyray(void)
    {
        static PyTypeObject EngineType = { PyVarObject_HEAD_INIT(NULL, 0) };
        EngineType.tp_name = "pyray.Engine";
        EngineType.tp_basicsize = sizeof(EngineObject);
        EngineType.tp_flags = Py_TPFLAGS_DEFAULT;
        EngineType.tp_new = PyType_GenericNew;
        EngineType.tp_methods = Engine_methods;
        EngineType.tp_members = Engine_members;
        
        if(PyType_Ready(&EngineType)) {
            return NULL;
        }

        static PyTypeObject SettingsType = { PyVarObject_HEAD_INIT(NULL, 0) };
        SettingsType.tp_name = "pyray.Settings";
        SettingsType.tp_basicsize = sizeof(SettingsObject);
        SettingsType.tp_flags = Py_TPFLAGS_DEFAULT;
        SettingsType.tp_new = PyType_GenericNew;
        SettingsType.tp_init = Settings_init;
        SettingsType.tp_members = Settings_members;
        
        if(PyType_Ready(&SettingsType)) {
            return NULL;
        }

        static PyTypeObject SceneType = { PyVarObject_HEAD_INIT(NULL, 0) };
        SceneType.tp_name = "pyray.Scene";
        SceneType.tp_basicsize = sizeof(SceneObject);
        SceneType.tp_flags = Py_TPFLAGS_DEFAULT;
        SceneType.tp_new = PyType_GenericNew;
        SceneType.tp_init = Scene_init;
        
        if(PyType_Ready(&SceneType)) {
            return NULL;
        }

        PyObject *m = PyModule_Create(&pyraymodule);
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

        return m;
    }
}