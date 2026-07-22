#ifndef CPPLIB_SRC_ALGORITHM_TREE_TREE_PATH_KTH_VALUE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_TREE_TREE_PATH_KTH_VALUE_HPP_INCLUDED

#include <vector>

#include "tree_path_order_statistics.hpp"

template<class Value, int MAX_SIZE>
struct TreePathKthValue{
private:
    TreePathOrderStatistics<Value, MAX_SIZE> statistics;

public:
    explicit TreePathKthValue(int n): statistics(n){}
    void add_edge(int left, int right){ statistics.add_edge(left, right); }
    void build(const std::vector<Value>& values, int root = 0){ statistics.build(values, root); }
    Value kth_smallest(int left, int right, int order) const{
        return statistics.kth_smallest(left, right, order);
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_TREE_TREE_PATH_KTH_VALUE_HPP_INCLUDED
