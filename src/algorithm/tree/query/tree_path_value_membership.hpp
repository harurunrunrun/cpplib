#ifndef CPPLIB_SRC_ALGORITHM_TREE_QUERY_TREE_PATH_VALUE_MEMBERSHIP_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_TREE_QUERY_TREE_PATH_VALUE_MEMBERSHIP_HPP_INCLUDED

#include <vector>

#include "tree_path_order_statistics.hpp"

template<class Value, int MAX_SIZE>
struct TreePathValueMembership{
private:
    TreePathOrderStatistics<Value, MAX_SIZE> statistics;

public:
    explicit TreePathValueMembership(int n): statistics(n){}
    void add_edge(int left, int right){ statistics.add_edge(left, right); }
    void build(const std::vector<Value>& values, int root = 0){ statistics.build(values, root); }
    bool contains(int left, int right, const Value& value) const{
        return statistics.count_equal(left, right, value) != 0;
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_TREE_QUERY_TREE_PATH_VALUE_MEMBERSHIP_HPP_INCLUDED
