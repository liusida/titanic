#include <collision.h>


void Collision::operator=(CUDA_COLLISION & d_c) {
    _left_index = d_c._left_index;
    _right_index = d_c._right_index;
    strength = d_c.strength;
}