
#include "VelocitySolverNSquared.h"

VelocitySolverNSquared::VelocitySolverNSquared(ArrayOfParticles & dstParticles, ArrayOfParticles & srcParticles, const int rank, const int size)
: dstParticles(dstParticles), srcParticles(srcParticles), rank(rank), size(size), timeC(0), timeT(0)
{
}

void VelocitySolverNSquared::ComputeVelocity()
{
	const double i2pi = 0.5/M_PI;
    const int N = dstParticles.Np;
    
    for (int pass=0; pass<size; pass++)
    {
        // 1. exchange
        Timer timerT;
        timerT.start();
        if (pass!=0)
        {
            // no need to send velocities
            MPI_Request reqs[6];
            MPI_Isend(dstParticles.x    , N, MPI_DOUBLE, (rank+pass     )%size, 0, MPI_COMM_WORLD, &reqs[0]);
            MPI_Irecv(srcParticles.x    , N, MPI_DOUBLE, (rank-pass+size)%size, 0, MPI_COMM_WORLD, &reqs[1]);
            MPI_Isend(dstParticles.y    , N, MPI_DOUBLE, (rank+pass     )%size, 1, MPI_COMM_WORLD, &reqs[2]);
            MPI_Irecv(srcParticles.y    , N, MPI_DOUBLE, (rank-pass+size)%size, 1, MPI_COMM_WORLD, &reqs[3]);
            MPI_Isend(dstParticles.gamma, N, MPI_DOUBLE, (rank+pass     )%size, 2, MPI_COMM_WORLD, &reqs[4]);
            MPI_Irecv(srcParticles.gamma, N, MPI_DOUBLE, (rank-pass+size)%size, 2, MPI_COMM_WORLD, &reqs[5]);
            
            MPI_Status status[6];
            MPI_Waitall(6, reqs, status);
        }
        timerT.stop();
        timeT += timerT.get_timing();
        
        // 2. compute local
        Timer timerC;
        timerC.start();
        for (int i=0; i<N; i++) // loop over dst (particles of this rank)
        {
            double u=0;
            double v=0;
            
            for (int j=0; j<N; j++) // loop over src (received particles)
            {
                if(i+rank*N==j+((rank+pass)%size)*N) continue;
                
                const double dx = dstParticles.x[i] - srcParticles.x[j];
                const double dy = dstParticles.y[i] - srcParticles.y[j];
                const double rsq = dx*dx + dy*dy;
                const double irsq = 1./rsq;
                
                u += -srcParticles.gamma[j] * dy * irsq;
                v +=  srcParticles.gamma[j] * dx * irsq;
            }
            
            dstParticles.u[i] += i2pi * u;
            dstParticles.v[i] += i2pi * v;
        }
        timerC.stop();
        timeC += timerC.get_timing();
    }
}