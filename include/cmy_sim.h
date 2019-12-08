/*
    In order to access titan freely, we should add a hook into titan's Simulation class:

    friend class CMySimulation;
 */
#ifndef CMY_SIM_H
#define CMY_SIM_H

#include <sim.h>
#include <cmy_contact_plane.h>

struct Color {
    float r, g, b, a;
};

class CMySimulation : public Simulation
{
public:
    CMySimulation();
    void createGLFWWindow();
    void start();
    void _run();
    void createPlane(const Vec & abc, double d );
    Constraint* lastConstraint();

    void setSkyColor(Color color ) ;

    Color sky_color;
};

#endif // CMY_SIM_H
