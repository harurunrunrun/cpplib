#ifndef CPPLIB_SRC_ALGORITHM_TREE_QUERY_SUBTREE_LABEL_ORDER_STATISTICS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_TREE_QUERY_SUBTREE_LABEL_ORDER_STATISTICS_HPP_INCLUDED

#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../decomposition/euler_tour.hpp"
#include "../../../structure/wavelet_matrix/compressed_wavelet_matrix.hpp"

template<int MAX_SIZE>
struct SubtreeLabelOrderStatistics{
private:
    int _n;
    std::vector<int> labels;
    std::unique_ptr<TreeEulerTour<MAX_SIZE>> tour;
    std::unique_ptr<CompressedWaveletMatrix<std::pair<int, int>, MAX_SIZE>> matrix;

public:
    explicit SubtreeLabelOrderStatistics(const std::vector<int>& vertex_labels):
        _n(static_cast<int>(vertex_labels.size())),
        labels(vertex_labels),
        tour(std::make_unique<TreeEulerTour<MAX_SIZE>>(_n)){}

    int size() const{ return _n; }

    void add_edge(int left, int right){ tour->add_edge(left, right); }

    void build(int root = 0){
        tour->build(root);
        std::vector<std::pair<int, int>> preorder_labels;
        preorder_labels.reserve(static_cast<std::size_t>(_n));
        for(int index = 0; index < _n; ++index){
            const int vertex = tour->preorder_vertex(index);
            preorder_labels.emplace_back(labels[static_cast<std::size_t>(vertex)], vertex);
        }
        matrix = std::make_unique<CompressedWaveletMatrix<std::pair<int, int>, MAX_SIZE>>(
            preorder_labels
        );
    }

    int kth_smallest_vertex(int subtree_root, int order) const{
        if(!matrix) throw std::runtime_error("library assertion fault: not built (kth_smallest_vertex).");
        const auto [left, right] = tour->subtree_range(subtree_root);
        if(order < 0 || right - left <= order){
            throw std::runtime_error("library assertion fault: range violation (kth_smallest_vertex).");
        }
        return matrix->kth_smallest(left, right, order).second;
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_TREE_QUERY_SUBTREE_LABEL_ORDER_STATISTICS_HPP_INCLUDED
