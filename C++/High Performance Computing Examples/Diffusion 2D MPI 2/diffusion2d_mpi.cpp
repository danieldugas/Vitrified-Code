#include <iostream>
#include <algorithm>
#include <functional>
#include <iterator>
#include <array>
#include <string>
#include <fstream>
#include <sstream>
#include <cassert>
#include <vector>
#include <cmath>

#include <mpi.h>

typedef double value_type;
typedef std::size_t size_type;

#define TAG 0

struct world_info
{
    int size;
    int dims_x;
    int dims_y;
    
    int left_proc;
    int right_proc;
    int top_proc;
    int bottom_proc;
    
    int rank;
    int cart_rank;
    int coord_x;
    int coord_y;
    
} world;

struct position
{
    value_type x,y;
};

class Diffusion2D
{
    struct position_ext
    {
        value_type x,y,rho;
    };
    
public:
    
    Diffusion2D(
                const value_type d,
                const value_type rmax,
                const value_type rmin,
                const size_type N
                )
    :
    d_(d)
    , rmax_(rmax)
    , rmin_(rmin)
    {
        /// global grid
        Nx_glo = N;
        Ny_glo = N;
        NN_glo = Nx_glo * Ny_glo;
        
        /// local grid
        Nx_loc = Nx_glo / world.dims_x;
        Ny_loc = Ny_glo / world.dims_y;
        NN_loc = Nx_loc * Ny_loc;
        
        /// build periodic process geometry with cartesian communicator
        int periods[2] = {false, false};
        int dims[2] = {world.dims_x, world.dims_y};
        
        MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, true, &cart_comm);
        
        MPI_Comm_rank(cart_comm,&world.cart_rank);
        
        MPI_Cart_shift(cart_comm, 0, 1, &world.left_proc, &world.right_proc);
        MPI_Cart_shift(cart_comm, 1, 1, &world.bottom_proc, &world.top_proc);
        
        int coords[2];
        MPI_Cart_coords(cart_comm, world.cart_rank, 2, coords);
        
        world.coord_x = coords[0];
        world.coord_y = coords[1];
        
        /// build contiguous (rows) and strided vectors (columns) for boundaries.
        /// each process has a rectangular tile in the cartesian grid
        MPI_Type_contiguous(Nx_loc, MPI_DOUBLE, &bottom_boundary);
        MPI_Type_commit(&bottom_boundary);
        
        MPI_Type_contiguous(Nx_loc, MPI_DOUBLE, &top_boundary);
        MPI_Type_commit(&top_boundary);
        
        MPI_Type_vector(Ny_loc, 1, Nx_loc + 2, MPI_DOUBLE, &left_boundary);
        MPI_Type_commit(&left_boundary);
        
        MPI_Type_vector(Ny_loc, 1, Nx_loc + 2, MPI_DOUBLE, &right_boundary);
        MPI_Type_commit(&right_boundary);
        
        /// real space grid spacing
        dr_ = (rmax_ - rmin_) / (N - 1);
        
        /// sub-domain boundaries
        xmin_loc = rmin + world.coord_x * Nx_loc * dr_;
        xmax_loc = xmin_loc + (Nx_loc - 1) * dr_;
        ymin_loc = rmin + world.coord_y * Ny_loc * dr_;
        ymax_loc = ymin_loc + (Ny_loc - 1) * dr_;
        
        /// dt < dx*dx / (4*d) for stability
        dt_ = dr_ * dr_ / (6 * d_);
        
        /// stencil factor
        fac_ = dt_ * d_ / (dr_ * dr_);
        
        /// allocate and fill density
        rho_ = new value_type[(Nx_loc + 2) * (Ny_loc + 2)];
        rho_tmp = new value_type[(Nx_loc + 2) * (Ny_loc + 2)];
        
        std::fill(rho_, rho_ + (Nx_loc + 2) * (Ny_loc + 2), 0.0);
        std::fill(rho_tmp, rho_tmp + (Nx_loc + 2) * (Ny_loc + 2), 0.0);
    }
    
    ~Diffusion2D()
    {
        delete[] rho_;
        delete[] rho_tmp;
        
        MPI_Type_free(&left_boundary);
        MPI_Type_free(&right_boundary);
        MPI_Type_free(&bottom_boundary);
        MPI_Type_free(&top_boundary);
        
        MPI_Comm_free(&cart_comm);
    }
    
    void initialize();
    void propagate_density();
    
    inline position get_position(size_type i, size_type j) const;
    
    value_type get_size();
    value_type get_moment();
    value_type get_time() const {return time_;}
    
    void write_density(const std::string file_name) const;
    
private:
    
    inline void stencil(const size_type i, const size_type j);
    
    const value_type d_, rmax_, rmin_;
    
    value_type xmin_loc, ymin_loc;
    value_type xmax_loc, ymax_loc;
    
    int NN_loc, Nx_loc, Ny_loc;
    int NN_glo, Nx_glo, Ny_glo;
    
    value_type dr_, dt_, fac_;
    
    value_type time_;
    
    value_type *rho_, *rho_tmp;
    
    MPI_Datatype left_boundary, right_boundary, bottom_boundary, top_boundary;
    
    MPI_Comm cart_comm;
};

inline position Diffusion2D::get_position(size_type i, size_type j) const
{
    position p;
    p.x = xmin_loc + j*dr_;
    p.y = ymin_loc + i*dr_;
    return p;
}

value_type Diffusion2D::get_size()
{
    value_type sum_local = 0.0;
    for(int i = 0; i < Ny_loc; ++i)
        for(int j = 0; j < Nx_loc; ++j){
            const size_type ind_rho = (i+1)*(Nx_loc+2) + (j+1);
            sum_local += rho_[ind_rho];
        }
    
    /// fetch partial sum from all processes
    value_type sum = 0.0;
    MPI_Reduce(&sum_local, &sum, 1, MPI_DOUBLE, MPI_SUM, 0, cart_comm);
    
    return dr_*dr_*sum;
}

value_type Diffusion2D::get_moment()
{
    value_type sum_local = 0.0;
    for(int i = 0; i < Ny_loc; ++i)
        for(int j = 0; j < Nx_loc; ++j){
            const size_type ind_rho = (i+1)*(Nx_loc+2) + (j+1);
            const position p = get_position(i,j);
            sum_local += rho_[ind_rho] * (p.x * p.x + p.y * p.y);
        }
    
    /// fetch partial sum from all processes
    value_type sum = 0.0;
    MPI_Reduce(&sum_local, &sum, 1, MPI_DOUBLE, MPI_SUM, 0, cart_comm);
    
    return dr_*dr_*sum;
}

inline void Diffusion2D::stencil(const size_type i, const size_type j)
{
    const size_type ind = i*(Nx_loc+2) + j;
    
    /// FTCS stencil
    rho_tmp[ind] =
    rho_[ind]
    +
    fac_
    *
    (
     rho_[i*(Nx_loc+2) + (j+1)]
     +
     rho_[i*(Nx_loc+2) + (j-1)]
     +
     rho_[(i+1)*(Nx_loc+2) + j]
     +
     rho_[(i-1)*(Nx_loc+2) + j]
     -
     4*rho_[ind]
     );
}

void Diffusion2D::write_density(const std::string file_name) const
{
    /// delete previous instance of file so we define a new striping pattern
    MPI_File_delete((char*)file_name.c_str(), MPI_INFO_NULL);
    
    /// create MPI data struct
    int blocklength[1] = {3};
    MPI_Aint offsets[1] = {0};
    MPI_Datatype types[1] = {MPI_DOUBLE};
    
    MPI_Datatype position_ext_MPI;
    MPI_Type_create_struct(1, blocklength, offsets, types, &position_ext_MPI);
    MPI_Type_commit(&position_ext_MPI);
    
    /// prepare grid and rho for I/O
    position_ext* data = new position_ext[NN_loc];
    for(int i=0; i < Ny_loc; i++)
        for(int j=0; j < Nx_loc; j++){
            size_type ind = i*Nx_loc + j;
            size_type ind_rho = (i+1)*(Nx_loc+2) + (j+1);
            
            position p = get_position(i,j);
            data[ind].x   = p.x;
            data[ind].y   = p.y;
            data[ind].rho = rho_[ind_rho];
        }
    
    /// allocate subarray for sub-domain
    int dim_glo[2] = {Ny_glo, Nx_glo};
    int dim_loc[2] = {Ny_loc, Nx_loc};
    int dim_ind[2] = {world.coord_y * Ny_loc, world.coord_x * Nx_loc};
    
    MPI_Datatype tile;
    MPI_Type_create_subarray(2, dim_glo, dim_loc, dim_ind, MPI_ORDER_C, position_ext_MPI, &tile);
    MPI_Type_commit(&tile);
    
    /// stripe the output file for faster I/O
    MPI_Info info;
    MPI_Info_create(&info);
    MPI_Info_set(info, (char*)"striping_unit", (char*)"1048576"); // 1MB -> default stripe size of Brutus Lustre filesystem
    
    /// open the file and allocal a local view per process
    MPI_File file;
    MPI_File_open(cart_comm, (char*)file_name.c_str(), MPI_MODE_CREATE|MPI_MODE_WRONLY, info, &file);
    MPI_File_set_view(file, 0, position_ext_MPI, tile, (char*)"native", info);
    
    /// write to file collectively
    MPI_Status status;
    MPI_File_write_all(file, data, NN_loc, position_ext_MPI, &status);
    delete[] data;
    
    /// clean
    MPI_File_close(&file);
    MPI_Type_free(&tile);
    MPI_Type_free(&position_ext_MPI);
    
    /// check status
    int bytes;
    MPI_Get_elements(&status, MPI_CHAR, &bytes);
    std::cout.precision(2);
    std::cout << "process " << world.rank << " wrote " << value_type(bytes) / 1048576 << " MB" << std::endl;
}

void Diffusion2D::propagate_density()
{
    MPI_Request reqs[8];
    MPI_Status status[8];
    
    /// exchange boundaries along x-direction
    if(world.coord_x % 2 == 0){
        
        MPI_Isend(&rho_[(Nx_loc+2)+1],1,left_boundary,world.left_proc,TAG,cart_comm,&reqs[0]);
        MPI_Irecv(&rho_[Nx_loc+2],1,left_boundary,world.left_proc,TAG,cart_comm,&reqs[1]);
        MPI_Isend(&rho_[(Nx_loc+2)+Nx_loc],1,right_boundary,world.right_proc,TAG,cart_comm,&reqs[2]);
        MPI_Irecv(&rho_[(Nx_loc+2)+Nx_loc+1],1,right_boundary,world.right_proc,TAG,cart_comm,&reqs[3]);
        
    }
    else{
        
        MPI_Irecv(&rho_[(Nx_loc+2)+Nx_loc+1],1,right_boundary,world.right_proc,TAG,cart_comm,&reqs[0]);
        MPI_Isend(&rho_[(Nx_loc+2)+Nx_loc],1,right_boundary,world.right_proc,TAG,cart_comm,&reqs[1]);
        MPI_Irecv(&rho_[Nx_loc+2],1,left_boundary,world.left_proc,TAG,cart_comm,&reqs[2]);
        MPI_Isend(&rho_[(Nx_loc+2)+1],1,left_boundary,world.left_proc,TAG,cart_comm,&reqs[3]);
        
    }
    
    /// exchange boundaries along y-direction
    if(world.coord_y % 2 == 0){
        
        MPI_Isend(&rho_[(Nx_loc+2)+1],1,bottom_boundary,world.bottom_proc,TAG,cart_comm,&reqs[4]);
        MPI_Irecv(&rho_[1],1,bottom_boundary,world.bottom_proc,TAG,cart_comm,&reqs[5]);
        MPI_Isend(&rho_[(Nx_loc+2)*(Ny_loc)+1],1,top_boundary,world.top_proc,TAG,cart_comm,&reqs[6]);
        MPI_Irecv(&rho_[(Nx_loc+2)*(Ny_loc+1)+1],1,top_boundary,world.top_proc,TAG,cart_comm,&reqs[7]);
        
    }
    else{
        
        MPI_Irecv(&rho_[(Nx_loc+2)*(Ny_loc+1)+1],1,top_boundary,world.top_proc,TAG,cart_comm,&reqs[4]);
        MPI_Isend(&rho_[(Nx_loc+2)*(Ny_loc)+1],1,top_boundary,world.top_proc,TAG,cart_comm,&reqs[5]);
        MPI_Irecv(&rho_[1],1,bottom_boundary,world.bottom_proc,TAG,cart_comm,&reqs[6]);
        MPI_Isend(&rho_[(Nx_loc+2)+1],1,bottom_boundary,world.bottom_proc,TAG,cart_comm,&reqs[7]);
        
    }
    
    /// update interior of subdomain
    for(int i = 2; i < Ny_loc; ++i)
        for(int j = 2; j < Nx_loc; ++j)
            stencil(i,j);
    
    /// ensure boundaries have arrived
    MPI_Waitall(8,reqs,status);
    
    /// update the rest
    for(int i = 1; i < Ny_loc+1; ++i){
        stencil(i,1);      /// left column
        stencil(i,Nx_loc); /// right column
    }
    
    for(int j = 1; j < Nx_loc+1; ++j){
        stencil(1,j);      /// bottom row
        stencil(Ny_loc,j); /// top row
    }
    
    /// assign new density
    using std::swap;
    swap(rho_tmp,rho_);
    
    time_ += dt_;
}

void Diffusion2D::initialize()
{
    /// initialize grid in non-overlapping sub-domains
    
    time_ = 0;
    
    const value_type bound = 1./2;
    
    /// initialize rho(x,y,t=0)
    
    for(int i = 0; i < Ny_loc; ++i)
        for(int j = 0; j < Nx_loc; ++j){
            const size_type ind_rho = (i+1)*(Nx_loc+2) + (j+1);
            const position p = get_position(i,j);
            
            if(std::fabs(p.x) < bound && std::fabs(p.y) < bound){
                rho_[ind_rho] = 1;
            }
            else{
                rho_[ind_rho] = 0;
            }
            
        }
}

int main(int argc, char* argv[])
{
    /// initialize MPI domain
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &world.size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world.rank);
    
    int dims[2] = {0,0};
    MPI_Dims_create(world.size, 2, dims);
    world.dims_x = dims[0];
    world.dims_y = dims[1];
    
    if(world.rank == 0)
        std::cout
        << "processes: " << world.size << "\n"
        << "dims_x: " << world.dims_x << "\n"
        << "dims_y: " << world.dims_y << "\n"
        << std::endl;
    
    /// params
    
    assert(argc == 2);
    const size_type tmp = 1 << std::stoul(argv[1]);
    
    /// make gridpoints multiple of procs in both directions
    const size_type N_ = tmp % world.size == 0 ? tmp : tmp + (world.size - tmp % world.size);
    assert(N_ % world.size == 0);
    
    const value_type d_ = 1;
    const value_type tmax = 0.0002;
    const value_type rmax = 1;
    const value_type rmin = -1;
    
    if (world.rank == 0)
        std::cout
        << "domain: " << N_ << " x " << N_ << "\n"
        << "diffusion coefficient: " << d_ << "\n"
        << "tmax: " << tmax << "\n"
        << "rmax: " << rmax << "\n"
        << "rmin: " << rmin << "\n"
        << std::endl;
    
    {
        Diffusion2D system(d_, rmax, rmin, N_);
        system.initialize();
        
        MPI_Barrier(MPI_COMM_WORLD);
        
        value_type time = 0;
        const size_type max_steps = 20;
        
        double start = MPI_Wtime();
        
        //while(time < tmax){
        for(size_type steps = 0; steps < max_steps; ++steps){
            system.propagate_density();
            time = system.get_time();
            value_type sys_size = system.get_size();
            value_type sys_moment = system.get_moment();
            if(world.rank == 0) std::cout << time << '\t' << sys_size << '\t' << sys_moment << std::endl;
            /// note: we don't need an MPI_Barrier, because the measurements already invoke a collective communication
        }
        
        double end = MPI_Wtime();
        
        double elapsed = end-start;
        
        std::string density_file = "Density.dat";
        system.write_density(density_file);
        
        if(world.rank == 0){
            
            std::string sep = "_";
            
            std::cout
            << "performance: " << '\t'
            << world.size << '\t'
            << elapsed << '\t'
            << N_ << '\t'
            << "\n*********\n"
            << std::endl;
            
        }
        
    } /// destroy system before finalizing
    
    MPI_Finalize();
    
    return 0;
}
