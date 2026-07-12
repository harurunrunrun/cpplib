#pragma once

#include <algorithm>
#include <cstddef>
#include <vector>

template<class T>
struct RectangleUnionRectangle{
    T x1;
    T y1;
    T x2;
    T y2;
};

namespace rectangle_union_internal{

template<class T>
struct Event{
    T x;
    T y1;
    T y2;
    int delta;
};

template<class T>
class SegmentTree{
    int n;
    const std::vector<T>& ys;
    std::vector<int> count;
    std::vector<T> length;

    void pull(int node, int left, int right){
        if(count[static_cast<std::size_t>(node)] > 0){
            length[static_cast<std::size_t>(node)] = ys[static_cast<std::size_t>(right)] - ys[static_cast<std::size_t>(left)];
        }else if(left + 1 == right){
            length[static_cast<std::size_t>(node)] = T(0);
        }else{
            length[static_cast<std::size_t>(node)] =
                length[static_cast<std::size_t>(node * 2)] + length[static_cast<std::size_t>(node * 2 + 1)];
        }
    }

    void add(int node, int left, int right, int query_left, int query_right, int delta){
        if(query_right <= left || right <= query_left) return;
        if(query_left <= left && right <= query_right){
            count[static_cast<std::size_t>(node)] += delta;
            pull(node, left, right);
            return;
        }
        int mid = (left + right) / 2;
        add(node * 2, left, mid, query_left, query_right, delta);
        add(node * 2 + 1, mid, right, query_left, query_right, delta);
        pull(node, left, right);
    }

public:
    explicit SegmentTree(const std::vector<T>& ys_) : n(static_cast<int>(ys_.size()) - 1), ys(ys_){
        count.assign(static_cast<std::size_t>(std::max(1, 4 * n + 4)), 0);
        length.assign(static_cast<std::size_t>(std::max(1, 4 * n + 4)), T(0));
    }

    void add(int left, int right, int delta){
        if(left >= right || n <= 0) return;
        add(1, 0, n, left, right, delta);
    }

    T all() const{
        return length[1];
    }
};

} // namespace rectangle_union_internal

template<class T>
T rectangle_union_area(std::vector<RectangleUnionRectangle<T>> rectangles){
    std::vector<T> ys;
    std::vector<rectangle_union_internal::Event<T>> events;
    ys.reserve(rectangles.size() * 2);
    events.reserve(rectangles.size() * 2);
    for(auto rect: rectangles){
        if(rect.x1 > rect.x2) std::swap(rect.x1, rect.x2);
        if(rect.y1 > rect.y2) std::swap(rect.y1, rect.y2);
        if(rect.x1 == rect.x2 || rect.y1 == rect.y2) continue;
        ys.push_back(rect.y1);
        ys.push_back(rect.y2);
        events.push_back({rect.x1, rect.y1, rect.y2, 1});
        events.push_back({rect.x2, rect.y1, rect.y2, -1});
    }
    if(events.empty()) return T(0);
    std::sort(ys.begin(), ys.end());
    ys.erase(std::unique(ys.begin(), ys.end()), ys.end());
    std::sort(events.begin(), events.end(), [](const auto& lhs, const auto& rhs){
        return lhs.x < rhs.x;
    });
    rectangle_union_internal::SegmentTree<T> seg(ys);
    T answer = T(0);
    T previous_x = events[0].x;
    for(std::size_t i = 0; i < events.size();){
        T x = events[i].x;
        answer += seg.all() * (x - previous_x);
        while(i < events.size() && events[i].x == x){
            int y1 = static_cast<int>(std::lower_bound(ys.begin(), ys.end(), events[i].y1) - ys.begin());
            int y2 = static_cast<int>(std::lower_bound(ys.begin(), ys.end(), events[i].y2) - ys.begin());
            seg.add(y1, y2, events[i].delta);
            i++;
        }
        previous_x = x;
    }
    return answer;
}
