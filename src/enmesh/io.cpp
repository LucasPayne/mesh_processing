#include "mesh_processing.h"
namespace Enmesh {

SurfaceGeometry load_geometry(const std::string &filename)
{
    std::ifstream in;
    in.open(filename, std::ios::in);
    std::cout << filename << "\n";
    assert(in);

    
    // next_line() returns a string of the next non-comment non-whitespace line, with stripped whitespace.
    // If EOF is reached, nullptr is returend.
    std::string line_buf;
    const unsigned int buf_size = 1024;
    char buf[buf_size+1];
    unsigned int line_number = 0;
    auto Assert = [&](bool val) {
        if (!val) printf("Failure at line: %d\n", line_number);
        assert(val);
    };
    auto next_line = [&]()->char *{
        while (true) {
            std::getline(in, line_buf);
            if (in.eof()) return nullptr;
            line_number += 1;
            unsigned int i = 0;
            for (; i < line_buf.length() && line_buf[i] == ' '; i++);
            if (i == line_buf.length() || line_buf[i] == '#') continue;
            // Copy the line_buf into a buffer and strip trailing comments.
            unsigned int j = i;
            for (; j < line_buf.length() && j-i < buf_size && line_buf[j] != '#'; j++) {
                buf[j-i] = line_buf[j];
            }
            if (j-i > buf_size) {
                std::cerr << "Line too long.\n";
                Assert(0);
            }
            buf[j-i] = '\0';
            unsigned int k = j-i;
            while (k > 0) {
                --k;
                if (buf[j-i] == ' ') {
                    buf[j-i] = '\0';
                } else {
                    break;
                }
            }
            return buf;
        }
    };

    SurfaceGeometry geom;
    std::vector<Vertex> vertices;

    char *line;
    while ((line = next_line()) != nullptr) {
        if (strcmp(line, "Vertices") == 0) {
            line = next_line(); Assert(line);
            unsigned int num_vertices;
            Assert(sscanf(line, "%u", &num_vertices) == 1);
            for (unsigned int i = 0; i < num_vertices; i++) {
                line = next_line(); Assert(line);
                float x, y, z;
                puts(line);
                Assert(sscanf(line, "%f %f %f", &x, &y, &z) == 3);
                vertices.push_back(geom.add_vertex(x, y, z));
            }
        } else if (strcmp(line, "Triangles") == 0) {
            Assert(vertices.size() > 0);
            line = next_line(); Assert(line);
            unsigned int num_triangles;
            Assert(sscanf(line, "%u", &num_triangles) == 1);
            for (unsigned int i = 0; i < num_triangles; i++) {
                line = next_line(); Assert(line);
                unsigned int a, b, c;
                unsigned int extra;
                Assert(sscanf(line, "%u %u %u %u", &a, &b, &c, &extra) == 4);
                a--; b--; c--; // medit indices start at 1.
                Assert(a < vertices.size() && b < vertices.size() && c < vertices.size());
                geom.add_triangle(vertices[a], vertices[b], vertices[c]);
            }
        }
    }
    in.close();
    return geom;
}


} // namespace Enmesh
