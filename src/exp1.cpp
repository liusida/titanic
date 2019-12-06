#include <iostream>
#include <chrono>
#include <thread>

#include <sim.h>

int main() {
  Simulation sim;

  Mass *a = sim.createMass(Vec(0,0,2));
  Mass *b = sim.createMass(Vec(0,0,1));
  sim.createSpring(a, b);
  sim.createPlane(Vec(0, 0, 1), 0); // create constraint plane

  sim.start();
  std::cout<<"simulation Thread Started."<<std::endl;

  while(1) {
      sim.getAll();
      std::cout<<"Reading a position: "<<a->pos<<std::endl;
      std::cout<<"Reading b position: "<<b->pos<<std::endl;
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }

}
