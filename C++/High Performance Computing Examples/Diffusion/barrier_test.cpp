
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>

#include "timer.hpp"
#include "barrier.hpp"

typedef std::size_t size_type;

int main()
{
    const size_type max_threads = std::thread::hardware_concurrency();
    const size_type nrep = 500;
    
    timer t;
    
    for (size_type nthreads=1; nthreads<max_threads; nthreads++) {
        t.start();
        
        std::vector<std::thread> threads(nthreads);
        barrier b(nthreads);
        for (size_type n = 0; n < nthreads; ++n)
            threads[n] = std::thread([&]() {
                long ii = nrep;
                while( ii-- )
                    b.wait();
            });
        
        for (auto & t : threads)
            t.join();
        
        t.stop();
        
        std::cout << "Timing : " << nthreads << " " << t.get_timing() << std::endl;
    }
    
    return 0;
}
