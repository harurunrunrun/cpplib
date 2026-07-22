#ifndef CPPLIB_SRC_APPROXIMATE_GEOMETRY_VISVALINGAM_WHYATT_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_GEOMETRY_VISVALINGAM_WHYATT_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <queue>
#include <stdexcept>
#include <vector>

#include "detail/polyline_common.hpp"

namespace approximate::geometry {

template<class T>
std::vector<std::size_t> visvalingam_whyatt_indices(
    const std::vector<std::array<T, 2>>& polyline,
    std::size_t target_size
) {
    for(const auto& point: polyline) detail::validate_polyline_point(point);
    if(target_size > polyline.size()) throw std::invalid_argument("target_size exceeds input size");
    if(polyline.size() >= 2 && target_size < 2){
        throw std::invalid_argument("target_size must retain both endpoints");
    }
    if(target_size == polyline.size()){
        std::vector<std::size_t> indices(polyline.size());
        for(std::size_t index = 0; index < polyline.size(); ++index) indices[index] = index;
        return indices;
    }
    if(polyline.size() <= 1) return {};
    struct Entry { long double area; std::size_t index; std::size_t version; };
    const auto compare = [](const Entry& first, const Entry& second) {
        if(first.area != second.area) return first.area > second.area;
        return first.index > second.index;
    };
    std::priority_queue<Entry, std::vector<Entry>, decltype(compare)> queue(compare);
    const std::size_t absent = polyline.size();
    std::vector<std::size_t> previous(polyline.size(), absent);
    std::vector<std::size_t> next(polyline.size(), absent);
    std::vector<std::size_t> version(polyline.size(), 0);
    std::vector<unsigned char> alive(polyline.size(), 1U);
    for(std::size_t index = 0; index < polyline.size(); ++index){
        if(index != 0) previous[index] = index - 1;
        if(index + 1 != polyline.size()) next[index] = index + 1;
    }
    const auto push_area = [&](std::size_t index) {
        if(index == 0 || index + 1 == polyline.size() || alive[index] == 0U) return;
        queue.push(Entry{
            detail::doubled_triangle_area(
                polyline[previous[index]], polyline[index], polyline[next[index]]
            ), index, version[index]
        });
    };
    for(std::size_t index = 1; index + 1 < polyline.size(); ++index) push_area(index);
    std::size_t alive_count = polyline.size();
    while(alive_count > target_size){
        Entry entry{};
        do {
            if(queue.empty()) throw std::logic_error("Visvalingam queue became empty");
            entry = queue.top();
            queue.pop();
        } while(alive[entry.index] == 0U || version[entry.index] != entry.version);
        const std::size_t left = previous[entry.index];
        const std::size_t right = next[entry.index];
        alive[entry.index] = 0U;
        --alive_count;
        if(left != absent) next[left] = right;
        if(right != absent) previous[right] = left;
        if(left != absent && left != 0){
            ++version[left];
            push_area(left);
        }
        if(right != absent && right + 1 != polyline.size()){
            ++version[right];
            push_area(right);
        }
    }
    std::vector<std::size_t> result;
    result.reserve(target_size);
    for(std::size_t index = 0; index < polyline.size(); ++index){
        if(alive[index] != 0U) result.push_back(index);
    }
    return result;
}

template<class T>
std::vector<std::array<T, 2>> visvalingam_whyatt(
    const std::vector<std::array<T, 2>>& polyline,
    std::size_t target_size
) {
    const auto indices = visvalingam_whyatt_indices(polyline, target_size);
    std::vector<std::array<T, 2>> result;
    result.reserve(indices.size());
    for(std::size_t index: indices) result.push_back(polyline[index]);
    return result;
}

}  // namespace approximate::geometry

#endif  // CPPLIB_SRC_APPROXIMATE_GEOMETRY_VISVALINGAM_WHYATT_HPP_INCLUDED
