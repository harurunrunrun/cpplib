#ifndef CPPLIB_SRC_ALGORITHM_RANGE_INTERVAL_SET_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_RANGE_INTERVAL_SET_HPP_INCLUDED

#include <algorithm>
#include <iterator>
#include <map>
#include <optional>
#include <type_traits>
#include <utility>
#include <vector>

template<class T>
class IntervalSet{
    std::map<T, T> intervals_;

public:
    IntervalSet() = default;

    bool empty() const{
        return intervals_.empty();
    }

    int size() const{
        return static_cast<int>(intervals_.size());
    }

    void clear(){
        intervals_.clear();
    }

    const std::map<T, T>& intervals() const{
        return intervals_;
    }

    bool contains(T x) const{
        auto it = intervals_.upper_bound(x);
        if(it == intervals_.begin()) return false;
        --it;
        return it->first <= x && x < it->second;
    }

    bool covered(T l, T r) const{
        if(l >= r) return true;
        auto it = intervals_.upper_bound(l);
        if(it == intervals_.begin()) return false;
        --it;
        return it->first <= l && r <= it->second;
    }

    void add(T l, T r){
        if(l >= r) return;
        auto it = intervals_.lower_bound(l);
        if(it != intervals_.begin()){
            auto previous = std::prev(it);
            if(previous->second >= l){
                l = std::min(l, previous->first);
                r = std::max(r, previous->second);
                it = intervals_.erase(previous);
            }
        }
        while(it != intervals_.end() && it->first <= r){
            r = std::max(r, it->second);
            it = intervals_.erase(it);
        }
        intervals_[l] = r;
    }

    void erase(T l, T r){
        if(l >= r) return;
        auto it = intervals_.lower_bound(l);
        if(it != intervals_.begin()) --it;
        while(it != intervals_.end()){
            T a = it->first;
            T b = it->second;
            if(b <= l){
                ++it;
                continue;
            }
            if(r <= a) break;
            it = intervals_.erase(it);
            if(a < l) intervals_[a] = l;
            if(r < b){
                intervals_[r] = b;
                break;
            }
        }
    }

    T mex(T x) const{
        auto it = intervals_.upper_bound(x);
        if(it == intervals_.begin()) return x;
        --it;
        if(!(it->first <= x && x < it->second)) return x;
        return it->second;
    }
};

template<auto Monoid, class Coordinate = long long>
class WeightedIntervalSet{
public:
    using monoid_type = std::remove_cvref_t<decltype(Monoid)>;
    using S = typename monoid_type::S;

    struct Interval{
        Coordinate right;
        S value;
    };

    struct Entry{
        Coordinate left;
        Coordinate right;
        S value;
    };

    using map_type = std::map<Coordinate, Interval>;

private:
    map_type intervals_;

    using iterator = typename map_type::iterator;
    using const_iterator = typename map_type::const_iterator;

    iterator split(Coordinate x){
        auto upper = intervals_.upper_bound(x);
        if(upper == intervals_.begin()) return intervals_.lower_bound(x);
        auto current = std::prev(upper);
        if(current->first == x) return current;
        if(!(x < current->second.right)) return intervals_.lower_bound(x);

        Coordinate right = current->second.right;
        S value = current->second.value;
        current->second.right = x;
        return intervals_.emplace(
            x,
            Interval{right, std::move(value)}
        ).first;
    }

    iterator merge_around(iterator current){
        if(current != intervals_.begin()){
            auto previous = std::prev(current);
            if(
                previous->second.right == current->first
                && previous->second.value == current->second.value
            ){
                previous->second.right = current->second.right;
                intervals_.erase(current);
                current = previous;
            }
        }

        auto next = std::next(current);
        if(
            next != intervals_.end()
            && current->second.right == next->first
            && current->second.value == next->second.value
        ){
            current->second.right = next->second.right;
            intervals_.erase(next);
        }
        return current;
    }

    const_iterator first_intersecting(Coordinate left) const{
        auto current = intervals_.lower_bound(left);
        if(current != intervals_.begin()){
            auto previous = std::prev(current);
            if(left < previous->second.right) return previous;
        }
        return current;
    }

public:
    WeightedIntervalSet() = default;

    bool empty() const{
        return intervals_.empty();
    }

    int size() const{
        return static_cast<int>(intervals_.size());
    }

    void clear(){
        intervals_.clear();
    }

    const map_type& intervals() const{
        return intervals_;
    }

    bool contains(Coordinate x) const{
        auto current = intervals_.upper_bound(x);
        if(current == intervals_.begin()) return false;
        --current;
        return x < current->second.right;
    }

    bool covered(Coordinate left, Coordinate right) const{
        if(!(left < right)) return true;
        auto current = first_intersecting(left);
        if(
            current == intervals_.end()
            || left < current->first
            || !(left < current->second.right)
        ) return false;

        Coordinate covered_right = current->second.right;
        while(covered_right < right){
            ++current;
            if(
                current == intervals_.end()
                || !(current->first == covered_right)
            ) return false;
            covered_right = current->second.right;
        }
        return true;
    }

    Coordinate mex(Coordinate x) const{
        auto current = first_intersecting(x);
        if(
            current == intervals_.end()
            || x < current->first
            || !(x < current->second.right)
        ) return x;

        Coordinate result = current->second.right;
        ++current;
        while(
            current != intervals_.end()
            && current->first == result
        ){
            result = current->second.right;
            ++current;
        }
        return result;
    }

    void set(Coordinate left, Coordinate right, S value){
        if(!(left < right)) return;
        split(left);
        split(right);
        intervals_.erase(
            intervals_.lower_bound(left),
            intervals_.lower_bound(right)
        );
        auto current = intervals_.emplace(
            left,
            Interval{right, std::move(value)}
        ).first;
        merge_around(current);
    }

    void assign(Coordinate left, Coordinate right, S value){
        set(left, right, std::move(value));
    }

    void erase(Coordinate left, Coordinate right){
        if(!(left < right)) return;
        split(left);
        split(right);
        intervals_.erase(
            intervals_.lower_bound(left),
            intervals_.lower_bound(right)
        );
    }

    std::optional<S> get(Coordinate x) const{
        auto current = intervals_.upper_bound(x);
        if(current == intervals_.begin()) return std::nullopt;
        --current;
        if(!(x < current->second.right)) return std::nullopt;
        return current->second.value;
    }

    std::optional<Entry> interval(Coordinate x) const{
        auto current = intervals_.upper_bound(x);
        if(current == intervals_.begin()) return std::nullopt;
        --current;
        if(!(x < current->second.right)) return std::nullopt;
        return Entry{
            current->first,
            current->second.right,
            current->second.value
        };
    }

    std::vector<Entry> enumerate() const{
        std::vector<Entry> result;
        result.reserve(intervals_.size());
        for(const auto& [left, interval] : intervals_){
            result.push_back({left, interval.right, interval.value});
        }
        return result;
    }

    std::vector<Entry> enumerate(
        Coordinate left,
        Coordinate right
    ) const{
        std::vector<Entry> result;
        if(!(left < right)) return result;
        for(
            auto current = first_intersecting(left);
            current != intervals_.end() && current->first < right;
            ++current
        ){
            if(left < current->second.right){
                result.push_back({
                    current->first,
                    current->second.right,
                    current->second.value
                });
            }
        }
        return result;
    }

    S prod(Coordinate left, Coordinate right) const{
        S result = Monoid.e();
        if(!(left < right)) return result;
        for(
            auto current = first_intersecting(left);
            current != intervals_.end() && current->first < right;
            ++current
        ){
            if(left < current->second.right){
                result = Monoid.op(result, current->second.value);
            }
        }
        return result;
    }

    S all_prod() const{
        S result = Monoid.e();
        for(const auto& [left, interval] : intervals_){
            (void)left;
            result = Monoid.op(result, interval.value);
        }
        return result;
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_RANGE_INTERVAL_SET_HPP_INCLUDED
