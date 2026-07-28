#ifndef CPPLIB_SRC_ALGORITHM_RANGE_OFFLINE_QUERY_HILBERT_MO_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_RANGE_OFFLINE_QUERY_HILBERT_MO_HPP_INCLUDED

#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <vector>

inline std::uint64_t hilbert_order(unsigned int x, unsigned int y, int bits = 21, int rotation = 0){
    if(bits == 0) return 0;
    const unsigned int half = 1U << (bits - 1);
    unsigned int quadrant = (x < half) ? ((y < half) ? 0U : 3U) : ((y < half) ? 1U : 2U);
    quadrant = (quadrant + static_cast<unsigned int>(rotation)) & 3U;
    static constexpr int rotate_delta[4] = {3, 0, 0, 1};
    const unsigned int nx = x & (half - 1), ny = y & (half - 1);
    const int next_rotation = (rotation + rotate_delta[quadrant]) & 3;
    const std::uint64_t sub_square = 1ULL << (2 * bits - 2);
    const std::uint64_t add = hilbert_order(nx, ny, bits - 1, next_rotation);
    return quadrant * sub_square + ((quadrant == 1 || quadrant == 2) ? add : sub_square - add - 1);
}

class HilbertMo{
    struct Query { int left, right, index; std::uint64_t order; };
    int n_;
    int bits_ = 0;
    std::vector<Query> queries_;
public:
    explicit HilbertMo(int n): n_(n) {
        if(n < 0) throw std::invalid_argument("HilbertMo: negative size");
        while((1U << bits_) < static_cast<unsigned int>(std::max(1, n_))) ++bits_;
    }
    int add_query(int left, int right){
        if(left < 0 || left > right || right > n_) throw std::out_of_range("HilbertMo: invalid query");
        const int id = static_cast<int>(queries_.size());
        queries_.push_back({left, right, id, hilbert_order(left, right, bits_)});
        return id;
    }
    template<class AddLeft, class AddRight, class RemoveLeft, class RemoveRight, class Answer>
    void run(AddLeft add_left, AddRight add_right, RemoveLeft remove_left,
             RemoveRight remove_right, Answer answer) const {
        std::vector<Query> order = queries_;
        std::sort(order.begin(), order.end(), [](const Query& a, const Query& b){ return a.order < b.order; });
        int left = 0, right = 0;
        for(const Query& query: order){
            while(left > query.left) add_left(--left);
            while(right < query.right) add_right(right++);
            while(left < query.left) remove_left(left++);
            while(right > query.right) remove_right(--right);
            answer(query.index);
        }
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_RANGE_OFFLINE_QUERY_HILBERT_MO_HPP_INCLUDED
