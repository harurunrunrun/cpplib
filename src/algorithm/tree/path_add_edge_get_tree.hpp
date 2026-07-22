#ifndef CPPLIB_SRC_ALGORITHM_TREE_PATH_ADD_EDGE_GET_TREE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_TREE_PATH_ADD_EDGE_GET_TREE_HPP_INCLUDED

#include <stdexcept>
#include <utility>

#include "heavy_light_decomposition.hpp"
#include "../../structure/other/fenwick_tree.hpp"

template<class T, int MAX_SIZE>
class PathAddEdgeGetTree{
    static_assert(0 < MAX_SIZE);

    int vertex_count_;
    HeavyLightDecomposition decomposition_;
    FenwickTree<T, MAX_SIZE> endpoint_difference_;
    bool built_ = false;

    static int checked_size(int vertex_count){
        if(vertex_count < 0 || MAX_SIZE < vertex_count)[[unlikely]]{
            throw std::out_of_range(
                "PathAddEdgeGetTree size is out of range"
            );
        }
        return vertex_count;
    }

    void require_not_built(const char* operation) const{
        if(built_)[[unlikely]] throw std::logic_error(operation);
    }

    void require_built() const{
        if(!built_)[[unlikely]]{
            throw std::logic_error("PathAddEdgeGetTree is not built");
        }
    }

public:
    explicit PathAddEdgeGetTree(int vertex_count)
        : vertex_count_(checked_size(vertex_count)),
          decomposition_(vertex_count_),
          endpoint_difference_(vertex_count_){}

    int size() const{
        return vertex_count_;
    }

    void add_edge(int left, int right){
        require_not_built(
            "PathAddEdgeGetTree cannot add edges after build"
        );
        decomposition_.add_edge(left, right);
    }

    void build(int root = 0){
        require_not_built("PathAddEdgeGetTree is already built");
        decomposition_.build(root);
        built_ = true;
    }

    void path_add(int left, int right, const T& delta){
        require_built();
        const int ancestor = decomposition_.lca(left, right);
        endpoint_difference_.add(decomposition_.in(left), delta);
        endpoint_difference_.add(decomposition_.in(right), delta);
        const T negative_delta = T{} - delta;
        endpoint_difference_.add(
            decomposition_.in(ancestor), negative_delta
        );
        endpoint_difference_.add(
            decomposition_.in(ancestor), negative_delta
        );
    }

    T edge_get(int left, int right) const{
        require_built();
        int child = -1;
        if(decomposition_.parent(left) == right){
            child = left;
        }else if(decomposition_.parent(right) == left){
            child = right;
        }else{
            throw std::invalid_argument(
                "PathAddEdgeGetTree edge_get requires adjacent vertices"
            );
        }
        return endpoint_difference_.sum(
            decomposition_.in(child), decomposition_.out(child)
        );
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_TREE_PATH_ADD_EDGE_GET_TREE_HPP_INCLUDED
