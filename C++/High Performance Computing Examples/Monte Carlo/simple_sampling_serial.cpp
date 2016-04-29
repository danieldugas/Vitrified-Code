#include <vector>
#include <algorithm>
#include <functional>
#include <cmath>
#include <random>
#include <cassert>
#include <iostream>
#include <iterator>

#define PI 3.1415926535

template <class BC>
class RandomWalk {

  typedef std::mt19937 generator_type;

public:
  
  RandomWalk ( const std::vector<double> ipos_, const double d_ )
    : ipos (ipos_)
    , d (d_)
  {
    //BC bc ();
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
  
  void sample (const std::size_t N) {
    
    sum = sum2 = 0.0;
    
    for (std::size_t i = 0; i < N; ++i) {
      double value = func (seed * N + i);
      sum  += value;
      sum2 += value * value;
    }
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
  const std::vector<double> ipos = {0.3, 0.3};
  
  const std::size_t reps = 16;
  const std::size_t seed = 1;
  
  RandomWalk < BC_Dirichlet1 > rw (ipos, d);
  
  MC < RandomWalk <BC_Dirichlet1> > mc (rw, seed);
  
  std::cout << "# samples mean error" << std::endl;
  
  for (std::size_t i = 1; i < reps; i++) {
    const std::size_t N = std::size_t(1) << i;
    mc.sample(N);
    std::cout << N << ' ' << mc.mean(N) << ' ' << mc.error(N) << std::endl;
  }
}
