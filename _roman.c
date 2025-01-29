#include <Python.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Operator precedence helper
static int precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    return 0;
}

// Function to evaluate mathematical expressions
static int solve_expression(const char *expr) {
    int values[100], vtop = -1;
    char ops[100]; int otop = -1;
    
    for (size_t i = 0; expr[i]; i++) {
        if (isdigit(expr[i])) {
            int num = 0;
            while (isdigit(expr[i])) {
                num = num * 10 + (expr[i] - '0');
                i++;
            }
            i--; // Adjust for loop increment
            values[++vtop] = num;
        } else if (expr[i] == '(') {
            ops[++otop] = expr[i];
        } else if (expr[i] == ')') {
            while (otop >= 0 && ops[otop] != '(') {
                char op = ops[otop--];
                int b = values[vtop--], a = values[vtop--];
                values[++vtop] = (op == '+') ? (a + b) : (op == '-') ? (a - b) : (op == '*') ? (a * b) : (a / b);
            }
            otop--; // Remove '('
        } 
        else if(expr[i] == '['){
            ops[++otop] = expr[i];

        } 
        else if(expr[i] == ']'){
            while (otop >= 0 && ops[otop] != '[') {
                char op = ops[otop--];
                int b = values[vtop--], a = values[vtop--];
                values[++vtop] = (op == '+') ? (a + b) : (op == '-') ? (a - b) : (op == '*') ? (a * b) : (a / b);
            }
            otop--; // Remove '['
        } else if (strchr("+-*/", expr[i])) {
            while (otop >= 0 && precedence(ops[otop]) >= precedence(expr[i])) {
                char op = ops[otop--];
                int b = values[vtop--], a = values[vtop--];
                values[++vtop] = (op == '+') ? (a + b) : (op == '-') ? (a - b) : (op == '*') ? (a * b) : (a / b);
            }
            ops[++otop] = expr[i];
        }
    }
    while (otop >= 0) {
        char op = ops[otop--];
        int b = values[vtop--], a = values[vtop--];
        values[++vtop] = (op == '+') ? (a + b) : (op == '-') ? (a - b) : (op == '*') ? (a * b) : (a / b);
    }
    return values[vtop];
}

// Python wrapper for solve_expression
static PyObject* solve(PyObject *self, PyObject *args) {
    const char *expression;
    if (!PyArg_ParseTuple(args, "s", &expression)) return NULL;
    return PyLong_FromLong(solve_expression(expression));
}

// Python wrapper for roman_to_int
static PyObject* py_roman_to_int(PyObject *self, PyObject *args) {
    const char *roman;
    if (!PyArg_ParseTuple(args, "s", &roman)) return NULL;
    return PyLong_FromLong(roman_to_int(roman));
}



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
        }
        else if(s[i] == '(' || s[i] == ')' || s[i] == '[' || s[i] == ']'){
            strncat(res, &s[i], 1);
            i++;
        }
        else {
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

// Converts integer to Roman numeral
char* int_to_roman(int num) {
    struct { int value; const char *numeral; } map[] = {
        {1000, "M"}, {900, "CM"}, {500, "D"}, {400, "CD"}, {100, "C"},
        {90, "XC"}, {50, "L"}, {40, "XL"}, {10, "X"}, {9, "IX"},
        {5, "V"}, {4, "IV"}, {1, "I"}
    };

    char *res = (char*)malloc(20);  // Large enough for max value (3999)
    if (!res) return NULL;
    res[0] = '\0';

    for (int i = 0; i < 13 && num > 0; i++) {
        while (num >= map[i].value) {
            strcat(res, map[i].numeral);
            num -= map[i].value;
        }
    }
    return res;
}

// Python wrapper for int_to_roman
static PyObject* py_int_to_roman(PyObject* self, PyObject* args) {
    int number;
    if (!PyArg_ParseTuple(args, "i", &number)) return NULL;

    if (number <= 0 || number > 3999) {
        PyErr_SetString(PyExc_ValueError, "Number must be between 1 and 3999.");
        return NULL;
    }

    char* res = int_to_roman(number);
    if (!res) return NULL;

    PyObject* result = PyUnicode_FromString(res);
    free(res);
    return result;
}


static PyObject* _hello_world(PyObject* self) {
    return PyUnicode_FromString("Hello World!");
}

static struct PyMethodDef methods[] = {
    {"hello_world", (PyCFunction)_hello_world, METH_NOARGS, "Returns Hello World!"},
    {"convert", roman_handler, METH_VARARGS, "Convert Roman numeral equation to integer equation."},
    {"solve", solve, METH_VARARGS, "Solves a numeric equation"},
    {"roman_to_int", py_roman_to_int, METH_VARARGS, "Gives me the integer value of a Roman numeral"},
    {"int_to_roman", py_int_to_roman, METH_VARARGS, "Gives me the roman version of an integer"},
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
