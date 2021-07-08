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
The half-edge mesh data structure is largely based off of the [paper](imr.sandia.gov/papers/imr20/Sieger.pdf) by Mario Botsch, Daniel Sieger, (2011), "Design, Implementation, and Evaluation of the Surface_mesh data structure".

- [geometry-central](https://github.com/nmwsharp/geometry-central):
    A very good mesh data structure based off of the same principles.
    ([See this SGP 2020 video](https://www.youtube.com/watch?v=mw5Xz9CFZ7A))
- [libigl](https://github.com/libigl/libigl):
    A very good geometry processing algorithms library, with an OpenGL viewer. The core data structure is a simple
    vertex and incidence list.
    ([See this SGP 2020 video](https://www.youtube.com/watch?v=OSQWCABxLsM))
- [pmp-library, The Polygon Mesh Processing library](https://github.com/pmp-library/pmp-library)
    ([See this SGP 2020 video](https://www.youtube.com/watch?v=RXc9af0Rq8s))
- [OpenMesh](https://www.graphics.rwth-aachen.de/software/openmesh/)
- [OpenVolumeMesh](https://www.graphics.rwth-aachen.de/software/openvolumemesh/)


## The library
-----------------------
# SurfaceMesh


