#ifndef CMY_CONTACT_PLANE
#define CMY_CONTACT_PLANE
#include <object.h>

struct CMyContactPlane : public ContactPlane
{
public:
    CMyContactPlane(const Vec & normal, double offset);
    void setPlaneColor(glm::vec3 color1, glm::vec3 color2);

    void generateBuffers();
#ifdef GRAPHICS
    ~CMyContactPlane() {
        glDeleteBuffers(1, &vertices);
        glDeleteBuffers(1, &colors);
    }

    glm::vec3 _color1, _color2;
#endif
};
#endif // CMY_CONTACT_PLANE
