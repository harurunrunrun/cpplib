#define PROBLEM "https://br.spoj.com/SPOJ/problems/BGSHOOT/"

#include <algorithm>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

#include "../../src/structure/other/sparse_table.hpp"
#include "../../src/structure/types/common_monoids.hpp"

namespace{

constexpr MaxMonoid<int> maximum{};

} // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int animal_count;
    std::cin >> animal_count;
    std::vector<std::pair<int, int>> intervals(
        static_cast<std::size_t>(animal_count)
    );
    std::vector<int> coordinates;
    coordinates.reserve(static_cast<std::size_t>(2 * animal_count));
    for(auto& [left, right]: intervals){
        std::cin >> left >> right;
        if(right < left) std::swap(left, right);
        coordinates.push_back(left);
        coordinates.push_back(right);
    }

    int query_count;
    std::cin >> query_count;
    std::vector<std::pair<int, int>> queries(
        static_cast<std::size_t>(query_count)
    );
    coordinates.reserve(coordinates.size() + static_cast<std::size_t>(2 * query_count));
    for(auto& [left, right]: queries){
        std::cin >> left >> right;
        if(right < left) std::swap(left, right);
        coordinates.push_back(left);
        coordinates.push_back(right);
    }

    std::sort(coordinates.begin(), coordinates.end());
    coordinates.erase(
        std::unique(coordinates.begin(), coordinates.end()),
        coordinates.end()
    );
    std::vector<int> difference(coordinates.size() + 1, 0);
    for(auto [left, right]: intervals){
        const auto left_index = std::lower_bound(
            coordinates.begin(), coordinates.end(), left
        ) - coordinates.begin();
        const auto right_index = std::lower_bound(
            coordinates.begin(), coordinates.end(), right
        ) - coordinates.begin();
        difference[static_cast<std::size_t>(left_index)]++;
        difference[static_cast<std::size_t>(right_index + 1)]--;
    }

    std::vector<int> coverage(coordinates.size());
    int current = 0;
    for(std::size_t index = 0; index < coordinates.size(); ++index){
        current += difference[index];
        coverage[index] = current;
    }
    auto table = std::make_unique<
        SparseTable<maximum, 400000>
    >(coverage);
    for(auto [left, right]: queries){
        const int left_index = static_cast<int>(
            std::lower_bound(coordinates.begin(), coordinates.end(), left)
            - coordinates.begin()
        );
        const int right_index = static_cast<int>(
            std::upper_bound(coordinates.begin(), coordinates.end(), right)
            - coordinates.begin()
        );
        std::cout << table->prod(left_index, right_index) << '\n';
    }
}
