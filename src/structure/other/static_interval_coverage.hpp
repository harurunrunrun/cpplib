#ifndef CPPLIB_SRC_STRUCTURE_OTHER_STATIC_INTERVAL_COVERAGE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_OTHER_STATIC_INTERVAL_COVERAGE_HPP_INCLUDED

#include <stdexcept>
#include <utility>
#include <vector>

template<int MAX_COORDINATE = 1000000>
class StaticIntervalCoverage{
    std::vector<int> coverage_;

public:
    StaticIntervalCoverage(
        int coordinate_count,
        const std::vector<std::pair<int, int>>& closed_intervals
    ){
        if(coordinate_count < 0 || MAX_COORDINATE < coordinate_count)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: size out of range "
                "(StaticIntervalCoverage)."
            );
        }
        std::vector<int> difference(
            static_cast<std::size_t>(coordinate_count) + 1,
            0
        );
        for(const auto [left, right]: closed_intervals){
            if(left < 0 || right < left || coordinate_count <= right)[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: interval out of range "
                    "(StaticIntervalCoverage)."
                );
            }
            ++difference[static_cast<std::size_t>(left)];
            --difference[static_cast<std::size_t>(right + 1)];
        }
        coverage_.resize(static_cast<std::size_t>(coordinate_count));
        int current = 0;
        for(int index = 0; index < coordinate_count; ++index){
            current += difference[static_cast<std::size_t>(index)];
            coverage_[static_cast<std::size_t>(index)] = current;
        }
    }

    int size() const{ return static_cast<int>(coverage_.size()); }

    int coverage_at(int position) const{
        if(position < 0 || size() <= position)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: index out of range "
                "(StaticIntervalCoverage)."
            );
        }
        return coverage_[static_cast<std::size_t>(position)];
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_OTHER_STATIC_INTERVAL_COVERAGE_HPP_INCLUDED
