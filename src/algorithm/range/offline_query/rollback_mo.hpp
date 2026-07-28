#ifndef CPPLIB_SRC_ALGORITHM_RANGE_OFFLINE_QUERY_ROLLBACK_MO_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_RANGE_OFFLINE_QUERY_ROLLBACK_MO_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <vector>

class RollbackMo{
    struct Query { int left, right, index; };
    int n_;
    std::vector<Query> queries_;

public:
    explicit RollbackMo(int n): n_(n) {
        if(n < 0) throw std::invalid_argument("RollbackMo: negative size");
    }
    int add_query(int left, int right){
        if(left < 0 || left > right || right > n_) throw std::out_of_range("RollbackMo: invalid query");
        const int id = static_cast<int>(queries_.size());
        queries_.push_back({left, right, id});
        return id;
    }

    template<class Add, class Snapshot, class Rollback, class Answer>
    void run(Add add, Snapshot snapshot, Rollback rollback, Answer answer) const {
        const int block = std::max(1, static_cast<int>(std::sqrt(std::max(1, n_))));
        std::vector<Query> order = queries_;
        std::sort(order.begin(), order.end(), [block](const Query& a, const Query& b){
            if(a.left / block != b.left / block) return a.left < b.left;
            return a.right < b.right;
        });
        std::size_t begin = 0;
        while(begin < order.size()){
            const int block_id = order[begin].left / block;
            std::size_t end = begin;
            while(end < order.size() && order[end].left / block == block_id) ++end;
            const int boundary = std::min(n_, (block_id + 1) * block);
            int right = boundary;
            const auto base = snapshot();
            for(std::size_t i = begin; i < end; ++i){
                const Query& query = order[i];
                if(query.right <= boundary){
                    const auto local = snapshot();
                    for(int p = query.left; p < query.right; ++p) add(p);
                    answer(query.index);
                    rollback(local);
                    continue;
                }
                while(right < query.right) add(right++);
                const auto local = snapshot();
                for(int p = boundary - 1; p >= query.left; --p) add(p);
                answer(query.index);
                rollback(local);
            }
            rollback(base);
            begin = end;
        }
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_RANGE_OFFLINE_QUERY_ROLLBACK_MO_HPP_INCLUDED
