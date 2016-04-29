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
                const value_type dt,
                const int rank)
    : D_(D), L_(L), N_(N), Ntot((N_+2)*(N_+2)), dt_(dt), rank_(rank)
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
    /// Dirichlet boundaries; central differences in space, forward Euler
    /// in time
#pragma omp parallel for collapse(2)
    for(size_type i = 1; i <= N_; ++i) {
        for(size_type j = 1; j <= N_; ++j) {
            rho_tmp[i*N_ + j] = rho_[i*N_ + j] +
            fac_
            *
            (
             rho_[i*N_ + (j+1)]
             +
             rho_[i*N_ + (j-1)]
             +
             rho_[(i+1)*N_ + j]
             +
             rho_[(i-1)*N_ + j]
             -
             4.*rho_[i*N_ + j]
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
        
        for(size_type i = 1; i <= N_; ++i)
            for(size_type j = 1; j <= N_; ++j)
            {
                const value_type x2 = ((i-1)*dr_ - L_/2.);
                const value_type y2 = ((j-1)*dr_ - L_/2.);
                heat += rho_[i*N_ + j] * dr_ * dr_;
                mu2  += rho_[i*N_ + j] * dr_ * dr_ * (x2*x2 + y2*y2);
            }

#if DBG
	std::cout << "t = " << t_ << " heat = " << heat << " mu2 = " << mu2 << std::endl;        
#endif

        diag.push_back(Diagnostics(t_,heat,mu2));
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
        /// initialize rho(x,y,t=0)
        value_type bound = 1/2.;
        
#pragma omp parallel for collapse(2)
        for (size_type i = 1; i <= N_; ++i) {
            for (size_type j = 1; j <= N_; ++j) {
                if (std::abs((i-1)*dr_ - L_/2.) < bound && std::abs((j-1)*dr_ - L_/2.) < bound) {
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

    int rank_;
    
    std::vector<value_type> rho_, rho_tmp;
    
    std::vector<Diagnostics> diag;
};

#if !defined(_OPENMP)
int omp_get_num_threads() { return 1; }
#endif

int main(int argc, char* argv[])
{
    if (argc < 5) {
        std::cerr << "Usage: " << argv[0] << " D L N dt" << std::endl;
        return 1;
    }
    
#pragma omp parallel
    {
#pragma omp master
        std::cout << "Running with " << omp_get_num_threads() << " threads" << std::endl;
    }
    
    const value_type D  = std::stod(argv[1]);
    const value_type L  = std::stod(argv[2]);
    const size_type  N  = std::stoul(argv[3]);
    const value_type dt = std::stod(argv[4]);
    
    Diffusion2D system(D, L, N, dt, 0);
    
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
    
    std::cout << "Timing: " << N << " " << t.get_timing() << std::endl;
    
#ifndef _PERF_
    system.write_diagnostics("diagnostics_openmp.dat");
#endif
    
    return 0;
}
