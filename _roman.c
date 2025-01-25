#include <Python.h>

static PyObject* _hello_world(PyObject* self){
	return PyUnicode_FromString("Hello World!");
}

static struct PyMethodDef methods[] = {
	{"hello_world", (PyCFunction)_hello_world, METH_NOARGS},
	{NULL, NULL}
};

static struct PyModuleDef module = {
	PyModuleDef_HEAD_INIT,
	"_roman",
	NULL,
	-1,
	methods
};

PyMODINIT_FUNC PyInit__roman(void) {
	return PyModule_Create(&module);
}
