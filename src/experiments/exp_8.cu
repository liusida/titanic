/*
1. Sprinkle masses along the floor randomly.

2. Apply external forces according to an arbitrary 2d function/field (like a magnetic force) that makes them move along the floor in different directions and speeds according to the flow of the function.

3. When two masses touch there is a probability based on their velocities that they stick together permanently with a spring.
*/
#include <iostream>
using namespace std;
#include <sim.h>

#define random_double_number ((double) rand()/RAND_MAX)

__device__ Vec sam_field_force(CUDA_MASS * m) {
    Vec f = -0.1*m->pos;
    f[2] = 0;
    return f;
}
__device__ Vec gravity_force(CUDA_MASS * m) {
    return m->m * Vec(0,0,-9.81);
}
__device__ funcptr f_sam_field = sam_field_force ;
__device__ funcptr f_gravity = gravity_force;

int main() {
    funcptr h_sam_field;
    funcptr h_gravity;

    if (cudaSuccess != cudaMemcpyFromSymbol (&h_sam_field, f_sam_field, sizeof (funcptr)))
        printf ("FAILED to get SYMBOL\n");
    if (cudaSuccess != cudaMemcpyFromSymbol (&h_gravity, f_gravity, sizeof (funcptr)))
        printf ("FAILED to get SYMBOL\n");

    Simulation sim;
#ifdef GRAPHICS
    sim.setViewport(Vec(0,0,20), Vec(0,0,0), Vec(0,1,0));
#endif
    sim.setGlobalAcceleration(Vec(0,0,0));
    sim.createPlane(Vec(0,0,1), 0);
    sim.createField(h_sam_field);
    sim.createField(h_gravity);
    Mass * m_collision_1 = sim.createMass(Vec(0.1,0,0));
    Mass * m_collision_2 = sim.createMass(Vec(-0.1,0,0));
    Spring * s_collision = sim.createSpring(m_collision_1, m_collision_2);
    s_collision->_rest = (0.2); 

    Mass * m1 = sim.createMass(Vec(0,1,0));
    Mass * m2 = sim.createMass(Vec(0,1,0));
    // for (unsigned i=0;i<2;i++)
    //     sim.createMass(Vec(random_double_number*10,random_double_number*10,0));
    m1->vel = Vec(3,0,0);
    sim.start();
    // while(1) {
    //     sim.pause(sim.time()+1.3);
    //     sim.getAll();
    //     cout<<"Sim Time: "<<sim.time()<<endl;
    //     sim.printPositions();
    //     sim.resume();
    // }
}