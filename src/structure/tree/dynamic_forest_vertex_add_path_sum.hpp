#pragma once

#include <stdexcept>
#include <utility>
#include <vector>

#include "../types/common_monoids.hpp"
#include "lazy_link_cut_tree.hpp"
#include "lazy_top_tree.hpp"
#include "link_cut_tree.hpp"
#include "top_tree.hpp"

namespace dynamic_forest_vertex_add_path_sum_internal{

template<class T>
inline constexpr AddMonoid<T> add_monoid{};

template<class T>
inline constexpr AddSumMonoidAct<T> add_sum_monoid_act{};

template<class Backend, class T, int MAX_SIZE>
class Structure{
    Backend tree;

    void require_connected(int left, int right, const char* message){
        if(!tree.connected(left, right))[[unlikely]]{
            throw std::runtime_error(message);
        }
    }

public:
    explicit Structure(int n = MAX_SIZE): tree(n){}

    explicit Structure(const std::vector<T>& values): tree(values){}

    Structure(const Structure&) = delete;
    Structure& operator=(const Structure&) = delete;
    Structure(Structure&&) = default;
    Structure& operator=(Structure&&) = default;

    int size() const{
        return tree.size();
    }

    bool connected(int left, int right){
        return tree.connected(left, right);
    }

    bool link(int left, int right){
        return tree.link(left, right);
    }

    bool cut(int left, int right){
        return tree.cut(left, right);
    }

    bool replace_edge(int old_left, int old_right, int new_left, int new_right){
        if(!tree.cut(old_left, old_right)) return false;
        if(tree.link(new_left, new_right)) return true;
        const bool restored = tree.link(old_left, old_right);
        if(!restored)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: failed to restore edge (replace_edge)."
            );
        }
        return false;
    }

    void add_vertex(int vertex, const T& delta){
        if constexpr(requires(Backend& backend){
            backend.path_apply(vertex, vertex, delta);
        }){
            tree.path_apply(vertex, vertex, delta);
        }else{
            tree.set(vertex, tree.get(vertex) + delta);
        }
    }

    T path_sum(int left, int right){
        require_connected(
            left,
            right,
            "library assertion fault: disconnected path (path_sum)."
        );
        return tree.path_prod(left, right);
    }
};

template<class T, int MAX_SIZE>
using LinkCutBackend = LinkCutTree<add_monoid<T>, MAX_SIZE>;

template<class T, int MAX_SIZE>
using LazyLinkCutBackend = LazyLinkCutTree<add_sum_monoid_act<T>, MAX_SIZE>;

template<class T, int MAX_SIZE>
using TopTreeBackend = TopTree<add_monoid<T>, MAX_SIZE>;

template<class T, int MAX_SIZE>
using LazyTopTreeBackend = LazyTopTree<add_sum_monoid_act<T>, MAX_SIZE>;

} // namespace dynamic_forest_vertex_add_path_sum_internal

template<class T, int MAX_SIZE>
using DynamicForestVertexAddPathSumLinkCutTree =
    dynamic_forest_vertex_add_path_sum_internal::Structure<
        dynamic_forest_vertex_add_path_sum_internal::LinkCutBackend<T, MAX_SIZE>,
        T,
        MAX_SIZE
    >;

template<class T, int MAX_SIZE>
using DynamicForestVertexAddPathSumLazyLinkCutTree =
    dynamic_forest_vertex_add_path_sum_internal::Structure<
        dynamic_forest_vertex_add_path_sum_internal::LazyLinkCutBackend<T, MAX_SIZE>,
        T,
        MAX_SIZE
    >;

template<class T, int MAX_SIZE>
using DynamicForestVertexAddPathSumTopTree =
    dynamic_forest_vertex_add_path_sum_internal::Structure<
        dynamic_forest_vertex_add_path_sum_internal::TopTreeBackend<T, MAX_SIZE>,
        T,
        MAX_SIZE
    >;

template<class T, int MAX_SIZE>
using DynamicForestVertexAddPathSumLazyTopTree =
    dynamic_forest_vertex_add_path_sum_internal::Structure<
        dynamic_forest_vertex_add_path_sum_internal::LazyTopTreeBackend<T, MAX_SIZE>,
        T,
        MAX_SIZE
    >;
