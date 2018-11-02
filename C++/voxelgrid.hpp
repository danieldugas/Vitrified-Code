#ifndef SEGMATCH_VOXELGRID_HPP_
#define SEGMATCH_VOXELGRID_HPP_

#include <vector>

#include <glog/logging.h>

#include "segmatch/common.hpp"

namespace segmatch {

class VoxelGrid {
 public:
  VoxelGrid(size_t resolution,
      const float& x_min, const float& y_min, const float& z_min,
      const float& dx, const float& dy, const float& dz) :
    VoxelGrid(resolution, resolution, resolution, x_min, y_min, z_min, dx, dy, dz) {}
  VoxelGrid(size_t n_row, size_t n_col, size_t n_hgt,
      const float& x_min, const float& y_min, const float& z_min,
      const float& dx, const float& dy, const float& dz) :
                                      n_row_(n_row),
                                      n_col_(n_col),
                                      n_hgt_(n_hgt),
                                      x_min_(x_min),
                                      y_min_(y_min),
                                      z_min_(z_min),
                                      x_max_(x_min + dx * n_row),
                                      y_max_(y_min + dy * n_col),
                                      z_max_(z_min + dz * n_hgt),
                                      dx_(dx),
                                      dy_(dy),
                                      dz_(dz)

  {
                                         grid_.reserve(n_row_*n_col_*n_hgt_);
                                         std::fill(grid_.begin(), grid_.end(), 0.);
  }
  float& at(size_t i, size_t j, size_t k) {
    return grid_.at(flatindex(i, j, k));
  }
  size_t flatindex(const size_t& i, const size_t& j, const size_t& k) const {
    return (i * n_col_ * n_hgt_ + j * n_col_ + k);
  }
  std::array<float, 3> ijk_to_xyz(const size_t& i, const size_t& j, const size_t& k) const {
    // xyz values correspond to the lower left corner of a voxel
    float x = i * dx_ + x_min_;
    float y = j * dy_ + y_min_;
    float z = k * dz_ + z_min_;
    std::array<float, 3> xyz = {x, y, z};
    return xyz;
  }
  std::array<size_t, 3> xyz_to_ijk(const float& x, const float& y, const float& z) const {
    int i = ((x - x_min_ ) / dx_);
    int j = ((y - y_min_ ) / dy_);
    int k = ((z - z_min_ ) / dz_);
    if ( i >= n_row_ ) { i = n_row_ - 1; };
    if ( j >= n_col_ ) { j = n_col_ - 1; };
    if ( k >= n_hgt_ ) { k = n_hgt_ - 1; };
    if ( i < 0 ) { i = 0; };
    if ( j < 0 ) { j = 0; };
    if ( k < 0 ) { k = 0; };
    std::array<size_t, 3> ijk = {static_cast<size_t>(i),
                                 static_cast<size_t>(j),
                                 static_cast<size_t>(k)};
    return ijk;
  }
  size_t n_row() const { return n_row_; }
  size_t n_col() const { return n_col_; }
  size_t n_hgt() const { return n_hgt_; }
  float x_min() const { return x_min_; }
  float y_min() const { return y_min_; }
  float z_min() const { return z_min_; }
  float x_max() const { return x_max_; }
  float y_max() const { return y_max_; }
  float z_max() const { return z_max_; }
  float dx() const { return dx_; }
  float dy() const { return dy_; }
  float dz() const { return dz_; }
 private:
  // voxelgrid metadata
  size_t n_row_;
  size_t n_col_;
  size_t n_hgt_;
  float x_min_; // extremum of the voxelgrid in x dimension
  float y_min_; // [ ][ ][ ]
  float z_min_; // |xmin   |xmax
  float x_max_; // extremum of the voxelgrid in x dimension
  float y_max_;
  float z_max_;
  float dx_; // size of a voxel in x dimension
  float dy_;
  float dz_;
  // data
  std::vector<float> grid_;
};


} // namespace segmatch

#endif // SEGMATCH_VOXELGRID_HPP_
