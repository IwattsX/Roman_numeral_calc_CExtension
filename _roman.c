#include <Python.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


int roman_to_int(const char* roman) {
    regex_t regex;
    if (regcomp(&regex, "^M{0,3}(CM|CD|D?C{0,3})(XC|XL|L?X{0,3})(IX|IV|V?I{0,3})$", REG_EXTENDED) != 0) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to compile regex.");
        return -1;
    }

    if (regexec(&regex, roman, 0, NULL, 0) != 0) {
        regfree(&regex);
        PyErr_SetString(PyExc_ValueError, "Invalid Roman numeral.");
        return -1;
    }
    
    regfree(&regex);

    int result = 0;
    int prevValue = 0;
    
    for (int i = strlen(roman) - 1; i >= 0; i--) {
        int currentValue = 0;

        switch (roman[i]) {
            case 'I': currentValue = 1; break;
            case 'V': currentValue = 5; break;
            case 'X': currentValue = 10; break;
            case 'L': currentValue = 50; break;
            case 'C': currentValue = 100; break;
            case 'D': currentValue = 500; break;
            case 'M': currentValue = 1000; break;
            default: 
                PyErr_SetString(PyExc_ValueError, "Invalid character in Roman numeral.");
                return -1;
        }

        if (currentValue < prevValue) {
            result -= currentValue;
        } else {
            result += currentValue;
        }

        prevValue = currentValue;
    }

    return result;
}

char* roman_to_equation(const char* s) {
    size_t len = strlen(s);
    char* res = (char*)malloc(len * 4 + 1);  // Allocate enough space for worst-case scenario
    if (!res) {
        PyErr_SetString(PyExc_MemoryError, "Memory allocation failed.");
        return NULL;
    }
    res[0] = '\0';

    size_t i = 0;
    while (i < len) {
        if (isspace(s[i])) {
            i++;
        } else if (strchr("+-*/", s[i])) {
            strncat(res, &s[i], 1);
            i++;
        } else if (isalpha(s[i])) {
            char tmp[16] = {0};  // Temporary buffer for a Roman numeral
            int j = 0;
            
            while (isalpha(s[i]) && j < 15) {
                tmp[j++] = s[i++];
            }
            tmp[j] = '\0';

            int num = roman_to_int(tmp);
            if (num == -1) {
                free(res);
                return NULL;
            }

            char tmp_int[16];
            snprintf(tmp_int, sizeof(tmp_int), "%d", num);
            strcat(res, tmp_int);
        } else {
            PyErr_SetString(PyExc_ValueError, "Invalid character in equation.");
            free(res);
            return NULL;
        }
    }

    return res;
}

static PyObject* roman_handler(PyObject* self, PyObject* args) {
    const char* roman_equation;

    if (!PyArg_ParseTuple(args, "s", &roman_equation)) {
        return NULL;
    }

    char* equation = roman_to_equation(roman_equation);
    if (!equation) {
        return NULL;
    }

    PyObject* result = PyUnicode_FromString(equation);
    free(equation);
    return result;
}

static PyObject* _hello_world(PyObject* self) {
    return PyUnicode_FromString("Hello World!");
}

static struct PyMethodDef methods[] = {
    {"hello_world", (PyCFunction)_hello_world, METH_NOARGS, "Returns Hello World!"},
    {"convert", roman_handler, METH_VARARGS, "Convert Roman numeral equation to integer equation."},
    {NULL, NULL, 0, NULL}
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
