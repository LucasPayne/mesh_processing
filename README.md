## mesh_processing
------------------------
This library contains implementations of data structures and algorithms for geometry and mesh processing.
Other things that could be implemented here are curves and surface methods such as subdivision surfaces.

## Including mesh_processing in an application
-----------------------
<details>
mesh_processing is built with CMake and requires C++11.
First, make sure all dependencies are available on your system. Create a new directory called "build", and
run
```
    cmake ..
```
to generate build files. On Linux systems, continue with
```
    make
```
to build the library and example applications. To include mesh_processing in a C++ application,
simply include mesh_processing.h and link against the compiled library.
</details>

#Dependencies
-----------------------
[Eigen3 (C++ header-only linear algebra library)](https://gitlab.com/libeigen/eigen)

(Update CMakeLists.txt to point to the Eigen3 header files if needed.)


## The library
-----------------------


