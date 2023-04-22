#ifndef APP_PYTHONINTERFACE_HPP
#define APP_PYTHONINTERFACE_HPP

#define PY_SSIZE_T_CLEAN
#undef _DEBUG
#include <Python.h>

namespace App {
    PyObject *InitPythonInterface();
}

#endif