#ifndef CPPLIB_SRC_STRUCTURE_TREE_CENTROID_TREE_CONTOUR_FENWICK_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_TREE_CENTROID_TREE_CONTOUR_FENWICK_HPP_INCLUDED
#include "../../array/static_or_vector.hpp"

#include <algorithm>
#include <tuple>
#include <vector>
#include "centroid_distance_index.hpp"

// RangeAdd=false: point increments and distance-interval sums.
// RangeAdd=true: distance-interval increments and point values.
template<class T, class Distance, int MAX_SIZE = -1, bool RangeAdd = false>
class TreeContourFenwick{
    struct Bucket{
        std::vector<Distance> coordinates;
        std::vector<T> bit;
        void build(){
            std::sort(coordinates.begin(), coordinates.end());
            coordinates.erase(std::unique(coordinates.begin(), coordinates.end()), coordinates.end());
            bit.resize(coordinates.size() + 1);
        }
        int lower(Distance x) const{ return std::lower_bound(coordinates.begin(), coordinates.end(), x) - coordinates.begin(); }
        void add(int p, const T& x){ for(++p; p < cpplib_detail::checked_int_size(bit.size()); p += p & -p) bit[p] += x; }
        T prefix(int p) const{ T s{}; for(; p; p -= p & -p) s += bit[p]; return s; }
        T interval(Distance l, Distance r) const{ return prefix(lower(r)) - prefix(lower(l)); }
        void range_add(Distance l, Distance r, const T& x){ add(lower(l), x); add(lower(r), -x); }
        T point(Distance x) const{ return prefix(std::upper_bound(coordinates.begin(), coordinates.end(), x) - coordinates.begin()); }
    };
    CentroidDistanceIndex<Distance, MAX_SIZE> index;
    std::vector<Bucket> buckets;
    std::vector<T> initial;
public:
    TreeContourFenwick(const std::vector<T>& values,
                      const std::vector<std::tuple<int, int, Distance>>& edges):
        index(cpplib_detail::checked_int_size(values.size())), initial(values){
        for(auto [u, v, w]: edges) index.add_edge(u, v, w);
        index.build();
        buckets.resize(values.size() + index.branch_count());
        for(int v = 0; v < size(); ++v) for(const auto& e: index.entries(v)){
            buckets[e.centroid].coordinates.push_back(e.distance);
            if(e.branch != -1 && e.centroid != v) buckets[size() + e.branch].coordinates.push_back(e.distance);
        }
        for(auto& b: buckets) b.build();
        if constexpr(!RangeAdd) for(int v = 0; v < size(); ++v) point_add(v, values[v]);
    }
    int size() const{ return index.size(); }
    void point_add(int v, const T& x) requires(!RangeAdd){
        for(const auto& e: index.entries(v)){
            auto& a = buckets[e.centroid]; a.add(a.lower(e.distance), x);
            if(e.branch != -1 && e.centroid != v){ auto& b = buckets[size() + e.branch]; b.add(b.lower(e.distance), x); }
        }
    }
    T contour_sum(int v, Distance l, Distance r) const requires(!RangeAdd){
        if(l > r) throw std::invalid_argument("contour interval");
        T answer{};
        for(const auto& e: index.entries(v)){
            answer += buckets[e.centroid].interval(l - e.distance, r - e.distance);
            if(e.branch != -1 && e.centroid != v) answer -= buckets[size() + e.branch].interval(l - e.distance, r - e.distance);
        }
        return answer;
    }
    void contour_add(int v, Distance l, Distance r, const T& x) requires(RangeAdd){
        if(l > r) throw std::invalid_argument("contour interval");
        for(const auto& e: index.entries(v)){
            buckets[e.centroid].range_add(l - e.distance, r - e.distance, x);
            if(e.branch != -1 && e.centroid != v) buckets[size() + e.branch].range_add(l - e.distance, r - e.distance, -x);
        }
    }
    T point_get(int v) const requires(RangeAdd){
        T answer = initial.at(v);
        for(const auto& e: index.entries(v)){
            answer += buckets[e.centroid].point(e.distance);
            if(e.branch != -1 && e.centroid != v) answer += buckets[size() + e.branch].point(e.distance);
        }
        return answer;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_TREE_CENTROID_TREE_CONTOUR_FENWICK_HPP_INCLUDED
