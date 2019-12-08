#include <cmy_contact_plane.h>

CMyContactPlane::CMyContactPlane(const Vec & normal, double offset) :
    ContactPlane(normal, offset),
    _color1({.5,.4,.5}),
    _color2({.5,.5,.4})
{
}

void CMyContactPlane::setPlaneColor(glm::vec3 color1, glm::vec3 color2) {
    _color1 = color1;
    _color2 = color2;
}

void CMyContactPlane::generateBuffers() {
    Vec temp = (dot(_normal, Vec(0, 1, 0)) < 0.8) ? Vec(0, 1, 0) : Vec(1, 0, 0);

    Vec v1 = cross(_normal, temp); // two unit vectors along plane
    v1 = v1 / v1.norm();

    Vec v2 = cross(_normal, v1);
    v2 = v2 / v2.norm();

    const static GLfloat vertex_buffer_platform[118] = {
            -1, -1, -1,
            -1, -1,  1,
            -1,  1,  1,
            1,  1, -1,
            -1, -1, -1,
            -1,  1, -1,
            1, -1,  1,
            -1, -1, -1,
            1, -1, -1,
            1,  1, -1,
            1, -1, -1,
            -1, -1, -1,
            -1, -1, -1,
            -1,  1,  1,
            -1,  1, -1,
            1, -1,  1,
            -1, -1,  1,
            -1, -1, -1,
            -1,  1,  1,
            -1, -1,  1,
            1, -1,  1,
            1,  1,  1,
            1, -1, -1,
            1,  1, -1,
            1, -1, -1,
            1,  1,  1,
            1, -1,  1,
            1,  1,  1,
            1,  1, -1,
            -1,  1, -1,
            1,  1,  1,
            -1,  1, -1,
            -1,  1,  1,
            1,  1,  1,
            -1,  1,  1,
            1, -1,  1
    };

    GLfloat vertex_data[108];

    for (int i = 0; i < 36; i++) {
        Vec temp = Vec(vertex_buffer_platform[3 * i], vertex_buffer_platform[3 * i + 1], vertex_buffer_platform[3 * i + 2]);
        Vec vertex = 30 * dot(v1, temp) * v1 + 30 * dot(v2, temp) * v2 + _normal * (_offset + dot(_normal, temp) - 1.0);

        vertex_data[3 * i] = vertex[0];
        vertex_data[3 * i + 1] = vertex[1];
        vertex_data[3 * i + 2] = vertex[2];
    }

    glGenBuffers(1, &vertices); // create buffer for these vertices
    glBindBuffer(GL_ARRAY_BUFFER, vertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);

    GLfloat g_color_buffer_data[108];

    for (int i = 0; i < 36; i++) {
        if (i%2) {
            g_color_buffer_data[3 * i] = _color1[0];
            g_color_buffer_data[3 * i + 1] = _color1[1];
            g_color_buffer_data[3 * i + 2] = _color1[2];
        } else {
            g_color_buffer_data[3 * i] = _color2[0];
            g_color_buffer_data[3 * i + 1] = _color2[1];
            g_color_buffer_data[3 * i + 2] = _color2[2];
        }
    }

    glGenBuffers(1, &colors);
    glBindBuffer(GL_ARRAY_BUFFER, colors);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);

    _initialized = true;
}