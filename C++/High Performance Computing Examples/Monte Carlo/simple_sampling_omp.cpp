#include <vector>
#include <algorithm>
#include <functional>
#include <cmath>
#include <random>
#include <cassert>
#include <iostream>
#include <iterator>
#include <chrono>
#include <omp.h>

#define PI 3.1415926535

template <class BC>
class RandomWalk {

  typedef std::mt19937 generator_type;

public:
  
  RandomWalk ( const std::vector<double> ipos_, const double d_ )
    : ipos (ipos_)
    , d (d_)
  {
    BC bc ();
  }
  
  double operator () (const std::size_t seed) {
    std::vector<double> pos (ipos);
    generator_type generator (seed);
    std::function <double()> rng = std::bind ( std::uniform_real_distribution <> (0, 1), std::ref (generator) );
    double direction;
    while ( bc.inside (pos) ) {
      direction = rng ();
      pos [0] += d * std::cos (2 * PI * direction);
      pos [1] += d * std::sin (2 * PI * direction);
    }
    return bc.evaluate ( pos );
  }
  
private:
  
  BC bc;
  const std::vector<double> ipos;
  double d;
};

template <class T>
class MC {

public:
  
  MC (const T& func_, std::size_t seed_)
    : func (func_)
    , seed (seed_)
  {}
  
  void sample (const std::size_t N, const unsigned num_threads) {
    
    double sum_ = 0.0, sum2_ = 0.0;
    
    #pragma omp parallel num_threads(num_threads)
    #pragma omp for reduction(+:sum_) reduction(+:sum2_) schedule(static)
    //#pragma omp for reduction(+:sum_) reduction(+:sum2_) schedule(guided,4)
    for (std::size_t i = 0; i < N; ++i) {
      double value = func (seed * N + i);
      sum_  += value;
      sum2_ += value * value;
    }
    
    sum  = sum_;
    sum2 = sum2_;
  }
  
  double mean (const std::size_t N) const
  {
    return sum / double (N);
  }
  
  double error (const std::size_t N) const
  {
    return std::sqrt ( ( sum2 - sum * sum / double(N) ) / double ( N * (N-1) ) );
  }
  
private:
  
  const std::function <double(std::size_t)> func;
  const std::size_t seed;
  double sum, sum2;
};

struct BC {
  
  const double a = 1;
  const double b = 1;
  
  bool inside (const std::vector<double> &pos) const {
    assert (pos.size() == 2);
    return pos[0] > 0 && pos[0] < a && pos [1] > 0 && pos [1] < b;
  }
  
  virtual double evaluate (const std::vector<double>& pos) const { return 0; }
  
  double project (const std::vector<double>& pos_) {
    std::vector<double> pos (pos_);
    if ( pos [0] < 0 ) pos [0] = 0;
    if ( pos [1] < 0 ) pos [1] = 0;
    if ( pos [0] > a ) pos [0] = a;
    if ( pos [1] > b ) pos [1] = b;
    return evaluate (pos);
  }
};

struct BC_Dirichlet1 : BC {
  
  double evaluate (const std::vector<double>& pos) const {
    assert (pos.size() == 2);
    return pos[0];
  }
};

int main()
{
  const double d = 0.01;
  const std::vector<double> ipos = {0.3, 0.4};
  
  const std::size_t reps = 18;
  const std::size_t seed = 17;
  
  RandomWalk < BC_Dirichlet1 > rw (ipos, d);
  
  MC < RandomWalk <BC_Dirichlet1> > mc (rw, seed);
  
  const std::vector<unsigned> threads_vec = {1,2,4,8,16,24};
  
  std::chrono::time_point<std::chrono::high_resolution_clock> start,end;
  
  std::cout << "# runtimes" << std::endl;
  
  for (std::size_t i = 0; i < reps; i++) {
    
    const std::size_t N = std::size_t(1) << i;
    
    std::cout << "********************" << std::endl;
    std::cout << "N: " << N << std::endl;
    std::cout << "------------" << std::endl;
    
    for (const auto num_threads : threads_vec) {
      
      std::vector<double> runtimes;
      
      for (unsigned j = 0; j < 20; ++j) {
        
        start = std::chrono::high_resolution_clock::now();
        mc.sample (N, num_threads);
        end = std::chrono::high_resolution_clock::now();
        
        const double elapsed = std::chrono::duration<double>(end-start).count();
        
        runtimes.push_back(elapsed);
      }
      
      //std::sort (runtimes.begin(), runtimes.end());
      
      //const double time_min = *std::min_element (runtimes.begin(), runtimes.end());
      //const double time_median = runtimes[5];

      //mean and error
      const double time_mean = std::accumulate(runtimes.begin(),runtimes.end(),(double) 0.0)/runtimes.size();
      double time_err = 0.0;
      std::for_each(runtimes.begin(),runtimes.end(), [&time_err,&time_mean](const double d){
	      time_err += (d-time_mean)*(d-time_mean);});
      time_err = std::sqrt(time_err/(runtimes.size()*(runtimes.size()-1)));

      
      std::cout << num_threads << ' ' << time_mean << ' ' << time_err << std::endl;
    }
  }
}
