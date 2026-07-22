#ifndef CPPLIB_SRC_STRUCTURE_GRAPH_DYNAMIC_GRAPH_CONNECTIVITY_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_GRAPH_DYNAMIC_GRAPH_CONNECTIVITY_HPP_INCLUDED

#include "online_dynamic_connectivity.hpp"

template<int MAX_SIZE>
struct DynamicGraphConnectivity{
private:
    OnlineDynamicConnectivity<MAX_SIZE> graph;

public:
    explicit DynamicGraphConnectivity(int n): graph(n){}
    bool add_edge(int left, int right){ return graph.add_edge(left, right); }
    bool remove_edge(int left, int right){ return graph.erase_edge(left, right); }
    bool connected(int left, int right) const{ return graph.same(left, right); }
};

#endif  // CPPLIB_SRC_STRUCTURE_GRAPH_DYNAMIC_GRAPH_CONNECTIVITY_HPP_INCLUDED
