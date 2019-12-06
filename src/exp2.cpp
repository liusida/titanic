#include <iostream>

#include <sim.h>

int main() {
  Simulation sim;

  Mass *a = sim.createMass(Vec(0,1,1));
  Mass *b = sim.createMass(Vec(0,0,1));
  sim.createSpring(a, b);
  sim.createPlane(Vec(0, 0, 1), 0); // create constraint plane

  sim.start();
  std::cout<<"simulation started."<<std::endl;
}
