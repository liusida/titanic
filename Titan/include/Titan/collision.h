//
// Created by Sida Liu on Dec 13, 2019.
//

#ifndef TITAN_COLLISION_H
#define TITAN_COLLISION_H

#include <mass.h>

class Collision;
struct CUDA_COLLISION;

class Collision { //It seems to be an Event, not belong to Object or Constraint or Container.
public:
    Collision() = default;
    void operator=(CUDA_COLLISION & d_c);
    int _left_index;
    int _right_index;
    double strength=0; // TODO: what is the physical variable for describing how strong a collision is?
};

struct CUDA_COLLISION
{
    CUDA_COLLISION() = default;
    CUDA_COLLISION(Collision &c):
        _left_index(c._left_index), _right_index(c._right_index), strength(c.strength) {};
    int _left_index;
    int _right_index;
    double strength=0;    
};


#endif //TITAN_COLLISION_H
