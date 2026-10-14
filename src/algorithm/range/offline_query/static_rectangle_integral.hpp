#ifndef CPPLIB_SRC_ALGORITHM_RANGE_OFFLINE_QUERY_STATIC_RECTANGLE_INTEGRAL_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_RANGE_OFFLINE_QUERY_STATIC_RECTANGLE_INTEGRAL_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cstdint>
#include <stdexcept>
#include <vector>

template<class T>
struct WeightedRectangle{
    std::int64_t left, lower, right, upper;
    T weight;
};

struct RectangleIntegralQuery{
    std::int64_t left, lower, right, upper;
};

// Integrate the sum of constant rectangle densities over query rectangles.
template<class T>
std::vector<T> static_rectangle_integral(
    const std::vector<WeightedRectangle<T>>& rectangles,
    const std::vector<RectangleIntegralQuery>& queries){
    struct Event{ std::int64_t x, y; T weight; int query, sign; };
    std::vector<Event> events;
    std::vector<std::int64_t> ys;
    events.reserve(4 * (rectangles.size() + queries.size()));
    for(const auto& r: rectangles){
        if(r.left > r.right || r.lower > r.upper) throw std::invalid_argument("rectangle integral bounds");
        for(int i = 0; i < 4; ++i){
            auto x = i & 1 ? r.right : r.left;
            auto y = i & 2 ? r.upper : r.lower;
            events.push_back({x, y, ((i == 1 || i == 2) ? -r.weight : r.weight), -1, 0});
            ys.push_back(y);
        }
    }
    for(int id = 0; id < static_cast<int>(queries.size()); ++id){
        const auto& r = queries[id];
        if(r.left > r.right || r.lower > r.upper) throw std::invalid_argument("rectangle integral query bounds");
        for(int i = 0; i < 4; ++i)
            events.push_back({i & 1 ? r.right : r.left, i & 2 ? r.upper : r.lower,
                              T{}, id, (i == 1 || i == 2) ? -1 : 1});
    }
    std::sort(ys.begin(), ys.end()); ys.erase(std::unique(ys.begin(), ys.end()), ys.end());
    std::sort(events.begin(), events.end(), [](const Event& a, const Event& b){ return a.x < b.x; });
    std::vector<std::array<T, 4>> bit(ys.size() + 1);
    std::vector<T> answer(queries.size());
    for(const auto& e: events){
        int p = std::lower_bound(ys.begin(), ys.end(), e.y) - ys.begin();
        if(e.query == -1){
            std::array<T, 4> c{e.weight, e.weight * T(e.x), e.weight * T(e.y), e.weight * T(e.x) * T(e.y)};
            for(++p; p < static_cast<int>(bit.size()); p += p & -p)
                for(int j = 0; j < 4; ++j) bit[p][j] += c[j];
        }else{
            std::array<T, 4> c{};
            for(; p > 0; p -= p & -p) for(int j = 0; j < 4; ++j) c[j] += bit[p][j];
            const T value = c[0] * T(e.x) * T(e.y) - c[1] * T(e.y) - c[2] * T(e.x) + c[3];
            answer[e.query] += e.sign == 1 ? value : -value;
        }
    }
    return answer;
}

#endif  // CPPLIB_SRC_ALGORITHM_RANGE_OFFLINE_QUERY_STATIC_RECTANGLE_INTEGRAL_HPP_INCLUDED
