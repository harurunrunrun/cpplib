#ifndef CPPLIB_SRC_STRUCTURE_GRAPH_COORDINATE_PRODUCT_KNIGHT_DISTANCES_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_GRAPH_COORDINATE_PRODUCT_KNIGHT_DISTANCES_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <optional>
#include <stdexcept>
#include <vector>

#include "../../algorithm/graph/dijkstra.hpp"

class CoordinateProductKnightDistances{
    int board_size_;
    std::vector<std::vector<DijkstraEdge<long long>>> graph_;
    mutable std::vector<std::optional<DijkstraResult<long long>>> cache_;

    int vertex_id(int x, int y) const{
        if(x < 0 || board_size_ <= x || y < 0 || board_size_ <= y)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: coordinate violation "
                "(CoordinateProductKnightDistances)."
            );
        }
        return x * board_size_ + y;
    }

public:
    explicit CoordinateProductKnightDistances(int board_size):
        board_size_(board_size)
    {
        if(board_size <= 0 || 46340 < board_size)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: board size violation "
                "(CoordinateProductKnightDistances)."
            );
        }

        const int vertex_count = board_size * board_size;
        graph_.resize(static_cast<std::size_t>(vertex_count));
        cache_.resize(static_cast<std::size_t>(vertex_count));
        constexpr std::array<int, 8> dx{1, 1, -1, -1, 2, 2, -2, -2};
        constexpr std::array<int, 8> dy{2, -2, 2, -2, 1, -1, 1, -1};
        for(int x = 0; x < board_size; ++x){
            for(int y = 0; y < board_size; ++y){
                auto& edges = graph_[static_cast<std::size_t>(
                    x * board_size + y
                )];
                for(std::size_t direction = 0; direction < dx.size(); ++direction){
                    const int next_x = x + dx[direction];
                    const int next_y = y + dy[direction];
                    if(next_x < 0 || board_size <= next_x ||
                       next_y < 0 || board_size <= next_y){
                        continue;
                    }
                    edges.push_back({
                        next_x * board_size + next_y,
                        static_cast<long long>(x) * next_x +
                            static_cast<long long>(y) * next_y,
                    });
                }
            }
        }
    }

    int board_size() const{
        return board_size_;
    }

    std::optional<long long> distance(
        int start_x,
        int start_y,
        int target_x,
        int target_y
    ) const{
        const int source = vertex_id(start_x, start_y);
        const int target = vertex_id(target_x, target_y);
        auto& result = cache_[static_cast<std::size_t>(source)];
        if(!result) result = dijkstra<long long>(graph_, source);
        if(!result->reachable[static_cast<std::size_t>(target)]) return std::nullopt;
        return result->dist[static_cast<std::size_t>(target)];
    }

    void clear_cache(){
        for(auto& result: cache_) result.reset();
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_GRAPH_COORDINATE_PRODUCT_KNIGHT_DISTANCES_HPP_INCLUDED
