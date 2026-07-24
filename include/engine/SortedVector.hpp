#ifndef SORTEDVECTOR_HPP
#define SORTEDVECTOR_HPP

#include <vector>
#include <algorithm>

template <typename T>
class SortedVector {
  private:
    std::vector<T> sorted_vector{};
    bool (*cmp)(const T&, const T&);
  public:
    SortedVector (bool (*cmp)(const T&, const T&)) { this->cmp = cmp; }
    void insert(const T& element) {
        auto it = std::lower_bound(sorted_vector.begin(), sorted_vector.end(), element, cmp);
        sorted_vector.insert(it, element);
    }

    std::vector<T>& getVector() const {
        return sorted_vector;
    }

    auto begin() { return sorted_vector.begin(); }
    auto end() { return sorted_vector.end(); }
    auto begin() const { return sorted_vector.begin(); }
    auto end() const { return sorted_vector.end(); }
    
};


#endif