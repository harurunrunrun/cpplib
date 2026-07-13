#define PROBLEM "https://www.spoj.com/problems/PT07J/"

#include <iostream>
#include <memory>
#include <utility>
#include <vector>

#include "../../src/algorithm/tree/euler_tour.hpp"
#include "../../src/structure/wavelet_matrix/compressed_wavelet_matrix.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int vertex_count;
    std::cin >> vertex_count;
    std::vector<int> labels(static_cast<std::size_t>(vertex_count));
    for(int& label: labels){
        std::cin >> label;
    }

    auto tour = std::make_unique<TreeEulerTour<100000>>(vertex_count);
    for(int edge = 1; edge < vertex_count; ++edge){
        int left, right;
        std::cin >> left >> right;
        tour->add_edge(left - 1, right - 1);
    }
    tour->build();

    std::vector<std::pair<int, int>> preorder_labels;
    preorder_labels.reserve(static_cast<std::size_t>(vertex_count));
    for(int index = 0; index < vertex_count; ++index){
        const int vertex = tour->preorder_vertex(index);
        preorder_labels.emplace_back(
            labels[static_cast<std::size_t>(vertex)],
            vertex
        );
    }
    CompressedWaveletMatrix<std::pair<int, int>, 100000> matrix(preorder_labels);

    int query_count;
    std::cin >> query_count;
    while(query_count-- != 0){
        int root, order;
        std::cin >> root >> order;
        --root;
        const auto [left, right] = tour->subtree_range(root);
        // The SPOJ sample and judge use increasing-label order.
        // This differs from the k-th largest wording in the statement.
        std::cout << matrix.kth_smallest(left, right, order - 1).second + 1 << '\n';
    }
}
