#include <fstream>
#include "common.h"
#include "ArrayOfParticles.h"
#include "VelocitySolverNSquared.h"

//#define WEAK_SCALING

void WriteToFile(ArrayOfParticles & allParticles, const int Np, const char * sFileName)
{
    FILE * f = fopen(sFileName, "w");
    
    fprintf(f,"%s,%s,%s,%s,%s,%s,%s\n","x","y","z","vx","vy","vz","w");
    for(int i=0; i<Np;i++)
        fprintf(f,"%f,%f,%f,%f,%f,%f,%f\n", allParticles.x[i],allParticles.y[i],0.0,
                allParticles.u[i],allParticles.v[i],0.0,
                allParticles.gamma[i]);
    fclose(f);
}

void WriteTimeToFile(const int Nranks, const double time, const char * sFileName)
{
    FILE * f = fopen(sFileName, "a");
    
    fprintf(f,"%d\t%f\n", Nranks,time);
    
    fclose(f);
}

void Dump(ArrayOfParticles& dstParticles, ArrayOfParticles& allParticles, const int Np, const int NpProcess, const int step, const int rank)
{
    // need to gather all particles here
    MPI_Gather(dstParticles.x    , NpProcess, MPI_DOUBLE, &allParticles.x[rank*NpProcess]    , NpProcess, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Gather(dstParticles.y    , NpProcess, MPI_DOUBLE, &allParticles.y[rank*NpProcess]    , NpProcess, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Gather(dstParticles.u    , NpProcess, MPI_DOUBLE, &allParticles.u[rank*NpProcess]    , NpProcess, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Gather(dstParticles.v    , NpProcess, MPI_DOUBLE, &allParticles.v[rank*NpProcess]    , NpProcess, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Gather(dstParticles.gamma, NpProcess, MPI_DOUBLE, &allParticles.gamma[rank*NpProcess], NpProcess, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    
    if (rank==0)
    {
        char buf[500];
        sprintf(buf, "vortexline_%5.5d.csv", step);
        WriteToFile(allParticles,Np,buf);
    }
}

int main (int argc, char ** argv)
{
    MPI_Init(&argc,&argv);
    
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    // number of processes
    if (rank==0)
        std::cout << "Running with " << size << " MPI processes\n";
    
    // timer setup
    Timer timerIC, timerSim;
	
	// time integration setup
	const double dt = 0.001;
    const double tfinal = 2.5;
    const int ndump = 10;
    // output
    const bool bAnimation = true;
    const bool bVerbose = true;
    // number of particles
    const int N = 10000;
	
    timerIC.start();
	// initialization
#ifndef WEAK_SCALING
    size_t Np = (N/size)*size; // ensures that the number of particles is divisible by the number of workers
#else
    size_t Np = N*sqrt(size);  // keep number of work per rank fixed
#endif
    
    // each worker gets a part of the whole array
    size_t NpProcess = Np/size;
    ArrayOfParticles dstParticles(NpProcess), srcParticles(NpProcess);
    ArrayOfParticles allParticles(rank==0 ? Np : 0); // list of all particles for output
    
	const double dx = 1./Np;
    double totGamma=0.;
    
    // initialize particles: position and circulation
	for(size_t i=0; i<NpProcess; i++)
	{
        const double Gamma_s = 1.;
		const double x = -0.5 + ((double)i+(double)rank*(double)NpProcess+.5)*dx;

		dstParticles.x[i] = x;
		dstParticles.y[i] = 0.0;
        const double g = 4.*Gamma_s*x/sqrt(1.-4.*x*x);
		dstParticles.gamma[i] = g*dx;
        
		totGamma += dstParticles.gamma[i];
        
        // each rank has also to compute the interactions with its own particles
		srcParticles.x[i] = dstParticles.x[i];
		srcParticles.y[i] = dstParticles.y[i];
		srcParticles.gamma[i] = dstParticles.gamma[i];
	}
    MPI_Reduce(rank==0 ? MPI_IN_PLACE : &totGamma, &totGamma, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    timerIC.stop();

    if (rank==0)
    {
        std::cout << "Number of particles: " << Np << std::endl;
        std::cout << "Number of particles per process: " << NpProcess << std::endl;
        std::cout << "Initial circulation: " << totGamma << std::endl;
        std::cout << "IC time: " << timerIC.get_timing() << std::endl;
    }
    
    // initialize velocity solver
    VelocitySolverNSquared VelocitySolver(dstParticles, srcParticles, rank, size);
	
    timerSim.start();
    double t=0;
    int it=0;
    for (it=1; it<=std::ceil(tfinal/dt); it++)
	{
        // reset particle velocities
		dstParticles.ClearVelocities();
		// compute velocities corresponding to time n
		VelocitySolver.ComputeVelocity();
        
        // dump the particles
        if((it-1)%ndump==0 && bAnimation)
            Dump(dstParticles,allParticles,Np,NpProcess,it-1,rank);
        
        // update time
        if (rank==0)
            t += dt;
        
        MPI_Bcast(&t, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        
        if(it%ndump==0 && rank==0 && bVerbose)
            std::cout << "Iteration " << it << " time " << t << std::endl;
		
		// advance particles in time to n+1 using forward Euler
		dstParticles.AdvectEuler(dt);
        // update "source" particles
        srcParticles = dstParticles;
    }
    // dump final state
    if((it-1)%ndump==0 && bAnimation)
        Dump(dstParticles,allParticles,Np,NpProcess,it-1,rank);
    
    if(bVerbose)
        std::cout << "Bye from rank " << rank << std::endl;
    timerSim.stop();
    
    if (rank==0)
    {
        char buf[500];
        sprintf(buf, "timing.dat");
        WriteTimeToFile(size,timerSim.get_timing(),buf);
        std::cout << "#Ranks, Time - " << size << "\t" << timerSim.get_timing() << "\t( " << VelocitySolver.timeT << "\t" << VelocitySolver.timeC << " )\n";
    }

    MPI_Finalize();

	return 0;
}


