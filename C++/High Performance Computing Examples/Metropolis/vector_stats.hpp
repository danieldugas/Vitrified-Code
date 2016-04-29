#ifndef HPCSE_VECTOR_STATS_HPP
#define HPCSE_VECTOR_STATS_HPP

#include <vector>
#include <cmath>
#include <cassert>

class VectorStats {
public:
    typedef std::size_t size_type;
    typedef double value_type;
    
    VectorStats(size_type M)
    : count_(0), sum_(M, 0.), sum2_(M, 0.)
    { }
    
    void operator<<(std::vector<value_type> const& v)
    {
        assert(v.size() == sum_.size());
        
        for (size_type i=0; i<v.size(); ++i) {
            sum_[i]  += v[i];
            sum2_[i] += v[i]*v[i];
        }
        count_ += 1;
    }

    void operator<<(VectorStats const& rhs)
    {
        assert(rhs.sum_.size() == sum_.size());
        
        for (size_type i=0; i<sum_.size(); ++i) {
            sum_[i]  += rhs.sum_[i];
            sum2_[i] += rhs.sum2_[i];
//            std::cout << "rhs.sum[i] : " << rhs.sum_[i] << std::endl;
//            std::cout << "rhs.sum2[i] : " << rhs.sum2_[i] << std::endl;
        }
        count_ += rhs.count_;
    }

    size_type count() const
    {
        return count_;
    }

    std::vector<value_type> mean() const
    {
        std::vector<value_type> v(sum_);
        for (auto& vi : v) vi /= count_;
        
        return v;
    }
    
    std::vector<value_type> error() const
    {
        std::vector<value_type> v(sum_.size());
        for (size_type i=0; i<v.size(); ++i)
            v[i] = std::sqrt( (sum2_[i] - sum_[i]*sum_[i]/double(count_)) / double(count_*(count_-1)) );
        return v;
    }
    
    
private:
    size_type count_;
    std::vector<value_type> sum_, sum2_;
};

#endif
