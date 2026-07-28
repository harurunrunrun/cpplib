#ifndef CPPLIB_SRC_STRUCTURE_RANGE_QUERY_EXTREMA_PLUS_MINUS_ONE_RMQ_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_RANGE_QUERY_EXTREMA_PLUS_MINUS_ONE_RMQ_HPP_INCLUDED

#include <algorithm>
#include <bit>
#include <stdexcept>
#include <unordered_map>
#include <vector>

class PlusMinusOneRMQ{
    int n_ = 0, block_size_ = 1;
    std::vector<int> values_, block_minimum_;
    std::vector<std::vector<int>> sparse_;
    std::vector<unsigned long long> signature_;
    std::unordered_map<unsigned long long, std::vector<unsigned short>> lookup_;

    int better(int a, int b) const {
        if(a == -1) return b;
        if(b == -1) return a;
        return values_[a] <= values_[b] ? a : b;
    }
    int within(int block, int left, int right) const {
        if(left >= right) return -1;
        const auto& table = lookup_.at(signature_[block]);
        return block * block_size_ + table[left * block_size_ + right - 1];
    }

public:
    PlusMinusOneRMQ() = default;
    explicit PlusMinusOneRMQ(std::vector<int> values): n_(static_cast<int>(values.size())), values_(std::move(values)){
        for(int i = 1; i < n_; ++i) if(values_[i] - values_[i - 1] != 1 && values_[i] - values_[i - 1] != -1)
            throw std::invalid_argument("PlusMinusOneRMQ: adjacent difference is not one");
        if(n_ == 0) return;
        block_size_ = std::max(1, (static_cast<int>(std::bit_width(static_cast<unsigned int>(n_))) - 1) / 2);
        const int blocks = (n_ + block_size_ - 1) / block_size_;
        signature_.assign(blocks, 0);
        block_minimum_.assign(blocks, -1);
        for(int block = 0; block < blocks; ++block){
            const int begin = block * block_size_;
            const int length = std::min(block_size_, n_ - begin);
            unsigned long long mask = 0;
            for(int i = 1; i < length; ++i) if(values_[begin + i] > values_[begin + i - 1]) mask |= 1ULL << (i - 1);
            mask |= static_cast<unsigned long long>(length) << 56;
            signature_[block] = mask;
            if(!lookup_.contains(mask)){
                std::vector<unsigned short> table(block_size_ * block_size_);
                for(int left = 0; left < length; ++left){
                    int best = left;
                    for(int right = left; right < length; ++right){
                        if(values_[begin + right] < values_[begin + best]) best = right;
                        table[left * block_size_ + right] = static_cast<unsigned short>(best);
                    }
                }
                lookup_.emplace(mask, std::move(table));
            }
            block_minimum_[block] = within(block, 0, length);
        }
        const int levels = static_cast<int>(std::bit_width(static_cast<unsigned int>(blocks)));
        sparse_.assign(levels, std::vector<int>(blocks, -1));
        sparse_[0] = block_minimum_;
        for(int k = 1; k < levels; ++k) for(int i = 0; i + (1 << k) <= blocks; ++i)
            sparse_[k][i] = better(sparse_[k - 1][i], sparse_[k - 1][i + (1 << (k - 1))]);
    }
    int size() const noexcept { return n_; }
    int argmin(int left, int right) const {
        if(left < 0 || left >= right || right > n_) throw std::out_of_range("PlusMinusOneRMQ: invalid range");
        const int first = left / block_size_, last = (right - 1) / block_size_;
        if(first == last) return within(first, left % block_size_, (right - 1) % block_size_ + 1);
        int answer = within(first, left % block_size_, block_size_);
        answer = better(answer, within(last, 0, (right - 1) % block_size_ + 1));
        if(first + 1 < last){
            const int count = last - first - 1;
            const int k = static_cast<int>(std::bit_width(static_cast<unsigned int>(count))) - 1;
            answer = better(answer, better(sparse_[k][first + 1], sparse_[k][last - (1 << k)]));
        }
        return answer;
    }
    int minimum(int left, int right) const { return values_[argmin(left, right)]; }
};

#endif  // CPPLIB_SRC_STRUCTURE_RANGE_QUERY_EXTREMA_PLUS_MINUS_ONE_RMQ_HPP_INCLUDED
