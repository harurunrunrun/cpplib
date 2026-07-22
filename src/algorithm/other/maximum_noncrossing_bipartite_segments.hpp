#ifndef CPPLIB_SRC_ALGORITHM_OTHER_MAXIMUM_NONCROSSING_BIPARTITE_SEGMENTS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_MAXIMUM_NONCROSSING_BIPARTITE_SEGMENTS_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>

template<class Coordinate>
inline std::size_t maximum_noncrossing_bipartite_segments(
    std::vector<std::pair<Coordinate, Coordinate>> segments
){
    std::sort(segments.begin(), segments.end());
    std::vector<Coordinate> tails;
    tails.reserve(segments.size());
    for(const auto& segment: segments){
        const auto iterator = std::upper_bound(
            tails.begin(), tails.end(), segment.second
        );
        if(iterator == tails.end()){
            tails.push_back(segment.second);
        }else{
            *iterator = segment.second;
        }
    }
    return tails.size();
}

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_MAXIMUM_NONCROSSING_BIPARTITE_SEGMENTS_HPP_INCLUDED
