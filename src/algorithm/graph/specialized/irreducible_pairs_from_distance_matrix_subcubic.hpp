#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_SPECIALIZED_IRREDUCIBLE_PAIRS_FROM_DISTANCE_MATRIX_SUBCUBIC_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_SPECIALIZED_IRREDUCIBLE_PAIRS_FROM_DISTANCE_MATRIX_SUBCUBIC_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <utility>
#include <vector>

namespace irreducible_pairs_distance_matrix_subcubic_detail{

inline std::size_t checked_square_area(std::size_t side){
    if(side != 0
        && side > std::numeric_limits<std::size_t>::max() / side)[[unlikely]]{
        throw std::length_error(
            "irreducible_pairs_from_distance_matrix_subcubic "
            "matrix is too large"
        );
    }
    return side * side;
}

inline int dominance_block_width(std::size_t vertex_count){
    int floor_logarithm = 0;
    for(std::size_t value = vertex_count; value > 1; value >>= 1){
        floor_logarithm++;
    }
    return std::max(1, floor_logarithm / 4);
}

class BlockMinimizers{
    struct TaggedPoint{
        int vertex;
        __int128_t coordinate;
        bool red;
    };

    static bool point_less(
        const TaggedPoint& left, const TaggedPoint& right
    ){
        if(left.coordinate != right.coordinate){
            return left.coordinate < right.coordinate;
        }
        if(left.red != right.red) return left.red;
        return left.vertex < right.vertex;
    }

    static void insertion_sort(
        std::vector<TaggedPoint>& points,
        std::size_t first,
        std::size_t last
    ){
        for(std::size_t index = first + 1; index < last; index++){
            TaggedPoint value = points[index];
            std::size_t position = index;
            while(first < position
                    && point_less(value, points[position - 1])){
                points[position] = points[position - 1];
                position--;
            }
            points[position] = value;
        }
    }

    static TaggedPoint select_kth(
        std::vector<TaggedPoint>& points,
        std::size_t first,
        std::size_t last,
        std::size_t selected
    ){
        while(true){
            const std::size_t size = last - first;
            if(size <= 5){
                insertion_sort(points, first, last);
                return points[selected];
            }

            const std::size_t group_count = (size + 4) / 5;
            for(std::size_t group = 0; group < group_count; group++){
                const std::size_t group_first = first + 5 * group;
                const std::size_t group_last =
                    std::min(last, group_first + 5);
                insertion_sort(points, group_first, group_last);
                const std::size_t median =
                    group_first + (group_last - group_first) / 2;
                std::swap(points[first + group], points[median]);
            }
            const TaggedPoint pivot = select_kth(
                points,
                first,
                first + group_count,
                first + group_count / 2
            );

            std::size_t lower = first;
            std::size_t current = first;
            std::size_t upper = last;
            while(current < upper){
                if(point_less(points[current], pivot)){
                    std::swap(points[lower], points[current]);
                    lower++;
                    current++;
                }else if(point_less(pivot, points[current])){
                    upper--;
                    std::swap(points[current], points[upper]);
                }else{
                    current++;
                }
            }
            if(selected < lower){
                last = lower;
            }else if(upper <= selected){
                first = upper;
            }else{
                return points[selected];
            }
        }
    }

    const std::vector<std::vector<long long>>& distances_;
    int vertex_count_;
    int block_begin_;
    int block_end_;
    std::size_t area_;
    int candidate_ = -1;
    std::vector<int> dimensions_;
    std::vector<int> minimizer_;

    __int128_t red_coordinate(int row, int competitor) const{
        const __int128_t difference =
            static_cast<__int128_t>(
                distances_[static_cast<std::size_t>(row)]
                          [static_cast<std::size_t>(candidate_)]
            ) -
            static_cast<__int128_t>(
                distances_[static_cast<std::size_t>(row)]
                          [static_cast<std::size_t>(competitor)]
            );
        return 2 * difference + (candidate_ > competitor ? 1 : 0);
    }

    __int128_t blue_coordinate(int column, int competitor) const{
        const __int128_t difference =
            static_cast<__int128_t>(
                distances_[static_cast<std::size_t>(competitor)]
                          [static_cast<std::size_t>(column)]
            ) -
            static_cast<__int128_t>(
                distances_[static_cast<std::size_t>(candidate_)]
                          [static_cast<std::size_t>(column)]
            );
        return 2 * difference;
    }

    void record_all(
        const std::vector<int>& red_vertices,
        const std::vector<int>& blue_vertices
    ){
        for(const int row: red_vertices){
            for(const int column: blue_vertices){
                int& slot = minimizer_[
                    static_cast<std::size_t>(row)
                        * static_cast<std::size_t>(vertex_count_)
                    + static_cast<std::size_t>(column)
                ];
                if(slot != -1)[[unlikely]]{
                    throw std::logic_error(
                        "subcubic distance-matrix irreducible-pair dominance "
                        "reported two minimizers"
                    );
                }
                slot = candidate_;
            }
        }
    }

    void report_dominating_pairs(
        const std::vector<int>& red_vertices,
        const std::vector<int>& blue_vertices,
        std::size_t dimension_count
    ){
        if(red_vertices.empty() || blue_vertices.empty()) return;
        if(dimension_count == 0){
            record_all(red_vertices, blue_vertices);
            return;
        }

        const int competitor = dimensions_[dimension_count - 1];
        std::vector<TaggedPoint> points;
        points.reserve(red_vertices.size() + blue_vertices.size());
        for(const int vertex: red_vertices){
            points.push_back({
                vertex, red_coordinate(vertex, competitor), true
            });
        }
        for(const int vertex: blue_vertices){
            points.push_back({
                vertex, blue_coordinate(vertex, competitor), false
            });
        }
        const std::size_t middle = points.size() / 2;
        select_kth(points, 0, points.size(), middle);
        std::vector<int> left_red;
        std::vector<int> left_blue;
        std::vector<int> right_red;
        std::vector<int> right_blue;
        left_red.reserve(middle);
        left_blue.reserve(middle);
        right_red.reserve(points.size() - middle);
        right_blue.reserve(points.size() - middle);
        for(std::size_t index = 0; index < points.size(); index++){
            const TaggedPoint& point = points[index];
            std::vector<int>* destination = nullptr;
            if(index < middle){
                destination = point.red ? &left_red : &left_blue;
            }else{
                destination = point.red ? &right_red : &right_blue;
            }
            destination->push_back(point.vertex);
        }

        report_dominating_pairs(left_red, left_blue, dimension_count);
        report_dominating_pairs(right_red, right_blue, dimension_count);
        report_dominating_pairs(left_red, right_blue, dimension_count - 1);
    }

public:
    BlockMinimizers(
        const std::vector<std::vector<long long>>& distances,
        int block_begin,
        int block_end,
        std::size_t area
    )
        : distances_(distances),
          vertex_count_(static_cast<int>(distances.size())),
          block_begin_(block_begin),
          block_end_(block_end),
          area_(area),
          minimizer_(area, -1){}

    std::vector<int> compute(){
        std::vector<int> vertices(
            static_cast<std::size_t>(vertex_count_)
        );
        std::iota(vertices.begin(), vertices.end(), 0);

        for(candidate_ = block_begin_; candidate_ < block_end_; candidate_++){
            dimensions_.clear();
            for(int competitor = block_begin_;
                    competitor < block_end_; competitor++){
                if(competitor != candidate_){
                    dimensions_.push_back(competitor);
                }
            }
            report_dominating_pairs(
                vertices, vertices, dimensions_.size()
            );
        }

        if(static_cast<std::size_t>(vertex_count_) != 0
            && minimizer_.size() != area_)[[unlikely]]{
            throw std::logic_error(
                "subcubic distance-matrix irreducible-pair minimizer area changed"
            );
        }
        for(const int minimizer: minimizer_){
            if(minimizer == -1)[[unlikely]]{
                throw std::logic_error(
                    "subcubic distance-matrix irreducible-pair minimizer is missing"
                );
            }
        }
        return std::move(minimizer_);
    }
};

}  // namespace irreducible_pairs_distance_matrix_subcubic_detail

inline std::vector<std::pair<int, int>>
irreducible_pairs_from_distance_matrix_subcubic(
    const std::vector<std::vector<long long>>& distances
){
    using namespace irreducible_pairs_distance_matrix_subcubic_detail;

    if(distances.size()
        > static_cast<std::size_t>(std::numeric_limits<int>::max()))
        [[unlikely]]{
        throw std::length_error(
            "irreducible_pairs_from_distance_matrix_subcubic "
            "matrix is too large"
        );
    }
    const std::size_t vertex_count = distances.size();
    const std::size_t area = checked_square_area(vertex_count);
    for(const auto& row: distances){
        if(row.size() != vertex_count)[[unlikely]]{
            throw std::invalid_argument(
                "irreducible_pairs_from_distance_matrix_subcubic "
                "requires a square matrix"
            );
        }
    }
    for(std::size_t vertex = 0; vertex < vertex_count; vertex++){
        if(distances[vertex][vertex] != 0)[[unlikely]]{
            throw std::invalid_argument(
                "irreducible_pairs_from_distance_matrix_subcubic "
                "requires a zero diagonal"
            );
        }
        for(std::size_t other = 0; other < vertex_count; other++){
            if(distances[vertex][other] < 0)[[unlikely]]{
                throw std::invalid_argument(
                    "irreducible_pairs_from_distance_matrix_subcubic "
                    "requires nonnegative distances"
                );
            }
            if(distances[vertex][other] != distances[other][vertex])
                [[unlikely]]{
                throw std::invalid_argument(
                    "irreducible_pairs_from_distance_matrix_subcubic "
                    "requires a symmetric matrix"
                );
            }
        }
    }
    if(vertex_count < 2) return {};

    std::vector<unsigned char> reducible(area, 0);
    const int width = dominance_block_width(vertex_count);
    const int count = static_cast<int>(vertex_count);
    for(int block_begin = 0; block_begin < count; block_begin += width){
        const int block_end = std::min(count, block_begin + width);
        BlockMinimizers minimizers(
            distances, block_begin, block_end, area
        );
        const std::vector<int> minimum_index = minimizers.compute();

        for(int left = 0; left < count; left++){
            for(int right = left + 1; right < count; right++){
                const std::size_t pair_index =
                    static_cast<std::size_t>(left) * vertex_count
                    + static_cast<std::size_t>(right);
                const __int128_t direct = static_cast<__int128_t>(
                    distances[static_cast<std::size_t>(left)]
                             [static_cast<std::size_t>(right)]
                );

                const bool endpoint_in_block =
                    (block_begin <= left && left < block_end)
                    || (block_begin <= right && right < block_end);
                if(endpoint_in_block){
                    for(int middle = block_begin;
                            middle < block_end; middle++){
                        if(middle == left || middle == right) continue;
                        const __int128_t through =
                            static_cast<__int128_t>(
                                distances[static_cast<std::size_t>(left)]
                                         [static_cast<std::size_t>(middle)]
                            ) +
                            static_cast<__int128_t>(
                                distances[static_cast<std::size_t>(middle)]
                                         [static_cast<std::size_t>(right)]
                            );
                        if(through < direct)[[unlikely]]{
                            throw std::invalid_argument(
                                "irreducible_pairs_from_distance_matrix_"
                                "subcubic triangle inequality violation"
                            );
                        }
                        if(through == direct){
                            reducible[pair_index] = 1;
                        }
                    }
                    continue;
                }

                const int middle = minimum_index[pair_index];
                const __int128_t through =
                    static_cast<__int128_t>(
                        distances[static_cast<std::size_t>(left)]
                                 [static_cast<std::size_t>(middle)]
                    ) +
                    static_cast<__int128_t>(
                        distances[static_cast<std::size_t>(middle)]
                                 [static_cast<std::size_t>(right)]
                    );
                if(through < direct)[[unlikely]]{
                    throw std::invalid_argument(
                        "irreducible_pairs_from_distance_matrix_subcubic "
                        "triangle inequality violation"
                    );
                }
                if(through == direct) reducible[pair_index] = 1;
            }
        }
    }

    std::vector<std::pair<int, int>> result;
    for(int left = 0; left < count; left++){
        for(int right = left + 1; right < count; right++){
            const std::size_t pair_index =
                static_cast<std::size_t>(left) * vertex_count
                + static_cast<std::size_t>(right);
            if(reducible[pair_index] == 0){
                result.emplace_back(left, right);
            }
        }
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_SPECIALIZED_IRREDUCIBLE_PAIRS_FROM_DISTANCE_MATRIX_SUBCUBIC_HPP_INCLUDED
