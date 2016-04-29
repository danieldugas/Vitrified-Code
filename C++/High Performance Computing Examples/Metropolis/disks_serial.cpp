
#include <iostream>
#include <vector>
#include <string>

#include "disks_mc.hpp"
#include "vector_stats.hpp"
#include "timer.hpp"

int main () {
    
    std::size_t Nx = 14;
    std::size_t Ny = 16;
    double L = 1.;
    std::size_t nu = 5;
    double d0 = (1. - (std::pow(2., nu-8.))) * L / Nx;
    
    std::size_t M = 512;
    double dA2 = 2.*(L/2.)*(L/2.) / M;
    
    std::size_t nequi = 64;
    std::size_t nmeas = 64;
    
    timer t;
    t.start();
    
    DisksMC system(Nx, Ny, L, d0, 42);
    system.write_configuration("config.initial.txt");
    
    
    /// Equilibration
    for (std::size_t n=0; n<nequi; ++n)
        system.sweep();
    
    /// Measurements
    int accepted = 0;
    VectorStats Nm_accumulator(M);
    for (std::size_t n=0; n<nmeas; ++n) {
        accepted += system.sweep();
        Nm_accumulator << system.distance_histogram(M, dA2);
    }
    
    t.stop();
    std::cout << "# Time elapsed : " << t.get_timing() << std::endl;
    
    std::cout << "# Acceptance rate : " << accepted / double(nmeas*Nx*Ny) << std::endl;
    
    std::vector<double> Nm_mean  = Nm_accumulator.mean();
    std::vector<double> Nm_error = Nm_accumulator.error();
    std::cout << "# Distance histogram:\n";
    for (std::size_t i=0; i<M; ++i)
        std::cout << i << "  " << Nm_mean[i] << "  " << Nm_error[i] << std::endl;
    
    system.write_configuration("config.final.txt");
}