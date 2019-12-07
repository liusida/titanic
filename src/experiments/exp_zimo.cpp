// An example of two robots proposed by my son :)
//
#include <iostream>
#include <vector>

#include <sim.h>

using namespace std;

int main() {
    double d = 0.9996; //damping, so close to 1.0, but if use default value 1.0, everything bounces annoyingly
    Simulation sim;
    sim.createPlane(Vec(0, 0, 1), -1); // create constraint plane

    //First one:
    vector<Vec> tops = {Vec(0,0,1), Vec(0,0,-1), Vec(1,0,0), Vec(0,1,0), Vec(-1,0,0), Vec(0,-1,0)};
    vector<vector<unsigned>> edges = {{0,2},{0,3},{0,4},{0,5}, {1,2},{1,3},{1,4},{1,5}, {2,4},{3,5}, {2,3},{3,4},{4,5},{5,2}};
    vector<Mass *> masses;
    for (auto t:tops) {
        masses.push_back(sim.createMass(t));
    }
    for (auto m:masses) {
        m->damping = d;
    }
    vector<Spring *> springs;
    for (auto e:edges) {
        springs.push_back(sim.createSpring(masses[e[0]], masses[e[1]]));
    }
    for (auto s:springs) {
        s->_omega = rand()%4;
        s->_type = rand()%4;
    }

    //Second one:
    Cube * c = sim.createCube(Vec(0,0,0), 1);
    for (auto m:c->masses) {
        m->damping = d;
    }
    for (auto s:c->springs) {
        s->_omega = rand()%4;
        s->_type = rand()%4;
    }

    sim.start();

  while(1) {
      sim.getAll();
      std::cout<<"Reading #1 Robot position: "<<masses[0]->pos.norm()<<std::endl;
      std::cout<<"Reading #2 Robot position: "<<c->masses[0]->pos.norm()<<std::endl;
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }

}