#ifndef CPPLIB_SRC_STRUCTURE_GRAPH_SPANNING_FOREST_INCREMENTAL_MINIMUM_SPANNING_FOREST_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_GRAPH_SPANNING_FOREST_INCREMENTAL_MINIMUM_SPANNING_FOREST_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <type_traits>

#include "../../tree/dynamic_forest/link_cut_tree.hpp"

template<class Weight>
struct IncrementalMinimumSpanningForestPathEntry{
    Weight weight{};
    int edge = -1;
};

template<class Weight>
struct IncrementalMinimumSpanningForestMaximumMonoid{
    using S = IncrementalMinimumSpanningForestPathEntry<Weight>;

    constexpr S op(const S& left, const S& right) const{
        if(left.weight < right.weight) return right;
        if(right.weight < left.weight) return left;
        return left.edge < right.edge ? right : left;
    }

    constexpr S e() const{
        return {std::numeric_limits<Weight>::lowest(), -1};
    }
};

template<class Weight, int MAX_VERTICES, int MAX_EDGES>
class IncrementalMinimumSpanningForest{
    static_assert(MAX_VERTICES > 0);
    static_assert(MAX_EDGES > 0);
    static_assert(MAX_VERTICES <= std::numeric_limits<int>::max() - MAX_EDGES);
    static_assert(std::numeric_limits<Weight>::is_specialized);

    using Entry = IncrementalMinimumSpanningForestPathEntry<Weight>;
    static constexpr auto maximum_monoid =
        IncrementalMinimumSpanningForestMaximumMonoid<Weight>{};
    using Forest = LinkCutTree<maximum_monoid, MAX_VERTICES + MAX_EDGES>;

    int vertex_count_ = 0;
    int edge_count_ = 0;
    int component_count_ = 0;
    Forest forest_;
    std::array<int, MAX_EDGES> first_endpoint_{};
    std::array<int, MAX_EDGES> second_endpoint_{};
    std::array<bool, MAX_EDGES> in_forest_{};

    static int storage_size(int vertex_count){
        if(vertex_count < 0 || MAX_VERTICES < vertex_count)[[unlikely]]{
            throw std::invalid_argument(
                "incremental minimum spanning forest size is out of range"
            );
        }
        return vertex_count + MAX_EDGES;
    }

    void check_vertex(int vertex) const{
        if(vertex < 0 || vertex_count_ <= vertex)[[unlikely]]{
            throw std::out_of_range(
                "incremental minimum spanning forest vertex is out of range"
            );
        }
    }

    int edge_node(int edge) const{
        return vertex_count_ + edge;
    }

    void attach_edge(int edge, int first, int second, const Weight& weight){
        const int node = edge_node(edge);
        forest_.set(node, Entry{weight, edge});
        if(!forest_.link(first, node) || !forest_.link(node, second))[[unlikely]]{
            throw std::logic_error(
                "incremental minimum spanning forest link invariant failed"
            );
        }
        first_endpoint_[static_cast<std::size_t>(edge)] = first;
        second_endpoint_[static_cast<std::size_t>(edge)] = second;
        in_forest_[static_cast<std::size_t>(edge)] = true;
    }

    void detach_edge(int edge){
        const int node = edge_node(edge);
        const int first = first_endpoint_[static_cast<std::size_t>(edge)];
        const int second = second_endpoint_[static_cast<std::size_t>(edge)];
        if(!forest_.cut(first, node) || !forest_.cut(node, second))[[unlikely]]{
            throw std::logic_error(
                "incremental minimum spanning forest cut invariant failed"
            );
        }
        in_forest_[static_cast<std::size_t>(edge)] = false;
    }

public:
    explicit IncrementalMinimumSpanningForest(int vertex_count):
        vertex_count_(vertex_count),
        component_count_(vertex_count),
        forest_(storage_size(vertex_count)){}

    int vertex_count() const{
        return vertex_count_;
    }

    int edge_count() const{
        return edge_count_;
    }

    int component_count() const{
        return component_count_;
    }

    bool contains_edge(int edge) const{
        if(edge < 0 || edge_count_ <= edge)[[unlikely]]{
            throw std::out_of_range(
                "incremental minimum spanning forest edge is out of range"
            );
        }
        return in_forest_[static_cast<std::size_t>(edge)];
    }

    int add_edge(int first, int second, const Weight& weight){
        check_vertex(first);
        check_vertex(second);
        if(edge_count_ == MAX_EDGES)[[unlikely]]{
            throw std::length_error(
                "incremental minimum spanning forest edge capacity exceeded"
            );
        }
        const int edge = edge_count_++;
        if(first == second) return edge;

        if(!forest_.connected(first, second)){
            attach_edge(edge, first, second, weight);
            --component_count_;
            return -1;
        }

        const Entry maximum = forest_.path_prod(first, second);
        if(maximum.edge < 0)[[unlikely]]{
            throw std::logic_error(
                "incremental minimum spanning forest path has no edge"
            );
        }
        if(!(weight < maximum.weight)) return edge;

        detach_edge(maximum.edge);
        attach_edge(edge, first, second, weight);
        return maximum.edge;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_GRAPH_SPANNING_FOREST_INCREMENTAL_MINIMUM_SPANNING_FOREST_HPP_INCLUDED
