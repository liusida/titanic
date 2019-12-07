#include <iostream>
#include <chrono>
#include <thread>

#include <crobot.h>
#include <sim.h>

int main() {
  CRobot r;
  Simulation sim;

  Mass *a = sim.createMass(Vec(0,1,2));
  Mass *b = sim.createMass(Vec(0,0,2));
  Mass *c = sim.createMass(Vec(1,0,2));
  Mass *top = sim.createMass(Vec(0,0,3));
  double d = 0.9996; //damping, so close to 1.0, but if use default value 1.0, everything bounces annoyingly
  a->damping =d;
  b->damping =d;
  c->damping =d;
  top->damping =d;

  Spring *s = sim.createSpring(top, a);
  s->_type = ACTIVE_CONTRACT_THEN_EXPAND;
  s->_omega = 4; //frequency, 4 times per sec
  s = sim.createSpring(top,b);
  s->_type = ACTIVE_CONTRACT_THEN_EXPAND;
  s->_omega = 2;
  sim.createSpring(top,c);
  s->_type = ACTIVE_CONTRACT_THEN_EXPAND;
  s->_omega = 1;
  sim.createSpring(a,c);
  sim.createSpring(b,c);
  sim.createSpring(a,b);

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
