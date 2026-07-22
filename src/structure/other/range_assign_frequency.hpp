#ifndef CPPLIB_SRC_STRUCTURE_OTHER_RANGE_ASSIGN_FREQUENCY_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_OTHER_RANGE_ASSIGN_FREQUENCY_HPP_INCLUDED

#include <cstddef>
#include <functional>
#include <iterator>
#include <map>
#include <stdexcept>
#include <utility>
#include <vector>

template<class T, class Compare = std::less<T>>
class RangeAssignFrequency{
public:
    struct Segment{
        std::size_t left;
        std::size_t right;
        T value;
    };

private:
    std::size_t element_count_ = 0;
    [[no_unique_address]] Compare compare_;
    std::map<std::size_t, T> runs_;
    std::map<T, std::size_t, Compare> frequencies_;

    bool equivalent(const T& left, const T& right) const{
        return !compare_(left, right) && !compare_(right, left);
    }

    typename std::map<std::size_t, T>::iterator split(
        std::size_t position
    ){
        if(position == element_count_) return runs_.end();
        auto upper = runs_.upper_bound(position);
        auto current = std::prev(upper);
        if(current->first == position) return current;
        return runs_.emplace(position, current->second).first;
    }

    void subtract_frequency(const T& value, std::size_t count){
        auto found = frequencies_.find(value);
        if(found == frequencies_.end() || found->second < count)[[unlikely]]{
            throw std::logic_error(
                "library assertion fault: inconsistent frequency "
                "(RangeAssignFrequency)."
            );
        }
        found->second -= count;
        if(found->second == 0) frequencies_.erase(found);
    }

    void check_range(std::size_t left, std::size_t right) const{
        if(left > right || right > element_count_)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(RangeAssignFrequency::assign)."
            );
        }
    }

public:
    explicit RangeAssignFrequency(Compare compare = Compare{})
        : compare_(std::move(compare)), frequencies_(compare_){}

    RangeAssignFrequency(
        std::size_t size,
        const T& initial_value,
        Compare compare = Compare{}
    ) : element_count_(size),
        compare_(std::move(compare)), frequencies_(compare_){
        if(size != 0){
            runs_.emplace(0, initial_value);
            frequencies_.emplace(initial_value, size);
        }
    }

    explicit RangeAssignFrequency(
        const std::vector<T>& values,
        Compare compare = Compare{}
    ) : element_count_(values.size()),
        compare_(std::move(compare)), frequencies_(compare_){
        for(std::size_t index = 0; index < values.size(); ++index){
            auto [found, inserted] = frequencies_.try_emplace(values[index], 0);
            ++found->second;
            if(index == 0 || !equivalent(values[index - 1], values[index])){
                runs_.emplace(index, values[index]);
            }
        }
    }

    std::size_t size() const noexcept{ return element_count_; }
    bool empty() const noexcept{ return element_count_ == 0; }
    std::size_t segment_count() const noexcept{ return runs_.size(); }

    const T& get(std::size_t index) const{
        if(index >= element_count_)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(RangeAssignFrequency::get)."
            );
        }
        return std::prev(runs_.upper_bound(index))->second;
    }

    std::size_t frequency(const T& value) const{
        const auto found = frequencies_.find(value);
        return found == frequencies_.end() ? 0 : found->second;
    }

    Segment segment(std::size_t index) const{
        if(index >= element_count_)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(RangeAssignFrequency::segment)."
            );
        }
        const auto current = std::prev(runs_.upper_bound(index));
        const auto next = std::next(current);
        return {
            current->first,
            next == runs_.end() ? element_count_ : next->first,
            current->second
        };
    }

    std::vector<Segment> segments() const{
        std::vector<Segment> result;
        result.reserve(runs_.size());
        for(auto current = runs_.begin(); current != runs_.end(); ++current){
            const auto next = std::next(current);
            result.push_back({
                current->first,
                next == runs_.end() ? element_count_ : next->first,
                current->second
            });
        }
        return result;
    }

    void assign(std::size_t left, std::size_t right, const T& value){
        check_range(left, right);
        if(left == right) return;

        auto first = split(left);
        const auto last = split(right);
        for(auto current = first; current != last; ++current){
            const auto next = std::next(current);
            const std::size_t segment_right =
                next == runs_.end() ? element_count_ : next->first;
            subtract_frequency(current->second, segment_right - current->first);
        }
        runs_.erase(first, last);
        auto [inserted, created] = runs_.emplace(left, value);
        (void)created;
        auto [count, count_created] = frequencies_.try_emplace(value, 0);
        (void)count_created;
        count->second += right - left;

        if(inserted != runs_.begin()){
            const auto previous = std::prev(inserted);
            if(equivalent(previous->second, inserted->second)){
                runs_.erase(inserted);
                inserted = previous;
            }
        }
        const auto next = std::next(inserted);
        if(next != runs_.end() && equivalent(inserted->second, next->second)){
            runs_.erase(next);
        }
    }

    void set(std::size_t left, std::size_t right, const T& value){
        assign(left, right, value);
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_OTHER_RANGE_ASSIGN_FREQUENCY_HPP_INCLUDED
