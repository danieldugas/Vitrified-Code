// Example codes for HPC course
// (c) 2015 ETH Zurich

#include <array>
#include <vector>
#include <algorithm>
#include <functional>
#include <random>
#include <iostream>
#include <iterator>
#include <cassert>
#include <tuple>
#include <fstream>
#include <sstream>
#include <timer.hpp>

const unsigned DIMENSIONS = 2;

typedef std::size_t size_type;
typedef double scalar_type;
typedef std::array<scalar_type,DIMENSIONS> position;

struct potential
{
    potential(scalar_type rm, scalar_type epsilon):
    rm2_(rm*rm),
    eps_(epsilon),
    rc2_(0.0),
    shift_(0)
    {
        rc2_ = 2.5*rm/std::pow(2,1/6.);
        
        position x = {{}};
        position y = {{rc2_}};
        assert( x[0] == 0  && x[1] == 0 );
        assert( y[1] == 0 );
        shift_ = -(*this)(x,y,position());
        rc2_ = rc2_*rc2_;
        std::cout << "# Potential shift -V(rc=" << rc2_ << ")=" << shift_ << std::endl;
    }
    

    /// potential V(r^2)
    scalar_type operator()(scalar_type r2) const
    {
        if( r2 >= rc2_ )    return 0;

        scalar_type s2 = rm2_ / r2;
        scalar_type s6 = s2*s2*s2;
        return eps_*(s6*s6 - 2*s6) + shift_;
    }

    /// potential V(x,y) considering periodic boundaries
    scalar_type operator()(const position& x, const position& y, const position& extent) const
    {
        scalar_type r2 = 0.;
        for( size_type d = 0; d < DIMENSIONS; ++d )
        {
            scalar_type r = dist(x[d],y[d],extent[d]);
            r2 += r*r;
        }

        return (*this)(r2);
    }

    /// compute the Lennard-Jones force F(x,y) which particle y exerts on x and add it to f,
    /// considering periodic boundaries at extent
    void add_force(position& f, const position& x, const position& y, const position& extent) const
    {
        position r;
        scalar_type r2 = 0.;
        for( size_type d = 0; d < DIMENSIONS; ++d )
        {
            r[d] = dist(x[d],y[d],extent[d]);
            r2 += r[d]*r[d];
        }

        if( r2 >= rc2_ )    return;

        // s = r_m/r
        // V(s) = eps * (s^12 - s^6)
        r2 = 1/r2;
        scalar_type s2 = rm2_ * r2; // (rm/r)^2
        scalar_type s6 = s2*s2*s2;  // (rm/r)^6
        scalar_type fr = 12*eps_ * (s6*s6 - s6) * r2; // global factor
        for( size_type d = 0; d < DIMENSIONS; ++d )
            f[d] += fr * r[d];
    }
    
    scalar_type cutoff_radius() const { return std::sqrt(rc2_); }

private:
    scalar_type dist(scalar_type x, scalar_type y, scalar_type extent) const
    {
        scalar_type r = x-y;
        if     ( r < -extent/2 ) r += extent;
        else if( r >  extent/2 ) r -= extent;
        return r;
    }

    scalar_type rm2_;   // r_m^2
    scalar_type eps_;   // \epsilon
    scalar_type rc2_;   // cut-off radius r_c^2
    scalar_type shift_; // potential shift -V(r_c)
};


class simulation
{
public:
    /// Initialize simulation in rectangular box with corners (0,0) and extent.
    /// Initial positions and velocities are given as x, v.
    simulation(const position& extent, const potential& pot,
               const std::vector<position>& x, const std::vector<position>& v ):
    extent_(extent),
    potential_(pot),
    x_(x),
    v_(v),
    a_(x.size())
    {
        assert( x.size() == v.size() );
        calculate_forces(a_,x_);
    }

    /// evolve the system for [steps] time steps of size [dt]
    void evolve(scalar_type dt, size_type steps)
    {
        using std::swap;
        configuration aold(a_);

        for( size_type s = 0; s < steps; ++s )
        {
            update_positions(x_,v_,a_,dt);
            swap(a_,aold);
            calculate_forces(a_,x_);
            update_velocities(v_,aold,a_,dt);
        }
    }
    
    /// dump results to files
    void dump(scalar_type time, int step) const
    {
        // calculate kinetic and potential energy of the current configuration
        measure_energies(time,step);
        
        // write visulatization data
#ifdef PRINT_CONFIGS
        print_config(step);
#endif //PRINT_CONFIGS
    }

private:
    typedef std::vector<position> configuration;

    void update_positions(configuration& x, const configuration& v, const configuration& a, scalar_type dt)
    {
        for( size_type i = 0; i < x.size(); ++i )
        {
            for( size_type d = 0; d < DIMENSIONS; ++d )
            {
                // Verlet step
                x[i][d] += v[i][d]*dt + 0.5*dt*dt*a[i][d];

                // enforce periodic boundaries
                x[i][d] = fmod(x[i][d],extent_[d]);
                if( x[i][d] <  0 )   x[i][d] += extent_[d];
                assert( x[i][d] >= 0 && x[i][d] < extent_[d] );
            }
        }
    }

    void update_velocities(configuration& v, const configuration& aold, const configuration& a, scalar_type dt)
    {
        for( size_type i = 0; i < v.size(); ++i )
        {
            for( size_type d = 0; d < DIMENSIONS; ++d )
                v[i][d] += 0.5*dt*(aold[i][d] + a[i][d]);
        }
    }

    void calculate_forces(configuration& a, configuration& x)
    {
        for( size_type i = 0; i < x.size(); ++i )
        {
            const position& xx = x[i];
            position& aa = a[i];
            std::fill(aa.begin(),aa.end(),scalar_type(0));

            for( size_type j = 0; j < x.size(); ++j )
            {
                if( j == i ) continue;
                potential_.add_force(aa,xx,x[j],extent_);
            }
        }
    }
    
    /// print the current configuration
    void print_config(int step) const
    {
        char buf[500];
        sprintf(buf, "data_%8.8d.csv", step);
        
        FILE * f = fopen(buf, "w");
        
        fprintf(f,"%s,%s,%s,%s,%s,%s\n","x","y","z","vx","vy","vz");
        
        for( size_type i = 0; i < x_.size(); ++i )
            fprintf(f,"%f,%f,%f,%f,%f,%f,\n", x_[i][0],x_[i][1],0.0,v_[i][0],v_[i][1],0.0);
        
        fclose(f);
    }
    
    /// calculate kinetic and potential energy of the current configuration
    void measure_energies(scalar_type time, int step) const
    {
        scalar_type etot = 0.0, epot = 0.0, ekin = 0.0;
        
        for( size_type i = 0; i < x_.size(); ++i )
        {
            const position& xx = x_[i];
            const position& vv = v_[i];
            
            ekin += std::inner_product(vv.begin(),vv.end(),vv.begin(),scalar_type(0));
            
            for( size_type j = 0; j < i; ++j ) // consider every pair only once
                epot += potential_(xx,x_[j],extent_);
        }
        
        ekin *= 0.5;
        etot = epot + ekin;
        
        // write to file
        std::string filename = "energies.dat";
        
        // write header
        std::ofstream f;
        f.open (filename.c_str(), std::ofstream::in);
        if ( f.good() )
            f.close();
        else
        {
            f.close();
            f.open (filename.c_str(), std::ofstream::out);
            f << "step   t   etot   epot   ekin" << std::endl;
            f.close();
        }
        
        // write data
        f.open (filename.c_str(), std::ofstream::app);
        f << step << " " << time;
        f << std::scientific;
        f << " " << etot;
        f << " " << epot;
        f << " " << ekin;
        f << std::endl;
        f.close();
    }

    position extent_; /// system extent along each dimension
    potential potential_;

    configuration x_; /// particle positions
    configuration v_; /// particle velocities
    configuration a_; /// forces on particles
};

/// seed particles on a lattice
std::vector<position> init_square_lattice(const position& extent, size_type n)
{
    assert( DIMENSIONS == 2);
    std::vector<position> p;
    size_type perrows = static_cast<size_type>(std::ceil(std::sqrt(n)));

    scalar_type deltax = extent[0] / perrows;
    scalar_type deltay = extent[1] / perrows;
    scalar_type offsetx = deltax*0.1;
    scalar_type offsety = deltay*0.1;
    for(size_type i=0; i < perrows; ++i)
    {
        for(size_type j=0; j < perrows; ++j)
        {
            if(p.size() >= n)
                break;
            position np{{i*deltax+offsetx,j*deltay+offsety}};
            assert( np[0] < extent[0] );
            assert( np[1] < extent[1] );
            p.push_back(np);
        }
    }
    return p;
}

/// create random velocity distribution for n particles with total kinetic energy ekin
std::vector<position> init_velocities(size_type n, scalar_type ekin)
{
    if( ekin < 0 )
        throw std::runtime_error("init_velocities: cannot set negative kinetic energy "+std::to_string(ekin));

    // Gaussian velocity distribution
    std::mt19937 gen(42);
    for( size_type i = 0; i < 1000000; ++i )    gen();
    std::normal_distribution<scalar_type> dist(0,1);
    std::vector<position> v(n);
    for( position& vv : v )
        std::generate(vv.begin(),vv.end(),std::bind(dist,std::ref(gen)));

    // T = 1/2 \sum_i v_i^2
    scalar_type t = 0;
    for( const position& vv : v )
        t += 0.5 * std::inner_product(vv.begin(),vv.end(),vv.begin(),scalar_type(0));

    // rescale v distribution
    scalar_type lambda = std::sqrt(ekin/t);
    for( position& vv : v )
        std::transform(vv.begin(),vv.end(),vv.begin(),[lambda](scalar_type s) { return lambda*s; });
    return v;
}

int main(int argc, const char** argv)
{
    try
    {
        // get parameters from command line
        if( argc < 8 || argc > 9 ){
            std::cerr << "Usage: " << argv[0] << " [box_length] [# particles] [r_m] [epsilon] [time step] [# time steps] [print steps] [ekin]" << std::endl
                      << "    e.g.  " << argv[0] << " 1.0 100 0.05 5.0 1e-7 1000000 1000 1e3" << std::endl;
            return -1;
        }
        scalar_type box_length = std::atof(argv[1]);
        size_type   particles  = std::atoi(argv[2]);
        scalar_type rm         = std::atof(argv[3]);
        scalar_type eps        = std::atof(argv[4]);
        scalar_type dt         = std::atof(argv[5]);
        size_type   steps      = std::atoi(argv[6]);
        size_type   printsteps = std::atoi(argv[7]);
        scalar_type ekinpp     = std::atof(argv[8]);

        // init potential
        potential pot(rm,eps);

        // init particle positions
        position extent;
        std::fill(extent.begin(),extent.end(),box_length);
        std::vector<position> x;
        x = init_square_lattice(extent,particles);

        assert(particles == x.size());
        std::cout << "# nparticles = " << particles << std::endl;

        // init particle velocities
        std::vector<position> v = init_velocities(particles, ekinpp);

        // init and run simulation for [steps] steps
        simulation sim(extent,pot,x,v);
        for( size_type i = 0; i < steps/printsteps; ++i )
        {
            // print energies and configuration every [printsteps] steps
            int isteps = i*printsteps;
            std::cout << "# STEP " << isteps << std::endl;
            sim.dump(((scalar_type)isteps)*dt,isteps);
            
            // run for [printsteps] steps
            timer t;
            t.start();
            sim.evolve(dt,printsteps);
            t.stop();
            std::cout << "Timing: time=" << t.get_timing() << " nparticles=" << particles << " steps=" << printsteps << std::endl;
        }
        
        // print last step
        std::cout << "# STEP " << steps << std::endl;
        sim.dump(((scalar_type)steps)*dt,steps);
    }
    catch( std::exception& e )
    {
        std::cerr << e.what() << std::endl;
        throw;
    }
}

