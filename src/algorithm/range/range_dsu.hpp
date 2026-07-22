#ifndef CPPLIB_SRC_ALGORITHM_RANGE_RANGE_DSU_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_RANGE_RANGE_DSU_HPP_INCLUDED

#include <algorithm>
#include <map>
#include <numeric>
#include <stdexcept>
#include <vector>

class RangeDSU{
    struct Interval{
        int right;
        int label;
    };

    int n;
    std::vector<int> parent;
    std::vector<int> size_;
    std::map<int, Interval> intervals;

    void check_index(int x) const{
        if(x < 0 || n <= x)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (RangeDSU).");
        }
    }

    void check_range(int l, int r) const{
        if(l < 0 || r < l || n < r)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (RangeDSU).");
        }
    }

    void split(int x){
        if(x <= 0 || n <= x) return;
        auto it = std::prev(intervals.upper_bound(x));
        if(it->first == x) return;
        if(it->second.right <= x) return;
        int right = it->second.right;
        int label = it->second.label;
        it->second.right = x;
        intervals[x] = {right, label};
    }

    void merge_around(int l){
        auto it = intervals.find(l);
        if(it == intervals.end()) return;
        if(it != intervals.begin()){
            auto previous = std::prev(it);
            if(previous->second.right == it->first && leader(previous->second.label) == leader(it->second.label)){
                previous->second.right = it->second.right;
                intervals.erase(it);
                it = previous;
            }
        }
        auto next = std::next(it);
        while(next != intervals.end() && it->second.right == next->first && leader(it->second.label) == leader(next->second.label)){
            it->second.right = next->second.right;
            next = intervals.erase(next);
        }
    }

public:
    explicit RangeDSU(int n_)
        : n(n_ < 0
            ? throw std::runtime_error(
                "library assertion fault: range violation (RangeDSU)."
            )
            : n_),
          parent(static_cast<std::size_t>(n)),
          size_(static_cast<std::size_t>(n), 1){
        std::iota(parent.begin(), parent.end(), 0);
        for(int i = 0; i < n; i++){
            intervals[i] = {i + 1, i};
        }
    }

    int leader(int x){
        check_index(x);
        while(parent[static_cast<std::size_t>(x)] != x){
            parent[static_cast<std::size_t>(x)] = parent[static_cast<std::size_t>(parent[static_cast<std::size_t>(x)])];
            x = parent[static_cast<std::size_t>(x)];
        }
        return x;
    }

    int leader(int x) const{
        check_index(x);
        while(parent[static_cast<std::size_t>(x)] != x){
            x = parent[static_cast<std::size_t>(x)];
        }
        return x;
    }

    bool same(int x, int y) const{
        return leader(x) == leader(y);
    }

    bool unite(int x, int y){
        x = leader(x);
        y = leader(y);
        if(x == y) return false;
        if(size_[static_cast<std::size_t>(x)] < size_[static_cast<std::size_t>(y)]) std::swap(x, y);
        parent[static_cast<std::size_t>(y)] = x;
        size_[static_cast<std::size_t>(x)] += size_[static_cast<std::size_t>(y)];
        return true;
    }

    void unite_range(int l, int r, int x){
        check_range(l, r);
        check_index(x);
        if(l == r) return;
        split(l);
        split(r);
        int root = leader(x);
        auto it = intervals.lower_bound(l);
        auto last = intervals.lower_bound(r);
        for(auto jt = it; jt != last; ++jt){
            unite(root, jt->second.label);
            root = leader(root);
        }
        intervals.erase(it, last);
        intervals[l] = {r, root};
        merge_around(l);
    }

    int size(int x) const{
        return size_[static_cast<std::size_t>(leader(x))];
    }

    bool same_range(int l, int r) const{
        check_range(l, r);
        if(l == r) return true;
        auto it = std::prev(intervals.upper_bound(l));
        int root = leader(it->second.label);
        while(it != intervals.end() && it->first < r){
            if(leader(it->second.label) != root) return false;
            if(r <= it->second.right) return true;
            ++it;
        }
        return true;
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_RANGE_RANGE_DSU_HPP_INCLUDED
