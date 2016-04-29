#ifndef HPCSE_BARRIER_HPP
#define HPCSE_BARRIER_HPP

#include <thread>
#include <mutex>
#include <cassert>
#include <limits>

class barrier
{
public:
    barrier(unsigned int count)
    : m_total(count)
    , m_count(count)
    , m_generation(0)
    {
        assert(count != 0);
    }
    
    void wait()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        unsigned int gen = m_generation;
        
        // decrease the count
        if (--m_count==0) {
            // if done reset to new generation of wait
            m_count = m_total;
            m_generation++;
        }
        else {
            lock.unlock();
            while (true) {
                lock.lock();
                if (gen != m_generation)
                    break;
                lock.unlock();
            }
        }
    }
    
    unsigned int num_waiting() const
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_count;
    }
    
private:
    mutable std::mutex m_mutex;
    unsigned long const m_total;
    unsigned long m_count;
    unsigned long m_generation;
};

#endif //HPCSE_BARRIER_HPP
