#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_ANGLE_INTERVAL_SET_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_ANGLE_INTERVAL_SET_HPP_INCLUDED

#include <cmath>
#include <cstddef>
#include <map>
#include <stdexcept>
#include <utility>
#include <vector>

#include "constants.hpp"

class AngleIntervalSet{
    using IntervalMap = std::map<long double, long double>;

    IntervalMap data_;
    long double covered_length_ = 0.0L;

    static long double full_turn(){
        return 2.0L * std::acos(-1.0L);
    }

    static void validate_angle(long double angle){
        if(!std::isfinite(angle))[[unlikely]]{
            throw std::invalid_argument("angle must be finite");
        }
    }

    static long double normalize(long double angle){
        const long double turn = full_turn();
        angle = std::fmod(angle, turn);
        if(angle < 0.0L) angle += turn;
        if(angle >= turn) angle = 0.0L;
        return angle;
    }

    static bool is_full_arc(long double start, long double end){
        return std::fabs(end - start) >= full_turn() - GEOMETRY_EPS;
    }

    static long double arc_length(long double start, long double end){
        long double result = normalize(end) - normalize(start);
        if(result < 0.0L) result += full_turn();
        return result;
    }

    void add_linear(long double left, long double right){
        if(right - left <= GEOMETRY_EPS) return;
        auto iterator = data_.lower_bound(left);
        if(iterator != data_.begin()){
            auto previous = std::prev(iterator);
            if(previous->second + GEOMETRY_EPS >= left) iterator = previous;
        }
        while(iterator != data_.end()
            && iterator->first <= right + GEOMETRY_EPS){
            if(iterator->second + GEOMETRY_EPS < left){
                ++iterator;
                continue;
            }
            left = std::min(left, iterator->first);
            right = std::max(right, iterator->second);
            covered_length_ -= iterator->second - iterator->first;
            iterator = data_.erase(iterator);
        }
        data_.emplace(left, right);
        covered_length_ += right - left;
    }

    void remove_linear(long double left, long double right){
        if(right - left <= GEOMETRY_EPS || data_.empty()) return;
        auto iterator = data_.lower_bound(left);
        if(iterator != data_.begin()) --iterator;
        std::vector<std::pair<long double, long double>> leftovers;
        while(iterator != data_.end() && iterator->first < right){
            if(iterator->second <= left){
                ++iterator;
                continue;
            }
            const auto current = *iterator;
            covered_length_ -= current.second - current.first;
            iterator = data_.erase(iterator);
            if(current.first < left){
                leftovers.emplace_back(current.first, left);
            }
            if(right < current.second){
                leftovers.emplace_back(right, current.second);
            }
        }
        for(const auto& interval: leftovers){
            data_.emplace(interval);
            covered_length_ += interval.second - interval.first;
        }
    }

    bool contains_linear(long double angle) const{
        auto iterator = data_.upper_bound(angle);
        if(iterator == data_.begin()) return false;
        --iterator;
        return iterator->first <= angle && angle < iterator->second;
    }

    bool covers_linear(long double left, long double right) const{
        if(right - left <= GEOMETRY_EPS) return true;
        auto iterator = data_.upper_bound(left);
        if(iterator == data_.begin()) return false;
        --iterator;
        return iterator->first <= left
            && right <= iterator->second + GEOMETRY_EPS;
    }

public:
    bool empty() const{ return data_.empty(); }

    bool full() const{
        return covered_length_ >= full_turn() - GEOMETRY_EPS;
    }

    std::size_t interval_count() const{ return data_.size(); }

    long double covered_length() const{ return covered_length_; }

    std::vector<std::pair<long double, long double>> intervals() const{
        return {data_.begin(), data_.end()};
    }

    void clear(){
        data_.clear();
        covered_length_ = 0.0L;
    }

    void add(long double start, long double end){
        validate_angle(start);
        validate_angle(end);
        if(is_full_arc(start, end)){
            clear();
            add_linear(0.0L, full_turn());
            return;
        }
        const long double length = arc_length(start, end);
        if(length <= GEOMETRY_EPS) return;
        const long double left = normalize(start);
        const long double right = normalize(end);
        if(left < right){
            add_linear(left, right);
        }else{
            add_linear(left, full_turn());
            add_linear(0.0L, right);
        }
    }

    void remove(long double start, long double end){
        validate_angle(start);
        validate_angle(end);
        if(is_full_arc(start, end)){
            clear();
            return;
        }
        const long double length = arc_length(start, end);
        if(length <= GEOMETRY_EPS) return;
        const long double left = normalize(start);
        const long double right = normalize(end);
        if(left < right){
            remove_linear(left, right);
        }else{
            remove_linear(left, full_turn());
            remove_linear(0.0L, right);
        }
    }

    bool contains(long double angle) const{
        validate_angle(angle);
        return contains_linear(normalize(angle));
    }

    bool covers(long double start, long double end) const{
        validate_angle(start);
        validate_angle(end);
        if(is_full_arc(start, end)) return full();
        const long double length = arc_length(start, end);
        if(length <= GEOMETRY_EPS) return true;
        const long double left = normalize(start);
        const long double right = normalize(end);
        if(left < right){
            return covers_linear(left, right);
        }
        return covers_linear(left, full_turn())
            && covers_linear(0.0L, right);
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_ANGLE_INTERVAL_SET_HPP_INCLUDED
