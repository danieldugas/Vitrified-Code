
#include <iostream>
#include <algorithm>
#include <string>
#include <fstream>
#include <cassert>
#include <vector>
#include <cmath>
#include <thread>
#include <mutex>

#include "timer.hpp"
#include "barrier.hpp"

typedef double value_type;
typedef std::size_t size_type;

class Diffusion2D {
public:
    Diffusion2D(const value_type D,
                const value_type L,
                const size_type N,
                const value_type dt,
                const size_type nthreads)
    : D_(D), L_(L), N_(N), Ntot(N_*N_), dt_(dt), nthreads_(nthreads), b_(nthreads_)
    {
        /// real space grid spacing
        dr_ = L_ / (N_ - 1);
        
        /// stencil factor
        fac_ = dt_ * D_ / (dr_ * dr_);
        
        rho_.resize(Ntot, 0.);
        rho_tmp.resize(Ntot, 0.);
        
        initialize_density();
    }
    
    void advance(const size_type start, const size_type end)
    {
        /// Open boundaries; central differences in space, forward Euler
        /// in time
        
        for(size_type i = start; i < end; ++i) {
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
        
        /// wait for all threads before swapping the data
        b_.wait();
        
        /// use swap instead of rho_=rho_tmp. this is much more efficient, because it does not have to copy
        /// element by element.        
        if (start == 0) std::swap(rho_tmp, rho_); /// only one thread is doing the swap!!
        
        /// make sure that no thread is leaving the iteration before the data has been swapped,
        /// otherwise some threads could start the next iteration using the old data
        b_.wait();
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
    
    value_type dr_, dt_, fac_;
    
    size_type nthreads_;
    barrier b_;
    
    std::vector<value_type> rho_, rho_tmp;
};


int main(int argc, char* argv[])
{
    if (argc < 6) {
        std::cerr << "Usage: " << argv[0] << " D L N dt nthreads" << std::endl;
        return 1;
    }
    
    const value_type D  = std::stod(argv[1]);
    const value_type L  = std::stod(argv[2]);
    const size_type  N  = std::stoul(argv[3]);
    const value_type dt = std::stod(argv[4]);
    const size_type  nthreads = std::stoul(argv[5]);

    
    Diffusion2D system(D, L, N, dt, nthreads);

    const value_type tmax = 10000 * dt;
    
    timer t;
    
    t.start();
    
    std::vector<std::thread> threads(nthreads);
    for (size_type n = 0; n < nthreads; ++n)
        threads[n] = std::thread([&, n] () {
            const size_type begin = n * N / nthreads;
            const size_type end   = (n+1) * N / nthreads;
            value_type time = 0;
            while (time < tmax) {
                system.advance(begin, end);
                time += dt;
            }
        });
    
    for (auto & t : threads)
        t.join();
    
    t.stop();
    
    std::cout << "Timing : " << N << " " << nthreads << " " << t.get_timing() << std::endl;
    
    system.write_density("density_barrier.dat");
    
    return 0;
}
