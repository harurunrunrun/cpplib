// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>

#include "../../src/approximate/routing/local_moves.hpp"
#include "../../src/approximate/routing/tour_cost.hpp"
#include "../../src/approximate/routing/distance_matrix.hpp"

namespace {

std::vector<int> read_route(std::size_t size){
    std::vector<int> route(size);
    for(int& vertex : route) std::cin >> vertex;
    return route;
}

std::vector<std::vector<long long>> read_matrix(std::size_t size){
    std::vector<std::vector<long long>> matrix(
        size,
        std::vector<long long>(size)
    );
    for(auto& row : matrix){
        for(long long& value : row) std::cin >> value;
    }
    return matrix;
}

void print_route(const std::vector<int>& route){
    std::cout << route.size();
    for(const int vertex : route) std::cout << ' ' << vertex;
}

}  // namespace

int main(){
    using namespace approximate::routing;
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    const std::vector<std::vector<int>> valid_distance{
        {0, 3}, {3, 0}
    };
    assert(internal::validate_distance_matrix(valid_distance) == 2);
    internal::validate_tour_vertices({0, 1}, 2);
    bool rejected = false;
    try{
        static_cast<void>(internal::validate_distance_matrix(
            std::vector<std::vector<int>>{{0, 1}, {1}}
        ));
    }catch(const std::invalid_argument&){
        rejected = true;
    }
    assert(rejected);
    rejected = false;
    try{
        internal::validate_tour_vertices({0, -1}, 2);
    }catch(const std::out_of_range&){
        rejected = true;
    }
    assert(rejected);

    int query_count = 0;
    std::cin >> query_count;
    while(query_count-- > 0){
        std::string command;
        std::cin >> command;
        if(command == "COST" || command == "PATH"){
            std::size_t matrix_size = 0;
            std::size_t route_size = 0;
            std::cin >> matrix_size >> route_size;
            const auto matrix = read_matrix(matrix_size);
            const auto route = read_route(route_size);
            std::cout << (
                command == "COST"
                ? tour_cost(matrix, route)
                : path_cost(matrix, route)
            ) << '\n';
        }else if(command == "TWO_DELTA"){
            std::size_t size = 0;
            std::size_t first = 0;
            std::size_t last = 0;
            std::cin >> size >> first >> last;
            const auto matrix = read_matrix(size);
            const auto route = read_route(size);
            std::cout << two_opt_symmetric_delta(
                matrix,
                route,
                first,
                last
            ) << '\n';
        }else if(command == "TWO_STAR_DELTA"){
            std::size_t matrix_size = 0;
            std::size_t first_size = 0;
            std::size_t second_size = 0;
            std::size_t first_cut = 0;
            std::size_t second_cut = 0;
            std::cin >> matrix_size >> first_size >> second_size;
            std::cin >> first_cut >> second_cut;
            const auto matrix = read_matrix(matrix_size);
            const auto first = read_route(first_size);
            const auto second = read_route(second_size);
            std::cout << two_opt_star_delta(
                matrix,
                first,
                second,
                first_cut,
                second_cut
            ) << '\n';
        }else if(command == "TWO_STAR"){
            std::size_t first_size = 0;
            std::size_t second_size = 0;
            std::size_t first_cut = 0;
            std::size_t second_cut = 0;
            std::cin >> first_size >> second_size >> first_cut >> second_cut;
            auto first = read_route(first_size);
            auto second = read_route(second_size);
            apply_two_opt_star(first, second, first_cut, second_cut);
            print_route(first);
            std::cout << " | ";
            print_route(second);
            std::cout << '\n';
        }else{
            std::size_t size = 0;
            std::cin >> size;
            auto route = read_route(size);
            if(command == "TWO" || command == "REVERSE"){
                std::size_t first = 0;
                std::size_t last = 0;
                std::cin >> first >> last;
                if(command == "TWO") apply_two_opt(route, first, last);
                else reverse_segment(route, first, last);
            }else if(command == "THREE"){
                std::size_t first = 0;
                std::size_t second = 0;
                std::size_t third = 0;
                int pattern = 0;
                std::cin >> first >> second >> third >> pattern;
                apply_three_opt(
                    route,
                    first,
                    second,
                    third,
                    static_cast<ThreeOptPattern>(pattern)
                );
            }else if(command == "RELOCATE"){
                std::size_t first = 0;
                std::size_t last = 0;
                std::size_t destination = 0;
                std::cin >> first >> last >> destination;
                relocate_segment(route, first, last, destination);
            }else if(command == "NODE"){
                std::size_t position = 0;
                std::size_t destination = 0;
                std::cin >> position >> destination;
                relocate_vertex(route, position, destination);
            }else if(command == "OR"){
                std::size_t first = 0;
                std::size_t length = 0;
                std::size_t destination = 0;
                std::cin >> first >> length >> destination;
                apply_or_opt(route, first, length, destination);
            }else if(command == "SWAP"){
                std::size_t first = 0;
                std::size_t second = 0;
                std::cin >> first >> second;
                swap_vertices(route, first, second);
            }else if(command == "EXCHANGE"){
                std::size_t first_begin = 0;
                std::size_t first_end = 0;
                std::size_t second_begin = 0;
                std::size_t second_end = 0;
                std::cin >> first_begin >> first_end >> second_begin >> second_end;
                exchange_segments(
                    route,
                    first_begin,
                    first_end,
                    second_begin,
                    second_end
                );
            }
            print_route(route);
            std::cout << '\n';
        }
    }
}
