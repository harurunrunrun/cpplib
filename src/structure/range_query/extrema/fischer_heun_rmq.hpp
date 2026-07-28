#ifndef CPPLIB_SRC_STRUCTURE_RANGE_QUERY_EXTREMA_FISCHER_HEUN_RMQ_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_RANGE_QUERY_EXTREMA_FISCHER_HEUN_RMQ_HPP_INCLUDED

#include <functional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "plus_minus_one_rmq.hpp"

template<class T, class Compare = std::less<T>>
class FischerHeunRMQ{
    std::vector<T> values_;
    Compare compare_;
    std::vector<int> first_, euler_;
    PlusMinusOneRMQ depth_rmq_;

public:
    FischerHeunRMQ() = default;
    explicit FischerHeunRMQ(std::vector<T> values, Compare compare = Compare{})
        : values_(std::move(values)), compare_(std::move(compare)){
        const int n = static_cast<int>(values_.size());
        if(n == 0) return;
        std::vector<int> parent(n, -1), left(n, -1), right(n, -1), stack;
        for(int i = 0; i < n; ++i){
            int last = -1;
            while(!stack.empty() && compare_(values_[i], values_[stack.back()])){
                last = stack.back();
                stack.pop_back();
            }
            if(!stack.empty()){ parent[i] = stack.back(); right[stack.back()] = i; }
            if(last != -1){ parent[last] = i; left[i] = last; }
            stack.push_back(i);
        }
        int root = 0;
        while(parent[root] != -1) root = parent[root];
        first_.assign(n, -1);
        std::vector<int> depths;
        struct Frame { int vertex, depth, state; };
        std::vector<Frame> dfs{{root, 0, 0}};
        while(!dfs.empty()){
            Frame& frame = dfs.back();
            if(frame.state == 0){
                if(first_[frame.vertex] == -1) first_[frame.vertex] = static_cast<int>(euler_.size());
                euler_.push_back(frame.vertex);
                depths.push_back(frame.depth);
                frame.state = 1;
                if(left[frame.vertex] != -1){ dfs.push_back({left[frame.vertex], frame.depth + 1, 0}); continue; }
            }
            if(frame.state == 1){
                frame.state = 2;
                if(left[frame.vertex] != -1){ euler_.push_back(frame.vertex); depths.push_back(frame.depth); }
                if(right[frame.vertex] != -1){ dfs.push_back({right[frame.vertex], frame.depth + 1, 0}); continue; }
            }
            if(right[frame.vertex] != -1){ euler_.push_back(frame.vertex); depths.push_back(frame.depth); }
            dfs.pop_back();
        }
        depth_rmq_ = PlusMinusOneRMQ(std::move(depths));
    }
    int size() const noexcept { return static_cast<int>(values_.size()); }
    int argmin(int left, int right) const {
        if(left < 0 || left >= right || right > size()) throw std::out_of_range("FischerHeunRMQ: invalid range");
        int a = first_[left], b = first_[right - 1];
        if(a > b) std::swap(a, b);
        return euler_[depth_rmq_.argmin(a, b + 1)];
    }
    const T& minimum(int left, int right) const { return values_[argmin(left, right)]; }
};

#endif  // CPPLIB_SRC_STRUCTURE_RANGE_QUERY_EXTREMA_FISCHER_HEUN_RMQ_HPP_INCLUDED
