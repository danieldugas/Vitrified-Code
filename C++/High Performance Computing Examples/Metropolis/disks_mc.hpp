#ifndef HPCSE_DISKSMC_HPP
#define HPCSE_DISKSMC_HPP

#include <fstream>
#include <array>
#include <vector>
#include <string>
#include <cmath>
#include <random>

class DisksMC {
public:
    typedef std::size_t size_type;
    typedef std::array<double,2> coords_type;
    typedef std::vector<coords_type> config_type;
    
    DisksMC(size_type Nx, size_type Ny, double L, double d0, size_type seed)
    : Nx_(Nx), Ny_(Ny), N_(Nx_*Ny_), L_(L)
    , d0_(d0), alpha_(L_/double(Nx_) - d0_)
    , data_(N_)
    , eng_(seed), int_distr_(0, N_-1), displ_distr_(-alpha_, alpha_)
    {
        /// init default lattice structure
        double dx = L_ / Nx_;
        double dy = std::sqrt(3.)/2. * L_ / Nx_;
        for (size_type i=0; i<Nx_; ++i)
            for (size_type j=0; j<Ny_; ++j) {
                data_[i*Ny_ + j][0] = i*dx + dx/4.;
                data_[i*Ny_ + j][1] = j*dy + dy/2.;
                if (j % 2 == 0)
                    data_[i*Ny_ + j][0] += dx/2.;
            }
    }
    
    
    size_type sweep()
    {
        size_type accepted = 0;
        for (size_type i=0; i<N_; ++i) {
            /// Pick a random particle
            size_type n = int_distr_(eng_);
            
            /// Propose new position
            coords_type new_pos = random_displacement(data_[n]);
            
            /// Check if new position is overlapping with other particles
            double d02 = d0_*d0_;
            bool accept = true;
            for (size_type m=0; m<N_ && accept; ++m) {
                accept = (m == n) || (minimal_dist2(new_pos, data_[m]) > d02);
            }
            
            /// In case, update the configuration
            if (accept)
                data_[n] = new_pos;
            
            accepted += accept;
        }
        
        return accepted;
    }
    
    std::vector<double> distance_histogram(size_type M, double dA2) const
    {
        std::vector<double> Nm(M, 0.);
        
        double d02 = d0_*d0_;
        for (size_type n=0; n<N_; ++n)
            for (size_type m=0; m<n; ++m) {
                double dist2 = minimal_dist2(data_[n], data_[m]);
                size_type ii = (dist2 - d02) / dA2;
                Nm[ii] += 1;
            }
        return Nm;
    }
    
    void write_configuration(std::string const& fname) const
    {
        std::ofstream ofs(fname);
        for (size_type i=0; i<N_; ++i)
            ofs << data_[i][0] << "  " << data_[i][1] << std::endl;
    }
    
    
private:
    
    coords_type random_displacement(coords_type pos)
    {
        /// compute new random number
        pos[0] += displ_distr_(eng_);
        pos[1] += displ_distr_(eng_);
        
        /// impose periodic boundary conditions
        if      (pos[0] > L_) pos[0] -= L_;
        else if (pos[0] < 0 ) pos[0] += L_;
        if      (pos[1] > L_) pos[1] -= L_;
        else if (pos[1] < 0 ) pos[1] += L_;
        
        return pos;
    }
    
    double minimal_dist2(coords_type const& x1, coords_type const& x2) const
    {
        coords_type s = {{ x1[0]-x2[0], x1[1]-x2[1] }};
        
        if      (s[0] >  L_/2.) s[0] -= L_;
        else if (s[0] < -L_/2.) s[0] += L_;
        if      (s[1] >  L_/2.) s[1] -= L_;
        else if (s[1] < -L_/2.) s[1] += L_;
        
        return s[0]*s[0] + s[1]*s[1];
    }
    
    
    size_type Nx_, Ny_, N_;
    double L_, d0_, alpha_;
    config_type data_;
    
    std::mt19937 eng_;
    std::uniform_int_distribution<int> int_distr_;
    std::uniform_real_distribution<double> displ_distr_;
};

#endif
