#include <iostream>
#include <algorithm>
#include <string>
#include <fstream>
#include <cassert>
#include <vector>
#include <cmath>
#include <omp.h>
#include <mpi.h>
#include <stdio.h>
#include "timer.hpp"


typedef double value_type;
#define MPI_VALUE_TYPE MPI_DOUBLE
typedef std::size_t size_type;


struct Diagnostics
{
    value_type time;
    value_type heat;
    value_type mu2;
    
    Diagnostics(value_type time, value_type heat, value_type mu2) : time(time), heat(heat), mu2(mu2) {}
};

class Diffusion2D_MPI {
public:
    Diffusion2D_MPI(const value_type D,
                const value_type L,
                const size_type N,
                const value_type dt,
                const int rank,
                const int procs)
    : D_(D), L_(L), N_(N), dt_(dt), rank_(rank), procs_(procs)
    {
	// initialize to zero
	t_  = 0.0;

        /// real space grid spacing
        dr_ = L_ / (N_ - 1);
        
        /// stencil factor
        fac_ = dt_ * D_ / (dr_ * dr_);

	// number of rows per process
	local_N_ = N_ / procs_;	

	// small correction for the last process
	if (rank_ == procs - 1) local_N_ += (N_ % procs_);

	// actual dimension of a row (+2 for the ghosts)
	real_N_ = N_ + 2;
	Ntot = (local_N_ + 2) * (N_+2);

	rho_.resize(Ntot, 0.);
	rho_tmp.resize(Ntot, 0.);

        // check that the timestep satisfies the restriction for stability
	if (rank_ == 0)
	        std::cout << "timestep from stability condition is " << dr_*dr_/(4.*D_) << std::endl;
        
        initialize_density();
    }
    
    value_type advance()
    {
	MPI_Request req[4];
	MPI_Status status[4];

	int prev_rank = rank_ - 1;
	int next_rank = rank_ + 1;

	if (prev_rank >= 0) {
		MPI_Irecv(&rho_[           0*real_N_+1], N_, MPI_VALUE_TYPE, prev_rank, 100, MPI_COMM_WORLD, &req[0]);
		MPI_Isend(&rho_[           1*real_N_+1], N_, MPI_VALUE_TYPE, prev_rank, 100, MPI_COMM_WORLD, &req[1]);
	} else {
		req[0] = MPI_REQUEST_NULL;
		req[1] = MPI_REQUEST_NULL;
	}
	
	if (next_rank < procs_) {
		MPI_Irecv(&rho_[(local_N_+1)*real_N_+1], N_, MPI_VALUE_TYPE, next_rank, 100, MPI_COMM_WORLD, &req[2]);
		MPI_Isend(&rho_[    local_N_*real_N_+1], N_, MPI_VALUE_TYPE, next_rank, 100, MPI_COMM_WORLD, &req[3]);
	} else {
		req[2] = MPI_REQUEST_NULL;
		req[3] = MPI_REQUEST_NULL;
	}

        /// Dirichlet boundaries; central differences in space, forward Euler
        /// in time
	// update the interior rows
        for(size_type i = 2; i < local_N_; ++i) {
          for(size_type j = 1; j <= N_; ++j) {
            rho_tmp[i*real_N_ + j] = rho_[i*real_N_ + j] +
            fac_
            *
            (
             rho_[i*real_N_ + (j+1)]
             +
             rho_[i*real_N_ + (j-1)]
             +
             rho_[(i+1)*real_N_ + j]
             +
             rho_[(i-1)*real_N_ + j]
             -
             4.*rho_[i*real_N_ + j]
             );
          }
        }

	// ensure boundaries have arrived
	MPI_Waitall(4, req, status);

	// update the first and the last rows
        for(size_type i = 1; i <= local_N_; i += (local_N_-1)) {
          for(size_type j = 1; j <= N_; ++j) {
            rho_tmp[i*real_N_ + j] = rho_[i*real_N_ + j] +
            fac_
            *
            (
             rho_[i*real_N_ + (j+1)]
             +
             rho_[i*real_N_ + (j-1)]
             +
             rho_[(i+1)*real_N_ + j]
             +
             rho_[(i-1)*real_N_ + j]
             -
             4.*rho_[i*real_N_ + j]
             );
          }
        }

        
        /// use swap instead of rho_=rho_tmp. this is much more efficient, because it does not have to copy
        /// element by element.
        using std::swap;
        swap(rho_tmp, rho_);
        
        t_ += dt_;
        
        return t_;
    }
    
    void compute_diagnostics()
    {
        value_type heat=0, mu2=0;

        for(size_type i = 1; i <= local_N_; ++i) {
	    size_type gi = rank_ * (N_ / procs_) + i;	// convert local index to global index
            for(size_type j = 1; j <= N_; ++j)
            {
                const value_type x2 = ((gi-1)*dr_ - L_/2.);
                const value_type y2 = ((j-1)*dr_ - L_/2.);
                heat += rho_[i*real_N_ + j] * dr_ * dr_;
                mu2  += rho_[i*real_N_ + j] * dr_ * dr_ * (x2*x2 + y2*y2);

            }
        }

	MPI_Reduce(rank_ == 0? MPI_IN_PLACE: &heat, &heat, 1, MPI_VALUE_TYPE, MPI_SUM, 0, MPI_COMM_WORLD); 
	MPI_Reduce(rank_ == 0? MPI_IN_PLACE: &mu2,  &mu2,  1, MPI_VALUE_TYPE, MPI_SUM, 0, MPI_COMM_WORLD); 

        if (rank_ == 0) {
#if DBG
		std::cout << "t = " << t_ << " heat = " << heat << " mu2 = " << mu2 << std::endl;
#endif
		diag.push_back(Diagnostics(t_,heat,mu2));
	}
    }
    
    void write_diagnostics(std::string const& filename) const
    {
        std::ofstream out_file(filename, std::ios::out);
        
        for (Diagnostics d : diag)
            out_file << d.time << '\t' << d.heat << '\t' << d.mu2 << "\n";
        out_file.close();
    }
    
private:
    
    void initialize_density()
    {
	size_type gi;
        /// initialize rho(x,y,t=0)
        value_type bound = 1/2.;

        for (size_type i = 1; i <= local_N_; ++i) {
            gi = rank_ * (N_ / procs_) + i;	// convert local index to global index
            for (size_type j = 1; j <= N_; ++j) {
                if (std::abs((gi-1)*dr_ - L_/2.) < bound && std::abs((j-1)*dr_ - L_/2.) < bound) {
                    rho_[i*real_N_ + j] = 1;
                } else {
                    rho_[i*real_N_ + j] = 0;
                }
            }
        }
    }
    
    value_type D_, L_;
    size_type N_, Ntot, local_N_, real_N_;
    
    value_type dr_, dt_, t_, fac_;

    int rank_, procs_;
    
    std::vector<value_type> rho_, rho_tmp;
    std::vector<Diagnostics> diag;
};


int main(int argc, char* argv[])
{
    if (argc < 5) {
        std::cerr << "Usage: " << argv[0] << " D L N dt" << std::endl;
        return 1;
    }

    MPI_Init(&argc, &argv);

    int rank, procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &procs);

    if (rank == 0)
        std::cout << "Running with " << procs  << " MPI processes" << std::endl;
    
    const value_type D  = std::stod(argv[1]);
    const value_type L  = std::stod(argv[2]);
    const size_type  N  = std::stoul(argv[3]);
    const value_type dt = std::stod(argv[4]);
    
    Diffusion2D_MPI system(D, L, N, dt, rank, procs);

#if DBG
    system.compute_diagnostics();
#endif
    
    const value_type tmax = 10000 * dt;
    value_type time = 0;
    
    timer t;
    
    int i = 0;
    t.start();
    while (time < tmax) {
        time = system.advance();
#ifndef _PERF_
        system.compute_diagnostics();
#endif
        i++;
        if (i == 100) break;
      
    }
    t.stop();
    

    if (rank == 0)
      std::cout << "Timing: " << N << " " << t.get_timing() << std::endl;
    
#ifndef _PERF_
    if (rank == 0)
       system.write_diagnostics("diagnostics_mpi.dat");
#endif
    
    MPI_Finalize();
    return 0;
}
