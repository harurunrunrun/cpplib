#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <limits>
#include <span>
#include <stdexcept>
#include <type_traits>
#include <vector>

struct CoordinateNeighborPolicy{
    bool predecessor = false;
    bool successor = false;
};

template<class X, class Y, int MAX_X_SIZE, int MAX_Y_SIZE>
class PlaneCoordinateCompression{
    static_assert(std::is_integral_v<X> && !std::is_same_v<std::remove_cv_t<X>, bool>);
    static_assert(std::is_integral_v<Y> && !std::is_same_v<std::remove_cv_t<Y>, bool>);
    static_assert(MAX_X_SIZE >= 0);
    static_assert(MAX_Y_SIZE >= 0);

    std::array<X, static_cast<std::size_t>(MAX_X_SIZE)> compressed_x{};
    std::array<Y, static_cast<std::size_t>(MAX_Y_SIZE)> compressed_y{};
    std::size_t x_count = 0;
    std::size_t y_count = 0;

    template<class T>
    static std::vector<T> make_axis(
        const std::vector<T>& values,
        CoordinateNeighborPolicy neighbors,
        std::size_t capacity,
        const char* capacity_message
    ){
        std::vector<T> candidates;
        const std::size_t values_per_coordinate =
            1 + static_cast<std::size_t>(neighbors.predecessor) +
            static_cast<std::size_t>(neighbors.successor);
        if(values.size() <= candidates.max_size() / values_per_coordinate){
            candidates.reserve(values.size() * values_per_coordinate);
        }
        for(const T value: values){
            if(neighbors.predecessor && value != std::numeric_limits<T>::lowest()){
                candidates.push_back(static_cast<T>(value - T{1}));
            }
            candidates.push_back(value);
            if(neighbors.successor && value != std::numeric_limits<T>::max()){
                candidates.push_back(static_cast<T>(value + T{1}));
            }
        }
        std::sort(candidates.begin(), candidates.end());
        candidates.erase(
            std::unique(candidates.begin(), candidates.end()),
            candidates.end()
        );
        if(candidates.size() > capacity)[[unlikely]]{
            throw std::runtime_error(capacity_message);
        }
        return candidates;
    }

    template<class T, std::size_t MAX_SIZE>
    static std::size_t index_of(
        const std::array<T, MAX_SIZE>& values,
        std::size_t count,
        T value,
        const char* message
    ){
        if(count == 0)[[unlikely]]{
            throw std::runtime_error(message);
        }
        const auto begin = values.begin();
        const auto end = begin + static_cast<std::ptrdiff_t>(count);
        const auto iterator = std::lower_bound(begin, end, value);
        if(iterator == end || *iterator != value)[[unlikely]]{
            throw std::runtime_error(message);
        }
        return static_cast<std::size_t>(iterator - begin);
    }

public:
    PlaneCoordinateCompression() = default;

    PlaneCoordinateCompression(
        const std::vector<X>& x_values,
        const std::vector<Y>& y_values,
        CoordinateNeighborPolicy x_neighbors = {},
        CoordinateNeighborPolicy y_neighbors = {}
    ){
        build(x_values, y_values, x_neighbors, y_neighbors);
    }

    void build(
        const std::vector<X>& x_values,
        const std::vector<Y>& y_values,
        CoordinateNeighborPolicy x_neighbors = {},
        CoordinateNeighborPolicy y_neighbors = {}
    ){
        const auto next_x = make_axis(
            x_values,
            x_neighbors,
            static_cast<std::size_t>(MAX_X_SIZE),
            "library assertion fault: capacity violation (PlaneCoordinateCompression x-axis)."
        );
        const auto next_y = make_axis(
            y_values,
            y_neighbors,
            static_cast<std::size_t>(MAX_Y_SIZE),
            "library assertion fault: capacity violation (PlaneCoordinateCompression y-axis)."
        );

        if constexpr(MAX_X_SIZE > 0){
            std::copy(next_x.begin(), next_x.end(), compressed_x.begin());
        }
        if constexpr(MAX_Y_SIZE > 0){
            std::copy(next_y.begin(), next_y.end(), compressed_y.begin());
        }
        x_count = next_x.size();
        y_count = next_y.size();
    }

    std::size_t x_size() const noexcept{ return x_count; }
    std::size_t y_size() const noexcept{ return y_count; }

    std::array<std::size_t, 2> size() const noexcept{
        return {x_count, y_count};
    }

    bool empty() const noexcept{ return x_count == 0 && y_count == 0; }

    std::span<const X> x_values() const noexcept{
        return std::span<const X>(compressed_x.data(), x_count);
    }

    std::span<const Y> y_values() const noexcept{
        return std::span<const Y>(compressed_y.data(), y_count);
    }

    bool contains_x(X value) const noexcept{
        if(x_count == 0) return false;
        return std::binary_search(
            compressed_x.begin(),
            compressed_x.begin() + static_cast<std::ptrdiff_t>(x_count),
            value
        );
    }

    bool contains_y(Y value) const noexcept{
        if(y_count == 0) return false;
        return std::binary_search(
            compressed_y.begin(),
            compressed_y.begin() + static_cast<std::ptrdiff_t>(y_count),
            value
        );
    }

    std::size_t x_index(X value) const{
        return index_of(
            compressed_x,
            x_count,
            value,
            "library assertion fault: missing value (PlaneCoordinateCompression::x_index)."
        );
    }

    std::size_t y_index(Y value) const{
        return index_of(
            compressed_y,
            y_count,
            value,
            "library assertion fault: missing value (PlaneCoordinateCompression::y_index)."
        );
    }

    const X& x_value(std::size_t index) const{
        if(index >= x_count)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (PlaneCoordinateCompression::x_value)."
            );
        }
        return compressed_x[index];
    }

    const Y& y_value(std::size_t index) const{
        if(index >= y_count)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (PlaneCoordinateCompression::y_value)."
            );
        }
        return compressed_y[index];
    }
};
