#pragma once

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

template<std::integral Coordinate>
class CountPointsInTriangles{
    struct Point{
        Coordinate x;
        Coordinate y;
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

        for(std::size_t vertex = 0; vertex < size; ++vertex){
            for(const Point& point: points){
                if(vertices_[vertex].y != point.y) continue;
                if(point.x < vertices_[vertex].x) ++point_left_[vertex];
                if(point.x == vertices_[vertex].x) ++point_on_[vertex];
            }
        }
        for(std::size_t first = 0; first < size; ++first){
            for(std::size_t second = 0; second < size; ++second){
                if(vertices_[first].y >= vertices_[second].y) continue;
                for(const Point& point: points){
                    if(point.y <= vertices_[first].y
                        || vertices_[second].y <= point.y) continue;
                    const __int128_t determinant = cross(
                        vertices_[first], vertices_[second], point
                    );
                    if(determinant > 0) ++edge_left_[first][second];
                    if(determinant == 0) ++edge_on_[first][second];
                }
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
