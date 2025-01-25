from setuptools import setup
from setuptools import Extension

setup(
        name='roman-num',
        version='1',
        ext_modules=[Extension('_roman', ['_roman.c'])]
)
