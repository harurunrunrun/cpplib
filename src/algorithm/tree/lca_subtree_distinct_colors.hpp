#pragma once

#include <memory>
#include <stdexcept>
#include <vector>

#include "euler_tour.hpp"
#include "lca.hpp"
#include "../range/static_range_count_distinct.hpp"

template<class Color, int MAX_SIZE>
struct LcaSubtreeDistinctColors{
private:
    int _n;
    std::vector<Color> colors;
    std::unique_ptr<TreeEulerTour<MAX_SIZE>> tour;
    LowestCommonAncestor lca_index;
    std::unique_ptr<StaticRangeCountDistinct<Color, MAX_SIZE>> distinct;

public:
    explicit LcaSubtreeDistinctColors(const std::vector<Color>& vertex_colors):
        _n(static_cast<int>(vertex_colors.size())), colors(vertex_colors),
        tour(std::make_unique<TreeEulerTour<MAX_SIZE>>(_n)), lca_index(_n){}

    void add_edge(int left, int right){
        tour->add_edge(left, right);
        lca_index.add_edge(left, right);
    }

    void build(int root){
        tour->build(root);
        lca_index.build(root);
        std::vector<Color> preorder(static_cast<std::size_t>(_n));
        for(int index = 0; index < _n; ++index){
            preorder[static_cast<std::size_t>(index)] = colors[
                static_cast<std::size_t>(tour->preorder_vertex(index))
            ];
        }
        distinct = std::make_unique<StaticRangeCountDistinct<Color, MAX_SIZE>>(preorder);
    }

    int count_at_lca_subtree(int left, int right) const{
        if(!distinct) throw std::runtime_error("library assertion fault: not built (count_at_lca_subtree).");
        const auto [range_left, range_right] = tour->subtree_range(lca_index.lca(left, right));
        return distinct->count(range_left, range_right);
    }
};
