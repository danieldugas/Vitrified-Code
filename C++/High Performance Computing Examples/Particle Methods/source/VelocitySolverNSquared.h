
#ifndef __DirectVortexSolver__VelocitySolverNSquared__
#define __DirectVortexSolver__VelocitySolverNSquared__

#include "common.h"
#include "ArrayOfParticles.h"

class VelocitySolverNSquared
{
private:
    ArrayOfParticles & dstParticles;
    ArrayOfParticles & srcParticles;
    
    const int rank, size;
    
public:
	VelocitySolverNSquared(ArrayOfParticles & dstParticles, ArrayOfParticles & srcParticles,const int rank,const int size);
	~VelocitySolverNSquared(){}
    
    void ComputeVelocity();
    
    double timeC, timeT;
};


#endif /* defined(__DirectVortexSolver__VelocitySolverNSquared__) */
