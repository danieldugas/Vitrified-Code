
#include <iostream>
#include <algorithm>
#include <string>
#include <fstream>
#include <cassert>
#include <vector>
#include <cmath>

#include "timer.hpp"


typedef double value_type;
typedef std::size_t size_type;

class Diffusion2D {
public:
    Diffusion2D(const value_type D,
                const value_type L,
                const size_type N,
                const value_type dt)
    : D_(D), L_(L), N_(N), Ntot(N_*N_), dt_(dt)
    {
        /// real space grid spacing
        dr_ = L_ / (N_ - 1);
        
        /// stencil factor
        fac_ = dt_ * D_ / (dr_ * dr_);
        
        rho_.resize(Ntot, 0.);
        rho_tmp.resize(Ntot, 0.);
        
        initialize_density();
    }
    
    void advance()
    {
        /// Open boundaries; central differences in space, forward Euler
        /// in time
        
        for(size_type i = 0; i < N_; ++i)
		for(size_type j = 0; j < N_; ++j) {
                	rho_tmp[i*N_ + j] = rho_[i*N_ + j] +
                	fac_
                	*
                	(
			(j == N_-1 ? 0. : rho_[i*N_ + (j+1)]) +
                        (j == 0    ? 0. : rho_[i*N_ + (j-1)]) +
                        (i == N_-1 ? 0. : rho_[(i+1)*N_ + j]) +
                        (i == 0    ? 0. : rho_[(i-1)*N_ + j]) -
                        4.*rho_[i*N_ + j]
                        );
                }       
 
        /// use swap instead of rho_=rho_tmp. this is much more efficient, because it does not have to copy
        /// element by element.
        std::swap(rho_tmp, rho_);
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
    
    std::vector<value_type> rho_, rho_tmp;
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
    system.write_density("density_000.dat");

    const value_type tmax = 10000 * dt;
    value_type time = 0;
    
    timer t;
	
    t.start();
    while (time < tmax) {		
        system.advance();
        time += dt;
    }
    t.stop();
    
    std::cout << "Timing : " << N << " " << 1 << " " << t.get_timing() << std::endl;
    
    system.write_density("density_serial.dat");
    
    return 0;
}
