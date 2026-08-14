#ifndef CPPLIB_SRC_ALGORITHM_RANGE_OFFLINE_QUERY_DYNAMIC_RANGE_MEX_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_RANGE_OFFLINE_QUERY_DYNAMIC_RANGE_MEX_HPP_INCLUDED

#include <algorithm>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>

class OfflineDynamicRangeMex{
    struct Update{
        std::size_t position;
        std::int64_t before;
        std::int64_t after;
    };
    struct Query{
        std::size_t left;
        std::size_t right;
        std::size_t time;
        std::size_t index;
    };

    class MissingBits{
        std::size_t size_;
        std::vector<std::vector<std::uint64_t>> level_;

        void set_summary(std::size_t level, std::size_t word, bool nonempty){
            if(level + 1 >= level_.size()) return;
            const std::size_t parent_word = word >> 6;
            const std::uint64_t mask = std::uint64_t{1} << (word & 63U);
            const bool before = level_[level + 1][parent_word] != 0;
            if(nonempty) level_[level + 1][parent_word] |= mask;
            else level_[level + 1][parent_word] &= ~mask;
            const bool after = level_[level + 1][parent_word] != 0;
            if(before != after) set_summary(level + 1, parent_word, after);
        }

    public:
        explicit MissingBits(std::size_t size): size_(size){
            std::size_t words = (size + 63) / 64;
            level_.push_back(std::vector<std::uint64_t>(words, ~std::uint64_t{0}));
            if(size % 64 != 0){
                level_[0].back() = (std::uint64_t{1} << (size % 64)) - 1;
            }
            while(words > 1){
                const std::size_t parent_words = (words + 63) / 64;
                std::vector<std::uint64_t> parent(parent_words, ~std::uint64_t{0});
                if(words % 64 != 0){
                    parent.back() = (std::uint64_t{1} << (words % 64)) - 1;
                }
                level_.push_back(std::move(parent));
                words = parent_words;
            }
        }

        void set_missing(std::size_t value, bool missing){
            const std::size_t word = value >> 6;
            const std::uint64_t mask = std::uint64_t{1} << (value & 63U);
            const bool before = level_[0][word] != 0;
            if(missing) level_[0][word] |= mask;
            else level_[0][word] &= ~mask;
            const bool after = level_[0][word] != 0;
            if(before != after) set_summary(0, word, after);
        }

        std::size_t first_missing() const noexcept{
            std::size_t word = 0;
            for(std::size_t level = level_.size(); level-- > 1;){
                const std::uint64_t bits = level_[level][word];
                word = (word << 6) + std::countr_zero(bits);
            }
            const std::uint64_t bits = level_[0][word];
            const std::size_t answer = (word << 6) + std::countr_zero(bits);
            return answer < size_ ? answer : size_;
        }
    };

    std::vector<std::int64_t> initial_;
    std::vector<std::int64_t> scheduled_;
    std::vector<Update> updates_;
    std::vector<Query> queries_;

public:
    explicit OfflineDynamicRangeMex(std::vector<std::int64_t> initial):
        initial_(std::move(initial)), scheduled_(initial_){}

    std::size_t size() const noexcept{ return initial_.size(); }
    std::size_t update_count() const noexcept{ return updates_.size(); }
    std::size_t query_count() const noexcept{ return queries_.size(); }

    void add_update(std::size_t position, std::int64_t value){
        if(position >= initial_.size()){
            throw std::out_of_range("OfflineDynamicRangeMex::add_update: position is out of range");
        }
        updates_.push_back(Update{position, scheduled_[position], value});
        scheduled_[position] = value;
    }

    std::size_t add_query(std::size_t left, std::size_t right){
        if(left > right || right > initial_.size()){
            throw std::out_of_range("OfflineDynamicRangeMex::add_query: invalid half-open range");
        }
        const std::size_t index = queries_.size();
        queries_.push_back(Query{left, right, updates_.size(), index});
        return index;
    }

    std::vector<std::size_t> solve() const{
        if(queries_.empty()) return {};
        std::vector<Query> order = queries_;
        const std::size_t scale = std::max<std::size_t>(1, initial_.size());
        const std::size_t block = std::max<std::size_t>(
            1,
            static_cast<std::size_t>(std::pow(static_cast<long double>(scale), 2.0L / 3.0L))
        );
        std::sort(order.begin(), order.end(), [block](const Query& lhs, const Query& rhs){
            const std::size_t lhs_left = lhs.left / block;
            const std::size_t rhs_left = rhs.left / block;
            if(lhs_left != rhs_left) return lhs_left < rhs_left;
            const std::size_t lhs_right = lhs.right / block;
            const std::size_t rhs_right = rhs.right / block;
            if(lhs_right != rhs_right){
                return (lhs_left & 1U) ? lhs_right > rhs_right : lhs_right < rhs_right;
            }
            return (lhs_right & 1U) ? lhs.time > rhs.time : lhs.time < rhs.time;
        });

        const std::size_t universe = initial_.size() + 1;
        std::vector<std::size_t> frequency(universe, 0);
        MissingBits missing(universe);
        std::vector<std::int64_t> current = initial_;
        const auto tracked = [universe](std::int64_t value){
            return value >= 0 && static_cast<std::uint64_t>(value) < universe;
        };
        const auto add = [&](std::int64_t value){
            if(!tracked(value)) return;
            const std::size_t converted = static_cast<std::size_t>(value);
            if(frequency[converted]++ == 0) missing.set_missing(converted, false);
        };
        const auto remove = [&](std::int64_t value){
            if(!tracked(value)) return;
            const std::size_t converted = static_cast<std::size_t>(value);
            if(--frequency[converted] == 0) missing.set_missing(converted, true);
        };

        std::size_t left = 0;
        std::size_t right = 0;
        std::size_t time = 0;
        std::vector<std::size_t> answer(queries_.size());
        for(const Query& query: order){
            while(time < query.time){
                const Update& update = updates_[time];
                if(left <= update.position && update.position < right){
                    remove(current[update.position]);
                    add(update.after);
                }
                current[update.position] = update.after;
                ++time;
            }
            while(time > query.time){
                --time;
                const Update& update = updates_[time];
                if(left <= update.position && update.position < right){
                    remove(current[update.position]);
                    add(update.before);
                }
                current[update.position] = update.before;
            }
            while(left > query.left) add(current[--left]);
            while(right < query.right) add(current[right++]);
            while(left < query.left) remove(current[left++]);
            while(right > query.right) remove(current[--right]);
            answer[query.index] = missing.first_missing();
        }
        return answer;
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_RANGE_OFFLINE_QUERY_DYNAMIC_RANGE_MEX_HPP_INCLUDED
