// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <numeric>
#include <random>
#include <stdexcept>
#include <vector>

#include "../../src/approximate/routing/advanced_local_search.hpp"

namespace {

bool is_permutation(const std::vector<int>& values) {
    std::vector<int> sorted = values;
    std::sort(sorted.begin(), sorted.end());
    for(std::size_t index = 0; index < sorted.size(); ++index){
        if(sorted[index] != static_cast<int>(index)) return false;
    }
    return true;
}

template<class Exception, class Function>
bool throws(Function&& function) {
    try {
        function();
    }catch(const Exception&) {
        return true;
    }
    return false;
}

}  // namespace

int main() {
    std::uint64_t seed;
    int rounds;
    if(!(std::cin >> seed >> rounds)) return 0;
    std::mt19937_64 random(seed);

    for(int round = 0; round < rounds; ++round){
        const std::size_t n = 8 + random() % 25;
        std::vector<std::vector<long long>> distance(
            n, std::vector<long long>(n)
        );
        std::vector<std::pair<long long, long long>> point(n);
        for(auto& [x, y] : point){
            x = static_cast<long long>(random() % 1000);
            y = static_cast<long long>(random() % 1000);
        }
        for(std::size_t first = 0; first < n; ++first){
            for(std::size_t second = 0; second < n; ++second){
                const long long dx = point[first].first - point[second].first;
                const long long dy = point[first].second - point[second].second;
                distance[first][second] = dx * dx + dy * dy;
            }
        }
        std::vector<int> tour(n);
        std::iota(tour.begin(), tour.end(), 0);
        std::shuffle(tour.begin(), tour.end(), random);

        {
            auto moved = tour;
            approximate::routing::adjacent_swap(moved, 1);
            approximate::routing::pair_swap(moved, 1, 4);
            approximate::routing::block_swap(moved, 0, 2, 5, 7);
            assert(is_permutation(moved));

            assert(throws<std::out_of_range>([&] {
                approximate::routing::adjacent_swap(
                    moved, std::numeric_limits<std::size_t>::max()
                );
            }));
            assert(throws<std::invalid_argument>([&] {
                approximate::routing::pair_swap(moved, 1, 2);
            }));
        }
        {
            auto moved = tour;
            approximate::routing::apply_four_opt(moved, 1, 3, 5, 7);
            assert(is_permutation(moved));
            approximate::routing::apply_k_opt(
                moved, {0, 2, 5, n}, {2, 0, 1}, {1, 0, 1}
            );
            assert(is_permutation(moved));
        }
        {
            std::vector<std::vector<int>> routes{
                {0, 1, 2}, {3, 4, 5}, {6, 7}
            };
            approximate::routing::cross_exchange(
                routes[0], routes[1], 1, 3, 0, 1
            );
            approximate::routing::cyclic_exchange(routes, {0, 1, 0});
            approximate::routing::apply_ejection_chain(
                routes,
                {{0, 0, 1, 1}, {1, 0, 2, 1}}
            );
            std::vector<int> flattened;
            for(const auto& route : routes){
                flattened.insert(flattened.end(), route.begin(), route.end());
            }
            assert(is_permutation(flattened));

            assert(throws<std::invalid_argument>([&] {
                approximate::routing::cross_exchange(
                    routes[0], routes[0], 0, 1, 1, 2
                );
            }));
        }
        {
            approximate::routing::EjectionPool<std::vector<int>, long long>
                pool(2);
            pool.push({0}, 4);
            pool.push({1}, 2);
            pool.push({2}, 3);
            const auto sorted = pool.sorted();
            assert(sorted.size() == 2);
            assert(sorted[0].second == 2 && sorted[1].second == 3);
            assert(throws<std::invalid_argument>([] {
                approximate::routing::EjectionPool<int, int> invalid(0);
            }));
        }
        {
            std::vector<int> target(n);
            std::iota(target.begin(), target.end(), 0);
            const auto path =
                approximate::routing::path_relinking(tour, target);
            assert(path.front() == tour);
            assert(path.back() == target);
            for(const auto& state : path) assert(is_permutation(state));
            assert(
                approximate::routing::path_relinking(tour, target, 0).size()
                == 1
            );
        }
        const long long initial =
            approximate::routing::tour_cost(distance, tour);
        const auto lk = approximate::routing::lin_kernighan_tour(
            distance, tour, 8
        );
        const auto lkh =
            approximate::routing::lin_kernighan_helsgaun_tour(
                distance, tour, 6, 8
            );
        assert(is_permutation(lk));
        assert(is_permutation(lkh));
        assert(approximate::routing::tour_cost(distance, lk) <= initial);
        assert(approximate::routing::tour_cost(distance, lkh) <= initial);
    }
    std::cout << "OK\n";
}
