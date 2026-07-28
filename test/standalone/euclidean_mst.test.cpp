// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <numeric>
#include <utility>
#include <vector>

#include "../../src/algorithm/geometry/2d/type/definition/euclidean_mst_edge.hpp"
#include "../../src/algorithm/geometry/2d/type/definition/euclidean_mst_result.hpp"
#include "../../src/algorithm/geometry/2d/result/point_set/euclidean_mst_points.hpp"
#include "../../src/algorithm/geometry/2d/aggregate/all/euclidean_mst.hpp"

namespace{

void print_uint128(__uint128_t value){
    if(value == 0){
        std::cout << '0';
        return;
    }
    char buffer[40];
    int size = 0;
    while(value != 0){
        buffer[size++] = static_cast<char>('0' + value % 10);
        value /= 10;
    }
    while(size-- > 0) std::cout << buffer[size];
}

class Dsu{
public:
    explicit Dsu(std::size_t size): parent_(size){
        std::iota(parent_.begin(), parent_.end(), std::size_t{0});
    }

    std::size_t leader(std::size_t vertex){
        if(parent_[vertex] == vertex) return vertex;
        return parent_[vertex] = leader(parent_[vertex]);
    }

    bool merge(std::size_t first, std::size_t second){
        first = leader(first);
        second = leader(second);
        if(first == second) return false;
        parent_[second] = first;
        return true;
    }

private:
    std::vector<std::size_t> parent_;
};

}  // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    const EuclideanMstEdge sample_edge{
        0, 1, static_cast<__uint128_t>(25)
    };
    assert(sample_edge.from == 0 && sample_edge.to == 1
        && sample_edge.squared_distance == 25);
    const EuclideanMstResult default_result;
    assert(default_result.edges.empty());

    int test_count;
    std::cin >> test_count;
    while(test_count-- > 0){
        std::size_t size;
        std::cin >> size;
        std::vector<std::pair<long long, long long>> points(size);
        for(auto& [x, y]: points) std::cin >> x >> y;
        const EuclideanMstResult result = euclidean_mst(points);
        assert(result.edges.size() == (size == 0 ? 0 : size - 1));
        Dsu dsu(size);
        std::vector<__uint128_t> weights;
        weights.reserve(result.edges.size());
        for(const EuclideanMstEdge& edge: result.edges){
            assert(edge.from < size && edge.to < size);
            assert(dsu.merge(edge.from, edge.to));
            weights.push_back(edge.squared_distance);
        }
        std::sort(weights.begin(), weights.end());
        std::cout << weights.size();
        for(__uint128_t weight: weights){
            std::cout << ' ';
            print_uint128(weight);
        }
        std::cout << '\n';
    }
}
