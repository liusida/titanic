//
// Created by Jacob Austin on 5/17/18.
//
#define GLM_FORCE_PURE
#include "mass.h"

#define gpuErrchk(ans) { gpuAssert((ans), __FILE__, __LINE__); }
inline void gpuAssert(cudaError_t code, const char *file, int line, bool abort=false)
{
    if (code != cudaSuccess)
    {
        //fprintf(stderr,"GPUassert: %s %s %d\n", cudaGetErrorString(code), file, line);

        if (abort) {
            char buffer[200];
            snprintf(buffer, sizeof(buffer), "GPUassert error in CUDA kernel: %s %s %d\n", cudaGetErrorString(code), file, line);
            std::string buffer_string = buffer;
            throw std::runtime_error(buffer_string);
            exit(code);
        }
    }
}

Mass::Mass() {
    m = 1.0;
    dt = 0.0001;
    damping = 1.0;
    T = 0;
    valid = true;
    arrayptr = nullptr;
    ref_count = 0;

#ifdef GRAPHICS
    color = Vec(1.0, 0.2, 0.2);
#endif
} // constructor TODO fix timing

void Mass::operator=(CUDA_MASS & mass) {
    m = mass.m;
    dt = mass.dt;
    T = mass.T;
    damping = mass.damping;
    pos = mass.pos;
    vel = mass.vel;
    acc = mass.acc;
    force = mass.force;
    valid = mass.valid;

    ref_count = this -> ref_count;
    arrayptr = this -> arrayptr;
    
    neighbors.clear();
    for (unsigned i=0;i<mass.num_neighbors;i++) {
        neighbors.push_back(mass.arrayptr->neighbors[i]);
    }
#ifdef CONSTRAINTS
    constraints = this -> constraints;
#endif

#ifdef GRAPHICS
    color = mass.color;
#endif
}

Mass::Mass(const Vec & position, double mass, bool fixed, double dt) {
    m = mass;
    pos = position;
    
    this -> dt = dt;

    T = 0;
    damping = 1.0;
    
    valid = true;
    arrayptr = nullptr;
    ref_count = 0;

#ifdef GRAPHICS
    color = Vec(1.0, 0.2, 0.2);
#endif
}

CUDA_MASS::CUDA_MASS(Mass &mass) {
    m = mass.m;
    dt = mass.dt;
    T = mass.T;
    damping = mass.damping;
    
    pos = mass.pos;
    vel = mass.vel;
    acc = mass.acc;
    force = mass.force;
    valid = true;

    arrayptr = &mass;
    //printf("CUDA: %p ->Mass: %p\n", this, arrayptr);
    CUDA_MASS ** temp = new CUDA_MASS * [mass.neighbors.size()];
    for (unsigned i=0;i<mass.neighbors.size();i++) {
        temp[i] = mass.neighbors[i]->arrayptr;
    }
    gpuErrchk(cudaMalloc((void **) &neighbors, sizeof(CUDA_MASS *) * mass.neighbors.size()));
    gpuErrchk(cudaMemcpy(neighbors, temp, mass.neighbors.size() * sizeof(CUDA_MASS *), cudaMemcpyHostToDevice));
    delete temp;

    num_neighbors = mass.neighbors.size();
    //printf("CUDA_MASS(mass): # of neighbors: %d. \n", num_neighbors);

    // for (unsigned i=0;i<mass.neighbors.size();i++) {
    //     neighbors[i] = mass.neighbors[i]->arrayptr;
    //     printf("ptr: %p\n", mass.neighbors[i]->arrayptr);
    // }
#ifdef CONSTRAINTS
    constraints = CUDA_LOCAL_CONSTRAINTS(mass.constraints);
#endif

#ifdef GRAPHICS
    color = mass.color;
#endif
}

#ifdef CONSTRAINTS
void Mass::addConstraint(CONSTRAINT_TYPE type, const Vec & vec, double num) { // TODO make this more efficient
    if (type == 0) {
        this -> constraints.constraint_plane.push_back(CudaConstraintPlane(vec, num));
        this -> constraints.num_constraint_planes++;
        this -> constraints.constraint_plane_ptr = thrust::raw_pointer_cast(constraints.constraint_plane.data());
    } else if (type == 1) {
        this -> constraints.contact_plane.push_back(CudaContactPlane(vec, num));
        this -> constraints.num_contact_planes++;
        this -> constraints.contact_plane_ptr = thrust::raw_pointer_cast(constraints.contact_plane.data());
    } else if (type == 2) {
        this -> constraints.ball.push_back(CudaBall(vec, num));
        this -> constraints.num_balls++;
        this -> constraints.ball_ptr = thrust::raw_pointer_cast(constraints.ball.data());
    } else if (type == 3) {
        this -> constraints.direction.push_back(CudaDirection(vec, num));
        this -> constraints.num_directions++;
        this -> constraints.direction_ptr = thrust::raw_pointer_cast(constraints.direction.data());
    }
}

void Mass::clearConstraints(CONSTRAINT_TYPE type) {
    if (type == 0) {
        this -> constraints.constraint_plane.clear();
        this -> constraints.constraint_plane.shrink_to_fit();
        this -> constraints.num_constraint_planes = 0;
    } else if (type == 1) {
        this -> constraints.contact_plane.clear();
        this -> constraints.contact_plane.shrink_to_fit();
        this -> constraints.num_contact_planes = 0;
    } else if (type == 2) {
        this -> constraints.ball.clear();
        this -> constraints.ball.shrink_to_fit();
        this -> constraints.num_balls = 0;
    } else if (type == 3) {
        this -> constraints.direction.clear();
        this -> constraints.direction.shrink_to_fit();
        this -> constraints.num_directions = 0;
    }
}

void Mass::clearConstraints() {
    clearConstraints(CONSTRAINT_PLANE);
    clearConstraints(CONTACT_PLANE);
    clearConstraints(DIRECTION);
    clearConstraints(BALL);
}

void Mass::fix() {
    this -> constraints.fixed = true;
}
void Mass::unfix() {
    this -> constraints.fixed = false;
}

void Mass::setDrag(double C) {
    this -> constraints.drag_coefficient = C;
}


#endif

void Mass::decrementRefCount() {
    if (--ref_count == 0) {

        if (arrayptr) {
            cudaFree(arrayptr);
        }

        delete this;
    }
}
