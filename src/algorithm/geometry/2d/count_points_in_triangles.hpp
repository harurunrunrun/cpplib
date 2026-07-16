#pragma once

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

template<std::integral Coordinate>
class CountPointsInTriangles{
    struct Point{
        Coordinate x;
        Coordinate y;
    };

    struct FenwickTree{
        std::vector<int> values;

        explicit FenwickTree(std::size_t size) : values(size + 1, 0){}

        void add(std::size_t index){
            for(++index; index < values.size(); index += index & -index){
                ++values[index];
            }
        }

        int prefix_sum(std::size_t end) const{
            int result = 0;
            for(; end > 0; end -= end & -end) result += values[end];
            return result;
        }
    };

    struct SweepEvent{
        Point point{};
        std::size_t vertex = 0;
        std::size_t rank = 0;
        bool is_vertex = false;
    };

    static bool less_yx(const Point& first, const Point& second){
        if(first.y != second.y) return first.y < second.y;
        return first.x < second.x;
    }

    static __int128_t cross(
        const Point& first,
        const Point& second,
        const Point& third
    ){
        const __int128_t first_x = static_cast<__int128_t>(first.x)
            - static_cast<__int128_t>(third.x);
        const __int128_t first_y = static_cast<__int128_t>(first.y)
            - static_cast<__int128_t>(third.y);
        const __int128_t second_x = static_cast<__int128_t>(second.x)
            - static_cast<__int128_t>(third.x);
        const __int128_t second_y = static_cast<__int128_t>(second.y)
            - static_cast<__int128_t>(third.y);
        return first_x * second_y - first_y * second_x;
    }

public:
    CountPointsInTriangles(
        const std::vector<std::pair<Coordinate, Coordinate>>& vertices,
        const std::vector<std::pair<Coordinate, Coordinate>>& query_points
    ){
        if(query_points.size() > static_cast<std::size_t>(
            std::numeric_limits<int>::max()
        ))[[unlikely]]{
            throw std::length_error(
                "CountPointsInTriangles has too many query points");
        }
        vertices_.reserve(vertices.size());
        for(const auto& [x, y]: vertices) vertices_.push_back({x, y});
        std::vector<Point> points;
        points.reserve(query_points.size());
        for(const auto& [x, y]: query_points) points.push_back({x, y});

        const std::size_t size = vertices_.size();
        point_left_.assign(size, 0);
        point_on_.assign(size, 0);
        edge_left_.assign(size, std::vector<int>(size, 0));
        edge_on_.assign(size, std::vector<int>(size, 0));

        std::sort(points.begin(), points.end(), less_yx);
        for(std::size_t vertex = 0; vertex < size; ++vertex){
            const Point& value = vertices_[vertex];
            const auto y_begin = std::lower_bound(
                points.begin(), points.end(), value.y,
                [](const Point& point, Coordinate y){ return point.y < y; }
            );
            const auto y_end = std::upper_bound(
                y_begin, points.end(), value.y,
                [](Coordinate y, const Point& point){ return y < point.y; }
            );
            const auto x_begin = std::lower_bound(
                y_begin, y_end, value.x,
                [](const Point& point, Coordinate x){ return point.x < x; }
            );
            const auto x_end = std::upper_bound(
                x_begin, y_end, value.x,
                [](Coordinate x, const Point& point){ return x < point.x; }
            );
            point_left_[vertex] = static_cast<int>(x_begin - y_begin);
            point_on_[vertex] = static_cast<int>(x_end - x_begin);
        }

        for(std::size_t first = 0; first < size; ++first){
            std::vector<SweepEvent> events;
            events.reserve(size + points.size());
            for(const Point& point: points){
                if(point.y > vertices_[first].y){
                    events.push_back({point, 0, 0, false});
                }
            }
            for(std::size_t second = 0; second < size; ++second){
                if(vertices_[second].y > vertices_[first].y){
                    events.push_back({
                        vertices_[second], second, 0, true
                    });
                }
            }
            if(events.empty()) continue;

            std::vector<std::size_t> slope_order(events.size());
            for(std::size_t index = 0; index < events.size(); ++index){
                slope_order[index] = index;
            }
            std::sort(
                slope_order.begin(), slope_order.end(),
                [&](std::size_t left, std::size_t right){
                    const __int128_t determinant = cross(
                        vertices_[first],
                        events[left].point,
                        events[right].point
                    );
                    if(determinant != 0) return determinant < 0;
                    return left < right;
                }
            );
            std::size_t rank = 0;
            events[slope_order.front()].rank = rank;
            for(std::size_t index = 1; index < slope_order.size(); ++index){
                if(cross(
                    vertices_[first],
                    events[slope_order[index - 1]].point,
                    events[slope_order[index]].point
                ) != 0){
                    ++rank;
                }
                events[slope_order[index]].rank = rank;
            }

            std::sort(
                events.begin(), events.end(),
                [](const SweepEvent& left, const SweepEvent& right){
                    if(left.point.y != right.point.y){
                        return left.point.y < right.point.y;
                    }
                    if(left.is_vertex != right.is_vertex){
                        return left.is_vertex;
                    }
                    return left.rank < right.rank;
                }
            );
            FenwickTree tree(rank + 1);
            for(std::size_t begin = 0; begin < events.size();){
                std::size_t end = begin + 1;
                while(end < events.size()
                    && events[end].point.y == events[begin].point.y){
                    ++end;
                }
                for(std::size_t index = begin; index < end; ++index){
                    if(!events[index].is_vertex) continue;
                    const int less = tree.prefix_sum(events[index].rank);
                    const int at_most = tree.prefix_sum(
                        events[index].rank + 1
                    );
                    edge_left_[first][events[index].vertex] = less;
                    edge_on_[first][events[index].vertex] = at_most - less;
                }
                for(std::size_t index = begin; index < end; ++index){
                    if(!events[index].is_vertex){
                        tree.add(events[index].rank);
                    }
                }
                begin = end;
            }
        }
    }

    int count(std::size_t first, std::size_t second, std::size_t third) const{
        if(first >= vertices_.size() || second >= vertices_.size()
            || third >= vertices_.size())[[unlikely]]{
            throw std::out_of_range("CountPointsInTriangles::count");
        }
        if(less_yx(vertices_[second], vertices_[first])){
            std::swap(first, second);
        }
        if(less_yx(vertices_[third], vertices_[second])){
            std::swap(second, third);
        }
        if(less_yx(vertices_[second], vertices_[first])){
            std::swap(first, second);
        }

        const __int128_t determinant = cross(
            vertices_[first], vertices_[second], vertices_[third]
        );
        if(determinant == 0) return 0;

        int result = 0;
        if(vertices_[first].y == vertices_[second].y){
            result = edge_left_[second][third]
                - edge_left_[first][third] - edge_on_[first][third];
        }else if(vertices_[second].y == vertices_[third].y){
            result = edge_left_[first][third]
                - edge_left_[first][second] - edge_on_[first][second];
        }else if(determinant < 0){
            result = edge_left_[first][third]
                - edge_left_[second][third] - edge_on_[second][third]
                - edge_left_[first][second] - edge_on_[first][second]
                - point_left_[second] - point_on_[second];
        }else{
            result = edge_left_[first][second] + edge_left_[second][third]
                + point_left_[second]
                - edge_left_[first][third] - edge_on_[first][third];
        }
        return result;
    }

    std::size_t vertex_count() const{
        return vertices_.size();
    }

private:
    std::vector<Point> vertices_;
    std::vector<int> point_left_;
    std::vector<int> point_on_;
    std::vector<std::vector<int>> edge_left_;
    std::vector<std::vector<int>> edge_on_;
};
