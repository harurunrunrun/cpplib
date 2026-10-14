#ifndef CPPLIB_SRC_STRUCTURE_RANGE_QUERY_EXTREMA_RANGE_LINEAR_ADD_MIN_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_RANGE_QUERY_EXTREMA_RANGE_LINEAR_ADD_MIN_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

class RangeLinearAddMin{
    struct Block{ int l, r; std::int64_t slope = 0, offset = 0; std::vector<int> hull; };
    std::vector<std::int64_t> values;
    std::vector<Block> blocks;
    int width;
    void rebuild(Block& b){
        b.hull.clear();
        for(int i = b.l; i < b.r; ++i){
            while(b.hull.size() >= 2){
                int a = b.hull[b.hull.size() - 2], c = b.hull.back();
                if(static_cast<__int128>(c - a) * (static_cast<__int128>(values[i]) - values[a])
                    >= static_cast<__int128>(i - a) * (static_cast<__int128>(values[c]) - values[a])) break;
                b.hull.pop_back();
            }
            b.hull.push_back(i);
        }
    }
    void push(Block& b){
        for(int i = b.l; i < b.r; ++i) values[i] += b.slope * i + b.offset;
        b.slope = b.offset = 0;
    }
    std::int64_t at(const Block& b, int i) const{ return values[i] + b.slope * i + b.offset; }
    void check(int l, int r) const{ if(l < 0 || l > r || r > size()) throw std::out_of_range("linear minimum interval"); }
public:
    explicit RangeLinearAddMin(const std::vector<std::int64_t>& a): values(a),
        width(std::max(1, static_cast<int>(std::sqrt(a.size())))){
        for(int l = 0; l < size(); l += width){
            blocks.push_back({l, std::min(size(), l + width), 0, 0, {}});
            rebuild(blocks.back());
        }
    }
    int size() const{ return static_cast<int>(values.size()); }
    void add(int l, int r, std::int64_t slope, std::int64_t offset){
        check(l, r);
        for(int id = l / width; id < static_cast<int>(blocks.size()) && blocks[id].l < r; ++id){
            auto& b = blocks[id];
            if(l <= b.l && b.r <= r){ b.slope += slope; b.offset += offset; }
            else{
                push(b);
                for(int i = std::max(l, b.l); i < std::min(r, b.r); ++i) values[i] += slope * i + offset;
                rebuild(b);
            }
        }
    }
    std::pair<std::int64_t,int> minimum_with_count(int l, int r) const{
        check(l, r);
        if(l == r) throw std::invalid_argument("empty linear minimum interval");
        std::int64_t answer = std::numeric_limits<std::int64_t>::max();
        int count=0;
        auto accept=[&](std::int64_t value,int multiplicity){
            if(value<answer){answer=value;count=multiplicity;}
            else if(value==answer)count+=multiplicity;
        };
        for(int id = l / width; id < static_cast<int>(blocks.size()) && blocks[id].l < r; ++id){
            const auto& b = blocks[id];
            if(l <= b.l && b.r <= r){
                int lo = 0, hi = static_cast<int>(b.hull.size()) - 1;
                while(lo < hi){ int mid = (lo + hi) / 2; if(at(b, b.hull[mid]) <= at(b, b.hull[mid + 1])) hi = mid; else lo = mid + 1; }
                const int first=lo;const auto value=at(b,b.hull[lo]);hi=static_cast<int>(b.hull.size());
                while(lo<hi){int mid=(lo+hi)/2;if(at(b,b.hull[mid])==value)lo=mid+1;else hi=mid;}
                accept(value,lo-first);
            }else for(int i = std::max(l, b.l); i < std::min(r, b.r); ++i) accept(at(b,i),1);
        }
        return {answer,count};
    }
    std::int64_t minimum(int l,int r) const{ return minimum_with_count(l,r).first; }
};

#endif  // CPPLIB_SRC_STRUCTURE_RANGE_QUERY_EXTREMA_RANGE_LINEAR_ADD_MIN_HPP_INCLUDED
