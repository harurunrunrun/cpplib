#ifndef CPPLIB_SRC_ALGORITHM_RANGE_OFFLINE_QUERY_MO_WITH_MODIFICATIONS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_RANGE_OFFLINE_QUERY_MO_WITH_MODIFICATIONS_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <vector>

class MoWithModifications{
    struct Query { int left, right, time, index; };
    int n_;
    std::vector<Query> queries_;

public:
    explicit MoWithModifications(int n): n_(n) {
        if(n < 0) throw std::invalid_argument("MoWithModifications: negative size");
    }
    int add_query(int left, int right, int time){
        if(left < 0 || left > right || right > n_ || time < 0)
            throw std::out_of_range("MoWithModifications: invalid query");
        const int id = static_cast<int>(queries_.size());
        queries_.push_back({left, right, time, id});
        return id;
    }
    int query_count() const noexcept { return static_cast<int>(queries_.size()); }

    template<class AddLeft, class AddRight, class RemoveLeft, class RemoveRight,
             class ApplyUpdate, class RollbackUpdate, class Answer>
    void run(AddLeft add_left, AddRight add_right, RemoveLeft remove_left,
             RemoveRight remove_right, ApplyUpdate apply_update,
             RollbackUpdate rollback_update, Answer answer) const {
        if(queries_.empty()) return;
        const int block = std::max(1, static_cast<int>(std::pow(std::max(1, n_), 2.0 / 3.0)));
        std::vector<Query> order = queries_;
        std::sort(order.begin(), order.end(), [block](const Query& a, const Query& b){
            const int al = a.left / block, bl = b.left / block;
            if(al != bl) return al < bl;
            const int ar = a.right / block, br = b.right / block;
            if(ar != br) return (al & 1) ? ar > br : ar < br;
            return (ar & 1) ? a.time > b.time : a.time < b.time;
        });
        int left = 0, right = 0, time = 0;
        for(const Query& query: order){
            while(time < query.time) apply_update(time++, left, right);
            while(time > query.time) rollback_update(--time, left, right);
            while(left > query.left) add_left(--left);
            while(right < query.right) add_right(right++);
            while(left < query.left) remove_left(left++);
            while(right > query.right) remove_right(--right);
            answer(query.index);
        }
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_RANGE_OFFLINE_QUERY_MO_WITH_MODIFICATIONS_HPP_INCLUDED
