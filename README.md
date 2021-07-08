## mesh_processing
------------------------
This library contains implementations of data structures and algorithms for geometry and mesh processing.
Other things that could be implemented here are curves and surface methods such as subdivision surfaces.

## Including mesh_processing in an application
-----------------------
mesh_processing is built with CMake and requires C++11.
First, make sure all dependencies are available on your system. Create a new directory called "build", and
run "cmake ..".
To include mesh_processing in a C++ application,
simply include mesh_processing.h and link against the compiled library.

# Dependencies
-----------------------
- [Eigen3 (C++ header-only linear algebra library. Update CMakeLists.txt to point to the Eigen3 header files if needed.)](https://gitlab.com/libeigen/eigen)
- [TetGen (Hang Si. A Quality Tetrahedral Mesh Generator and a 3D Delaunay Triangulator)](http://wias-berlin.de/software/index.jsp?id=TetGen&lang=1)


# References
-----------------------
The half-edge mesh data structure is largely based off of the [paper](imr.sandia.gov/papers/imr20/Sieger.pdf) by Mario Botsch, Daniel Sieger, (2011), "Design, Implementation, and Evaluation of the Surface_mesh data structure". A very good mesh library based off of these same principles is [geometry-central](https://github.com/nmwsharp/geometry-central).


## The library
-----------------------
# SurfaceMesh


