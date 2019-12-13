#include <collision.h>


void Collision::operator=(CUDA_COLLISION & d_c) {
    _left_index = d_c._left_index;
    _right_index = d_c._right_index;
    strength = d_c.strength;
}

void Collision::operator=(Collision & c) {
    _left_index = c._left_index;
    _right_index = c._right_index;
    strength = c.strength;
}

bool Collision::operator==(Collision &c) {
    return ( _left_index == c._left_index && _right_index == c._right_index && strength == c.strength );
}