#include "App/PythonInterface.hpp"

int main(int argc, char *argv[])
{
    PyImport_AppendInittab("raytrace", &App::InitPythonInterface);

    PyConfig config;
    PyConfig_InitPythonConfig(&config);
    PyConfig_SetString(&config, &config.run_filename, L"App/app.py");
    Py_InitializeFromConfig(&config);
    Py_RunMain();
    Py_FinalizeEx();

    return 0;
}