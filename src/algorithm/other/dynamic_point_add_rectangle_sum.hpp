#pragma once

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

template<class T, class Coordinate = long long>
class DynamicPointAddRectangleSum{
    std::vector<Coordinate> xs;
    std::vector<std::pair<Coordinate, Coordinate>> reserved_points;
    std::vector<std::vector<Coordinate>> ys;
    std::vector<std::vector<T>> bit;
    bool built = false;

    void require_built() const{
        if(!built)[[unlikely]]{
            throw std::runtime_error("library assertion fault: DynamicPointAddRectangleSum is not built.");
        }
    }

    int x_index(Coordinate x) const{
        auto it = std::lower_bound(xs.begin(), xs.end(), x);
        if(it == xs.end() || *it != x)[[unlikely]]{
            throw std::runtime_error("library assertion fault: unknown coordinate (DynamicPointAddRectangleSum).");
        }
        return static_cast<int>(it - xs.begin()) + 1;
    }

public:
    DynamicPointAddRectangleSum() = default;

    void reserve_point(Coordinate x, Coordinate y){
        if(built)[[unlikely]]{
            throw std::runtime_error("library assertion fault: DynamicPointAddRectangleSum is already built.");
        }
        reserved_points.push_back({x, y});
    }

    void build(){
        if(built)[[unlikely]]{
            throw std::runtime_error("library assertion fault: DynamicPointAddRectangleSum is already built.");
        }
        std::sort(reserved_points.begin(), reserved_points.end());
        reserved_points.erase(
            std::unique(reserved_points.begin(), reserved_points.end()),
            reserved_points.end()
        );
        xs.reserve(reserved_points.size());
        for(auto [x, y]: reserved_points){
            (void)y;
            xs.push_back(x);
        }
        std::sort(xs.begin(), xs.end());
        xs.erase(std::unique(xs.begin(), xs.end()), xs.end());
        ys.assign(xs.size() + 1, {});
        for(auto [x, y]: reserved_points){
            int xi = static_cast<int>(std::lower_bound(xs.begin(), xs.end(), x) - xs.begin()) + 1;
            for(int i = xi; i < static_cast<int>(ys.size()); i += i & -i){
                ys[static_cast<std::size_t>(i)].push_back(y);
            }
        }
        bit.resize(ys.size());
        for(std::size_t i = 1; i < ys.size(); i++){
            auto& current = ys[i];
            std::sort(current.begin(), current.end());
            current.erase(std::unique(current.begin(), current.end()), current.end());
            bit[i].assign(current.size() + 1, T{});
        }
        built = true;
    }

    void add(Coordinate x, Coordinate y, const T& value){
        require_built();
        if(!std::binary_search(reserved_points.begin(), reserved_points.end(), std::pair{x, y}))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: unknown coordinate (DynamicPointAddRectangleSum)."
            );
        }
        int xi = x_index(x);
        for(int i = xi; i < static_cast<int>(ys.size()); i += i & -i){
            int yi = static_cast<int>(std::lower_bound(ys[static_cast<std::size_t>(i)].begin(), ys[static_cast<std::size_t>(i)].end(), y) - ys[static_cast<std::size_t>(i)].begin());
            if(yi == static_cast<int>(ys[static_cast<std::size_t>(i)].size()) || ys[static_cast<std::size_t>(i)][static_cast<std::size_t>(yi)] != y)[[unlikely]]{
                throw std::runtime_error("library assertion fault: unknown coordinate (DynamicPointAddRectangleSum).");
            }
            for(++yi; yi < static_cast<int>(bit[static_cast<std::size_t>(i)].size()); yi += yi & -yi){
                bit[static_cast<std::size_t>(i)][static_cast<std::size_t>(yi)] += value;
            }
        }
    }

    T prefix_sum_less(Coordinate x, Coordinate y) const{
        require_built();
        T result{};
        int xi = static_cast<int>(std::lower_bound(xs.begin(), xs.end(), x) - xs.begin());
        for(int i = xi; i > 0; i -= i & -i){
            int yi = static_cast<int>(std::lower_bound(ys[static_cast<std::size_t>(i)].begin(), ys[static_cast<std::size_t>(i)].end(), y) - ys[static_cast<std::size_t>(i)].begin());
            for(; yi > 0; yi -= yi & -yi){
                result += bit[static_cast<std::size_t>(i)][static_cast<std::size_t>(yi)];
            }
        }
        return result;
    }

    T prefix_sum_leq(Coordinate x, Coordinate y) const{
        require_built();
        T result{};
        int xi = static_cast<int>(std::upper_bound(xs.begin(), xs.end(), x) - xs.begin());
        for(int i = xi; i > 0; i -= i & -i){
            int yi = static_cast<int>(std::upper_bound(ys[static_cast<std::size_t>(i)].begin(), ys[static_cast<std::size_t>(i)].end(), y) - ys[static_cast<std::size_t>(i)].begin());
            for(; yi > 0; yi -= yi & -yi){
                result += bit[static_cast<std::size_t>(i)][static_cast<std::size_t>(yi)];
            }
        }
        return result;
    }

    T sum(Coordinate x1, Coordinate y1, Coordinate x2, Coordinate y2) const{
        return prefix_sum_less(x2, y2) - prefix_sum_less(x1, y2) - prefix_sum_less(x2, y1) + prefix_sum_less(x1, y1);
    }
};
