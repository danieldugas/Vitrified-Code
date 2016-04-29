
#include <iostream>
#include <vector>
#include <string>

#include "disks_mc.hpp"
#include "vector_stats.hpp"
#include "timer.hpp"

#include <omp.h>


int main () {
    
    std::size_t Nx = 100;
    std::size_t Ny = 115;
    double L = 1.;
    std::size_t nu = 7;
    double d0 = (1. - (std::pow(2., nu-8.))) * L / Nx;
    
    std::size_t M = 512;
    double dA2 = 2.*(L/2.)*(L/2.) / M;
    
    std::size_t nequi = 100;
    std::size_t nmeas = 500;
    
    
    timer t;
    t.start();
    
    int nthreads;
#pragma omp parallel
#pragma omp master
    nthreads = omp_get_num_threads();
    
    std::vector<VectorStats> thread_accumulators(nthreads, VectorStats(M));
#pragma omp parallel
    {
        int thread_num = omp_get_thread_num();
        
        DisksMC system(Nx, Ny, L, d0, thread_num);
        
        /// Equilibration
        for (std::size_t n=0; n<nequi; ++n)
            system.sweep();
        
        /// Measurements
#pragma omp for
        for (std::size_t n=0; n<nmeas; ++n) {
            system.sweep();
            thread_accumulators[thread_num] << system.distance_histogram(M, dA2);
        }
    }
    
    /// Merge results between threads
    for (int n=1; n<nthreads; ++n)
        thread_accumulators[0] << thread_accumulators[n];
    
    t.stop();
    std::cout << "# Time elapsed : " << t.get_timing() << ", nthreads : " << nthreads << std::endl;
    
    // std::vector<double> Nm_mean  = thread_accumulators[0].mean();
    // std::vector<double> Nm_error = thread_accumulators[0].error();
    // std::cout << "# Distance histogram:\n";
    // for (std::size_t i=0; i<M; ++i)
    //     std::cout << i << "  " << Nm_mean[i] << "  " << Nm_error[i] << std::endl;
}
