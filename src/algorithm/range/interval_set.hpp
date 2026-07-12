#pragma once

#include <algorithm>
#include <map>

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
