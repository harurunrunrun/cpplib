#ifndef CPPLIB_SRC_STRUCTURE_ORDERED_SET_INTEGER_MEX_MULTISET_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_ORDERED_SET_INTEGER_MEX_MULTISET_HPP_INCLUDED

#include <cstddef>
#include <optional>
#include <stdexcept>
#include <vector>

class MexMultiset{
    std::size_t universe_ = 0;
    std::size_t leaf_count_ = 1;
    std::vector<std::size_t> frequency_;
    std::vector<std::size_t> present_count_;

    void refresh(std::size_t node){
        while(node > 1){
            node >>= 1;
            present_count_[node] =
                present_count_[node << 1] + present_count_[node << 1 | 1];
        }
    }

public:
    explicit MexMultiset(std::size_t universe): universe_(universe), frequency_(universe, 0){
        while(leaf_count_ < universe_) leaf_count_ <<= 1;
        present_count_.assign(leaf_count_ << 1, 0);
        for(std::size_t value = universe_; value < leaf_count_; value++){
            present_count_[leaf_count_ + value] = 1;
        }
        for(std::size_t node = leaf_count_; node-- > 1;){
            present_count_[node] =
                present_count_[node << 1] + present_count_[node << 1 | 1];
        }
    }

    std::size_t universe() const noexcept{ return universe_; }
    std::size_t distinct_size() const noexcept{ return present_count_[1] - (leaf_count_ - universe_); }
    bool empty() const noexcept{ return distinct_size() == 0; }

    std::size_t count(std::size_t value) const{
        if(value >= universe_) throw std::out_of_range("MexMultiset::count: value is outside the universe");
        return frequency_[value];
    }

    bool insert(std::size_t value){
        if(value >= universe_) throw std::out_of_range("MexMultiset::insert: value is outside the universe");
        const bool was_missing = frequency_[value] == 0;
        ++frequency_[value];
        if(was_missing){
            const std::size_t node = leaf_count_ + value;
            present_count_[node] = 1;
            refresh(node);
        }
        return was_missing;
    }

    bool erase(std::size_t value){
        if(value >= universe_) throw std::out_of_range("MexMultiset::erase: value is outside the universe");
        if(frequency_[value] == 0) return false;
        --frequency_[value];
        if(frequency_[value] == 0){
            const std::size_t node = leaf_count_ + value;
            present_count_[node] = 0;
            refresh(node);
            return true;
        }
        return false;
    }

    std::size_t mex() const noexcept{
        if(universe_ == 0 || distinct_size() == universe_) return universe_;
        std::size_t node = 1;
        std::size_t width = leaf_count_;
        while(node < leaf_count_){
            width >>= 1;
            const std::size_t left = node << 1;
            if(present_count_[left] < width) node = left;
            else node = left | 1;
        }
        return node - leaf_count_;
    }

    std::optional<std::size_t> kth_missing(std::size_t index) const noexcept{
        const std::size_t missing = universe_ - distinct_size();
        if(index >= missing) return std::nullopt;
        std::size_t node = 1;
        std::size_t width = leaf_count_;
        while(node < leaf_count_){
            width >>= 1;
            const std::size_t left = node << 1;
            const std::size_t left_missing = width - present_count_[left];
            if(index < left_missing) node = left;
            else{
                index -= left_missing;
                node = left | 1;
            }
        }
        const std::size_t answer = node - leaf_count_;
        return answer < universe_ ? std::optional<std::size_t>(answer) : std::nullopt;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_ORDERED_SET_INTEGER_MEX_MULTISET_HPP_INCLUDED
