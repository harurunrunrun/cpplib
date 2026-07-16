#pragma once

#include <cstddef>
#include <functional>
#include <optional>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

// A collection of priority queues that also supports taking the best element
// among a contiguous range of queues.
template<class T, class Compare = std::less<T>>
struct RangePriorityQueue{
private:
    using Entry = std::pair<T, std::size_t>;

    struct HeapCompare{
        Compare compare;

        bool operator()(const T& left, const T& right) const{
            return compare(left, right);
        }
    };

    std::size_t bucket_size = 0;
    std::size_t leaf_size = 1;
    Compare compare;
    std::vector<std::priority_queue<T, std::vector<T>, HeapCompare>> heaps;
    std::vector<std::optional<Entry>> segment;

    bool is_better(const Entry& left, const Entry& right) const{
        if(compare(left.first, right.first)) return false;
        if(compare(right.first, left.first)) return true;
        return left.second < right.second;
    }

    std::optional<Entry> merge_entry(
        const std::optional<Entry>& left,
        const std::optional<Entry>& right
    ) const{
        if(!left) return right;
        if(!right) return left;
        return is_better(*left, *right) ? left : right;
    }

    void check_index(std::size_t index) const{
        if(index >= bucket_size)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: index out of range (RangePriorityQueue)."
            );
        }
    }

    void check_range(std::size_t left, std::size_t right) const{
        if(left > right || right > bucket_size)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: invalid range (RangePriorityQueue)."
            );
        }
    }

    void refresh(std::size_t index){
        std::size_t node = leaf_size + index;
        if(heaps[index].empty()) segment[node].reset();
        else segment[node] = Entry{heaps[index].top(), index};
        while(node > 1){
            node >>= 1;
            segment[node] = merge_entry(segment[node << 1], segment[node << 1 | 1]);
        }
    }

    std::optional<Entry> range_best(std::size_t left, std::size_t right) const{
        std::optional<Entry> result_left;
        std::optional<Entry> result_right;
        left += leaf_size;
        right += leaf_size;
        while(left < right){
            if(left & 1U) result_left = merge_entry(result_left, segment[left++]);
            if(right & 1U) result_right = merge_entry(segment[--right], result_right);
            left >>= 1;
            right >>= 1;
        }
        return merge_entry(result_left, result_right);
    }

public:
    explicit RangePriorityQueue(
        std::size_t bucket_count,
        const Compare& compare_ = Compare()
    ): bucket_size(bucket_count), compare(compare_){
        while(leaf_size < bucket_size) leaf_size <<= 1;
        heaps.reserve(bucket_size);
        for(std::size_t index = 0; index < bucket_size; ++index){
            heaps.emplace_back(HeapCompare{compare});
        }
        segment.resize(leaf_size << 1);
    }

    std::size_t size() const noexcept{
        return bucket_size;
    }

    bool empty(std::size_t index) const{
        check_index(index);
        return heaps[index].empty();
    }

    std::size_t bucket_element_count(std::size_t index) const{
        check_index(index);
        return heaps[index].size();
    }

    bool range_empty(std::size_t left, std::size_t right) const{
        check_range(left, right);
        return !range_best(left, right).has_value();
    }

    void push(std::size_t index, const T& value){
        check_index(index);
        heaps[index].push(value);
        refresh(index);
    }

    void push(std::size_t index, T&& value){
        check_index(index);
        heaps[index].push(std::move(value));
        refresh(index);
    }

    const T& bucket_top(std::size_t index) const{
        check_index(index);
        if(heaps[index].empty())[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: empty bucket (RangePriorityQueue::bucket_top)."
            );
        }
        return heaps[index].top();
    }

    T top(std::size_t left, std::size_t right) const{
        check_range(left, right);
        const std::optional<Entry> result = range_best(left, right);
        if(!result)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: empty range (RangePriorityQueue::top)."
            );
        }
        return result->first;
    }

    std::pair<T, std::size_t> top_with_index(
        std::size_t left,
        std::size_t right
    ) const{
        check_range(left, right);
        const std::optional<Entry> result = range_best(left, right);
        if(!result)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: empty range (RangePriorityQueue::top_with_index)."
            );
        }
        return *result;
    }

    T pop(std::size_t left, std::size_t right){
        check_range(left, right);
        const std::optional<Entry> result = range_best(left, right);
        if(!result)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: empty range (RangePriorityQueue::pop)."
            );
        }
        T value = result->first;
        heaps[result->second].pop();
        refresh(result->second);
        return value;
    }

    T pop_bucket(std::size_t index){
        check_index(index);
        if(heaps[index].empty())[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: empty bucket (RangePriorityQueue::pop_bucket)."
            );
        }
        T value = heaps[index].top();
        heaps[index].pop();
        refresh(index);
        return value;
    }
};
