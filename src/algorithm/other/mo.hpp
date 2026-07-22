#ifndef CPPLIB_SRC_ALGORITHM_OTHER_MO_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_MO_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <vector>

class Mo{
    struct Query{
        int left;
        int right;
        int index;
    };

    int n;
    std::vector<Query> queries;

public:
    explicit Mo(int n_) : n(n_){
        if(n < 0)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (Mo).");
        }
    }

    int add_query(int left, int right){
        if(left < 0 || right < left || n < right)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (Mo::add_query).");
        }
        int index = static_cast<int>(queries.size());
        queries.push_back({left, right, index});
        return index;
    }

    int size() const{
        return static_cast<int>(queries.size());
    }

    template<class AddLeft, class AddRight, class EraseLeft, class EraseRight, class Answer>
    void solve(
        AddLeft add_left,
        AddRight add_right,
        EraseLeft erase_left,
        EraseRight erase_right,
        Answer answer,
        int block_size = -1
    ) const{
        if(block_size <= 0){
            block_size = std::max(1, static_cast<int>(static_cast<long double>(n) / std::max<long double>(1.0L, std::sqrt(queries.size()))));
        }
        std::vector<Query> order = queries;
        std::sort(order.begin(), order.end(), [&](const Query& a, const Query& b){
            int ablock = a.left / block_size;
            int bblock = b.left / block_size;
            if(ablock != bblock) return ablock < bblock;
            if(ablock & 1) return a.right > b.right;
            return a.right < b.right;
        });
        int left = 0;
        int right = 0;
        for(const Query& query: order){
            while(query.left < left) add_left(--left);
            while(right < query.right) add_right(right++);
            while(left < query.left) erase_left(left++);
            while(query.right < right) erase_right(--right);
            answer(query.index);
        }
    }

    template<class Add, class Erase, class Answer>
    void solve(Add add, Erase erase, Answer answer, int block_size = -1) const{
        solve(add, add, erase, erase, answer, block_size);
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_MO_HPP_INCLUDED
