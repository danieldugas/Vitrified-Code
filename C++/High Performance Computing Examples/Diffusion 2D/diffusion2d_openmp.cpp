#include <iostream>
#include <algorithm>
#include <string>
#include <fstream>
#include <cassert>
#include <vector>
#include <cmath>
#include <omp.h>

#include "timer.hpp"


typedef double value_type;
typedef std::size_t size_type;

struct Diagnostics
{
    value_type time;
    value_type heat;
    value_type mu2;
    
    Diagnostics(value_type time, value_type heat, value_type mu2) : time(time), heat(heat), mu2(mu2) {}
};

class Diffusion2D {
public:
    Diffusion2D(const value_type D,
                const value_type L,
                const size_type N,
                const value_type dt)
    : D_(D), L_(L), N_(N), Ntot(N_*N_), dt_(dt), t_(0)
    {
        /// real space grid spacing
        dr_ = L_ / (N_ - 1);
        
        /// stencil factor
        fac_ = dt_ * D_ / (dr_ * dr_);
        
        rho_.resize(Ntot, 0.);
        rho_tmp.resize(Ntot, 0.);
        
        // check that the timestep satisfies the restriction for stability
        std::cout << "timestep from stability condition is " << dr_*dr_/(4.*D_) << std::endl;
        
        initialize_density();
    }
    
	value_type advance()
	{
    	/// Open boundaries; central differences in space, forward Euler
    	/// in time
		#pragma omp parallel for
    	for(size_type i = 0; i < N_; ++i) {
        	for(size_type j = 0; j < N_; ++j) {
            	rho_tmp[i*N_ + j] = rho_[i*N_ + j] +
            	fac_
            	*
            	(
            	(j == N_-1 ? 0. : rho_[i*N_ + (j+1)])
            	+
            	(j == 0    ? 0. : rho_[i*N_ + (j-1)])
            	+
            	(i == N_-1 ? 0. : rho_[(i+1)*N_ + j])
            	+
            	(i == 0    ? 0. : rho_[(i-1)*N_ + j])
            	-
            	4.*rho_[i*N_ + j]
             );
        	}
    	}
        
        /// use swap instead of rho_=rho_tmp. this is much more efficient, because it does not have to copy
        /// element by element.
        std::swap(rho_tmp, rho_);
        
        t_ += dt_;
        
        return t_;
    }
    
    void compute_diagnostics()
    {
        value_type heat=0, mu2=0;
        
        for(size_type i = 0; i < N_; ++i)
            for(size_type j = 0; j < N_; ++j)
            {
                const value_type x2 = (i*dr_ - L_/2.);
                const value_type y2 = (j*dr_ - L_/2.);
                heat += rho_[i*N_ + j] * dr_ * dr_;
                mu2  += rho_[i*N_ + j] * dr_ * dr_ * (x2*x2 + y2*y2);
            }
        
        diag.push_back(Diagnostics(t_,heat,mu2));
    }
    
    void write_diagnostics(std::string const& filename) const
    {
        std::ofstream out_file(filename, std::ios::out);
        
        for (Diagnostics d : diag)
            out_file << d.time << '\t' << d.heat << '\t' << d.mu2 << "\n";
        out_file.close();
    }
    
    void write_density(std::string const& filename) const
    {
        std::ofstream out_file(filename, std::ios::out);
        
        for(size_type i = 0; i < N_; ++i) {
            for(size_type j = 0; j < N_; ++j)
                out_file << (i*dr_ - L_/2.) << '\t' << (j*dr_ - L_/2.) << '\t' << rho_[i*N_ + j] << "\n";
            out_file << "\n";
        }
        out_file.close();
    }
    
private:
    
    void initialize_density()
    {
        /// initialize rho(x,y,t=0)
        value_type bound = 1/2.;
        
		#pragma omp parallel for
        for (size_type i = 0; i < N_; ++i) {
            for (size_type j = 0; j < N_; ++j) {
                if (std::abs(i*dr_ - L_/2.) < bound && std::abs(j*dr_ - L_/2.) < bound) {
                    rho_[i*N_ + j] = 1;
                } else {
                    rho_[i*N_ + j] = 0;
                }
            }
        }
    }
    
    value_type D_, L_;
    size_type N_, Ntot;
    
    value_type dr_, dt_, t_, fac_;
    
    std::vector<value_type> rho_, rho_tmp;
    
    std::vector<Diagnostics> diag;
};


int main(int argc, char* argv[])
{
    if (argc < 5) {
        std::cerr << "Usage: " << argv[0] << " D L N dt" << std::endl;
        return 1;
    }

    const value_type D  = std::stod(argv[1]);
    const value_type L  = std::stod(argv[2]);
    const size_type  N  = std::stoul(argv[3]);
    const value_type dt = std::stod(argv[4]);
    
    
    Diffusion2D system(D, L, N, dt);
    system.write_density("density_openmp_000.dat");
    
    const value_type tmax = 10000 * dt;
    value_type time = 0;
    
    timer t;
    
    t.start();
    while (time < tmax) {
        time = system.advance();
#ifndef _PERF_
        system.compute_diagnostics();
#endif
    }
    t.stop();

	#pragma omp parallel
    {    
		#pragma omp master
		std::cout << "Timing: " << N << "\t" << omp_get_num_threads() << "\t" << t.get_timing() << std::endl;
    }

    system.write_density("density_openmp.dat");
#ifndef _PERF_
    system.write_diagnostics("diagnostics_openmp.dat");
#endif
    
    return 0;
}
