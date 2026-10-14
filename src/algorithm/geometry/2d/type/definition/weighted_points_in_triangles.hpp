#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TYPE_DEFINITION_WEIGHTED_POINTS_IN_TRIANGLES_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TYPE_DEFINITION_WEIGHTED_POINTS_IN_TRIANGLES_HPP_INCLUDED

#include <concepts>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <vector>
#include "count_points_in_triangles.hpp"

template<std::integral Coordinate>
class WeightedPointsInTriangles{
    using WeightedPoint = std::tuple<Coordinate, Coordinate, std::int64_t>;
    CountPointsInTriangles<Coordinate, __int128> index;

    static std::vector<std::pair<Coordinate, Coordinate>> coordinates(
        const std::vector<WeightedPoint>& points
    ){
        std::vector<std::pair<Coordinate, Coordinate>> result;
        result.reserve(points.size());
        for(const auto& [x, y, weight]: points) result.emplace_back(x, y);
        return result;
    }

    static std::vector<__int128> weights(const std::vector<WeightedPoint>& points){
        std::vector<__int128> result;
        result.reserve(points.size());
        for(const auto& point: points) result.push_back(std::get<2>(point));
        return result;
    }

public:
    WeightedPointsInTriangles(
        const std::vector<std::pair<Coordinate, Coordinate>>& vertices,
        const std::vector<WeightedPoint>& points
    ) : index(vertices, coordinates(points), weights(points)){}

    std::size_t vertex_count() const{return index.vertex_count();}

    std::int64_t weighted_sum(std::size_t a, std::size_t b, std::size_t c) const{
        const __int128 answer = index.count(a, b, c);
        if(answer < std::numeric_limits<std::int64_t>::min()
            || answer > std::numeric_limits<std::int64_t>::max()){
            throw std::overflow_error("weighted triangle sum");
        }
        return static_cast<std::int64_t>(answer);
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TYPE_DEFINITION_WEIGHTED_POINTS_IN_TRIANGLES_HPP_INCLUDED
