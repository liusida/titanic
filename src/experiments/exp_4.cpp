#include<iostream>
#include <sim.h>
using namespace std;

int main() {
    Simulation sim; // create the basic simulation object.
    
    sim.setViewport(Vec(0, 0, 40), Vec(0, 0, 0), Vec(0, 1, 0)); // Top View
    //sim.setViewport(Vec(80, 0, 0), Vec(0, 0, 0), Vec(0, 0, 1)); // Side View

    //sim.createBall(Vec(0,0,20), 3);
    //Lattice * l1 = sim.createLattice(Vec(0, 0, 20), Vec(1, 1, 1), 5, 5, 5);
    //Lattice * l2 = sim.createLattice(Vec(2, -2, 20), Vec(2, 2, 2), 5, 5, 5);
    Lattice * l3 = sim.createLattice(Vec(0, 0, 3), Vec(2, 2, 2), 5, 5, 5);
    
    sim.createPlane(Vec(0, 0, 1), 0);

    //l1->masses[0]->color = Vec(0.,1.,0.);
    //l1 -> masses[0] -> addConstraint(CONSTRAINT_PLANE, Vec(0, 0, 1), -1.0);
    //l2->masses[0]->color = Vec(0.,0.,1.);
    //l2 -> masses[0] -> addConstraint(CONTACT_PLANE, Vec(0, 0, 1), 5);
    l3->masses[0]->color = Vec(0.,1.,1.);
    l3 -> masses[0] -> addConstraint(DIRECTION, Vec(0,1,1), -0.25);
    
    
    //CONSTRAINT_PLANE : normal direction to the plane, -friction; fix the point on that plane with certain friction.
    //CONTACT_PLANE : normal direction to the plane, offset; a visible contact plane [=sim.createPlane(Normal, Offset)].
    //BALL: center of mass, radius; a invisible ball [=sim.createBall(CoM, Radius)].
    //DIRECTION: direction, -friction; constrain the mass in a line with friction.

    //l1 -> masses[20] -> addConstraint(DIRECTION, Vec(0, 1, 1), 0);
    //l1->masses[20]->addConstraint(CONTACT_PLANE, Vec(0, 0, 1), 0);
    //l1 -> masses[0] -> setDrag(100); // add drag to the mass with drag coefficient C equal to 100.

    sim.start();

    while (sim.time() < 10.0) {
        sim.pause(sim.time() + 1.0);
        sim.getAll();
        //sim.get(l1); // get lattice data from the GPU;
        cout<< l3->masses[0]->pos[0] <<","<< l3->masses[0]->pos[1] <<","<< l3->masses[0]->pos[2] <<","<< endl;
        //sim.setViewport(Vec(30+sim.time(), -30, 10), Vec(0, 0, 10), Vec(0, 0, 1)); // move the viewport so you can see the cubes
        //l1 -> setSpringConstants(10000 * exp(-sim.time() / 3)); // exponential decay of the spring constant
        //sim.set(l1); // push the lattice data to the GPU;

        sim.resume();
    }

    sim.stop();
}