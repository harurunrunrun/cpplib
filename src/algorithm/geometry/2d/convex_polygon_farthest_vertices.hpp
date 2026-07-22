#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CONVEX_POLYGON_FARTHEST_VERTICES_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CONVEX_POLYGON_FARTHEST_VERTICES_HPP_INCLUDED

#include <cstddef>
#include <numeric>
#include <utility>
#include <vector>

#include "convex_calipers_common.hpp"

namespace rotating_calipers_detail{

template<class Evaluate>
inline void smawk_row_maxima(
    const std::vector<std::size_t>& rows,
    const std::vector<std::size_t>& columns,
    std::vector<std::size_t>& answer,
    const Evaluate& evaluate
){
    if(rows.empty()) return;

    std::vector<std::size_t> reduced;
    reduced.reserve(std::min(rows.size(), columns.size()));
    for(const std::size_t column: columns){
        while(!reduced.empty()){
            const std::size_t row = rows[reduced.size() - 1];
            if(scalar_compare(
                evaluate(row, column), evaluate(row, reduced.back())
            ) <= 0){
                break;
            }
            reduced.pop_back();
        }
        if(reduced.size() < rows.size()) reduced.push_back(column);
    }

    std::vector<std::size_t> odd_rows;
    odd_rows.reserve(rows.size() / 2);
    for(std::size_t index = 1; index < rows.size(); index += 2){
        odd_rows.push_back(rows[index]);
    }
    smawk_row_maxima(odd_rows, reduced, answer, evaluate);

    std::size_t left = 0;
    for(std::size_t row_index = 0; row_index < rows.size(); row_index += 2){
        if(row_index != 0){
            while(reduced[left] != answer[rows[row_index - 1]]) ++left;
        }
        std::size_t right = reduced.size() - 1;
        if(row_index + 1 < rows.size()){
            right = left;
            while(reduced[right] != answer[rows[row_index + 1]]) ++right;
        }

        std::size_t best = reduced[left];
        for(std::size_t column_index = left + 1;
            column_index <= right;
            ++column_index){
            const std::size_t candidate = reduced[column_index];
            if(scalar_compare(
                evaluate(rows[row_index], candidate),
                evaluate(rows[row_index], best)
            ) > 0){
                best = candidate;
            }
        }
        answer[rows[row_index]] = best;
    }
}

}  // namespace rotating_calipers_detail

inline std::vector<std::size_t> convex_polygon_farthest_vertices(
    const NormalizedConvexPolygon& polygon
){
    using namespace rotating_calipers_detail;

    const std::vector<Point>& vertices = polygon.vertices();
    const std::size_t count = vertices.size();
    if(count == 0) return {};
    if(count == 1) return {0};
    if(count == 2) return {1, 0};

    std::vector<std::size_t> rows(count);
    std::iota(rows.begin(), rows.end(), 0);
    std::vector<std::size_t> columns(2 * count - 1);
    std::iota(columns.begin(), columns.end(), 0);
    std::vector<std::size_t> maxima(count);

    const auto matrix_entry = [&](std::size_t row, std::size_t column){
        if(column <= row){
            return static_cast<long double>(column)
                - static_cast<long double>(row);
        }
        if(column >= row + count) return -1.0L;
        return squared_distance(vertices[row], vertices[column % count]);
    };
    smawk_row_maxima(rows, columns, maxima, matrix_entry);
    for(std::size_t& index: maxima) index %= count;
    return maxima;
}

inline std::vector<std::size_t> convex_polygon_farthest_vertices(
    std::vector<Point> polygon
){
    return convex_polygon_farthest_vertices(
        NormalizedConvexPolygon(std::move(polygon))
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CONVEX_POLYGON_FARTHEST_VERTICES_HPP_INCLUDED
