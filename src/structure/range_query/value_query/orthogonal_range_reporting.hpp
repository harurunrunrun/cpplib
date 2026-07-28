#ifndef CPPLIB_SRC_STRUCTURE_RANGE_QUERY_VALUE_QUERY_ORTHOGONAL_RANGE_REPORTING_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_RANGE_QUERY_VALUE_QUERY_ORTHOGONAL_RANGE_REPORTING_HPP_INCLUDED

#include <utility>
#include <vector>

#include "range_tree.hpp"

template<class Coordinate>
class OrthogonalRangeReporting{
    using Tree = RangeTree<Coordinate, int>;
    Tree tree_;
public:
    explicit OrthogonalRangeReporting(const std::vector<std::pair<Coordinate, Coordinate>>& points)
        : tree_([&]{
            std::vector<typename Tree::Point> result;
            result.reserve(points.size());
            for(int i = 0; i < static_cast<int>(points.size()); ++i)
                result.push_back({points[i].first, points[i].second, i});
            return result;
        }()) {}
    int size() const noexcept { return tree_.size(); }
    std::vector<int> report(const Coordinate& lower_x, const Coordinate& upper_x,
                            const Coordinate& lower_y, const Coordinate& upper_y) const {
        return tree_.report(lower_x, upper_x, lower_y, upper_y);
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_RANGE_QUERY_VALUE_QUERY_ORTHOGONAL_RANGE_REPORTING_HPP_INCLUDED
