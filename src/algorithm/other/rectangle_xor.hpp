#pragma once

#include <algorithm>
#include <cstddef>
#include <vector>

template<class T>
struct RectangleXorRectangle{
    T x1;
    T y1;
    T x2;
    T y2;
};

namespace rectangle_xor_internal{

template<class T>
struct Event{
    T x;
    T y1;
    T y2;
};

template<class T>
class SegmentTree{
    int n;
    const std::vector<T>& ys;
    std::vector<char> lazy;
    std::vector<T> length;

    void apply(int node, int left, int right){
        length[static_cast<std::size_t>(node)] =
            (ys[static_cast<std::size_t>(right)] - ys[static_cast<std::size_t>(left)]) -
            length[static_cast<std::size_t>(node)];
        lazy[static_cast<std::size_t>(node)] ^= 1;
    }

    void push(int node, int left, int right){
        if(!lazy[static_cast<std::size_t>(node)] || left + 1 == right) return;
        int mid = (left + right) / 2;
        apply(node * 2, left, mid);
        apply(node * 2 + 1, mid, right);
        lazy[static_cast<std::size_t>(node)] = 0;
    }

    void pull(int node){
        length[static_cast<std::size_t>(node)] =
            length[static_cast<std::size_t>(node * 2)] + length[static_cast<std::size_t>(node * 2 + 1)];
    }

    void toggle(int node, int left, int right, int query_left, int query_right){
        if(query_right <= left || right <= query_left) return;
        if(query_left <= left && right <= query_right){
            apply(node, left, right);
            return;
        }
        push(node, left, right);
        int mid = (left + right) / 2;
        toggle(node * 2, left, mid, query_left, query_right);
        toggle(node * 2 + 1, mid, right, query_left, query_right);
        pull(node);
    }

public:
    explicit SegmentTree(const std::vector<T>& ys_) : n(static_cast<int>(ys_.size()) - 1), ys(ys_){
        lazy.assign(static_cast<std::size_t>(std::max(1, 4 * n + 4)), 0);
        length.assign(static_cast<std::size_t>(std::max(1, 4 * n + 4)), T(0));
    }

    void toggle(int left, int right){
        if(left >= right || n <= 0) return;
        toggle(1, 0, n, left, right);
    }

    T all() const{
        return length[1];
    }
};

} // namespace rectangle_xor_internal

template<class T>
T rectangle_xor_area(std::vector<RectangleXorRectangle<T>> rectangles){
    std::vector<T> ys;
    std::vector<rectangle_xor_internal::Event<T>> events;
    ys.reserve(rectangles.size() * 2);
    events.reserve(rectangles.size() * 2);
    for(auto rect: rectangles){
        if(rect.x1 > rect.x2) std::swap(rect.x1, rect.x2);
        if(rect.y1 > rect.y2) std::swap(rect.y1, rect.y2);
        if(rect.x1 == rect.x2 || rect.y1 == rect.y2) continue;
        ys.push_back(rect.y1);
        ys.push_back(rect.y2);
        events.push_back({rect.x1, rect.y1, rect.y2});
        events.push_back({rect.x2, rect.y1, rect.y2});
    }
    if(events.empty()) return T(0);
    std::sort(ys.begin(), ys.end());
    ys.erase(std::unique(ys.begin(), ys.end()), ys.end());
    std::sort(events.begin(), events.end(), [](const auto& lhs, const auto& rhs){
        return lhs.x < rhs.x;
    });
    rectangle_xor_internal::SegmentTree<T> seg(ys);
    T answer = T(0);
    T previous_x = events[0].x;
    for(std::size_t i = 0; i < events.size();){
        T x = events[i].x;
        answer += seg.all() * (x - previous_x);
        while(i < events.size() && events[i].x == x){
            int y1 = static_cast<int>(std::lower_bound(ys.begin(), ys.end(), events[i].y1) - ys.begin());
            int y2 = static_cast<int>(std::lower_bound(ys.begin(), ys.end(), events[i].y2) - ys.begin());
            seg.toggle(y1, y2);
            i++;
        }
        previous_x = x;
    }
    return answer;
}
