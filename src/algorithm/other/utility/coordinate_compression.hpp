#ifndef CPPLIB_SRC_ALGORITHM_OTHER_UTILITY_COORDINATE_COMPRESSION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_UTILITY_COORDINATE_COMPRESSION_HPP_INCLUDED

#include "../../../structure/array/static_or_vector.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <span>
#include <stdexcept>
#include <type_traits>
#include <vector>

template<class T, int MAX_SIZE = -1>
struct CoordinateCompression{
    static_assert(MAX_SIZE >= -1);

private:
    cpplib_detail::StaticOrVector<T, (MAX_SIZE < 0 ? -1 : static_cast<std::ptrdiff_t>(static_cast<std::size_t>(MAX_SIZE)))> coordinates{};
    std::size_t coordinate_count = 0;

public:
    CoordinateCompression() = default;

    explicit CoordinateCompression(const std::vector<T>& values){
        build(values);
    }

    void build(const std::vector<T>& values){
        if(values.size() > static_cast<std::size_t>(MAX_SIZE))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: capacity violation (CoordinateCompression::build)."
            );
        }
        std::vector<T> next = values;
        std::sort(next.begin(), next.end());
        next.erase(std::unique(next.begin(), next.end()), next.end());
        coordinates.resize(next.size());
        std::copy(next.begin(), next.end(), coordinates.begin());
        coordinate_count = next.size();
    }

    std::size_t size() const noexcept{
        return coordinate_count;
    }

    bool empty() const noexcept{
        return coordinate_count == 0;
    }

    auto values() const noexcept(!(MAX_SIZE < 0 && std::is_same_v<T, bool>)){
        if constexpr(MAX_SIZE < 0 && std::is_same_v<T, bool>)
            return std::vector<bool>(coordinates.begin(), coordinates.end());
        else return std::span<const T>(coordinates.data(), coordinate_count);
    }

    std::size_t lower_bound_index(const T& value) const{
        return static_cast<std::size_t>(std::lower_bound(
            coordinates.begin(),
            coordinates.begin() + static_cast<std::ptrdiff_t>(coordinate_count),
            value
        ) - coordinates.begin());
    }

    std::size_t upper_bound_index(const T& value) const{
        return static_cast<std::size_t>(std::upper_bound(
            coordinates.begin(),
            coordinates.begin() + static_cast<std::ptrdiff_t>(coordinate_count),
            value
        ) - coordinates.begin());
    }

    bool contains(const T& value) const{
        const std::size_t position = lower_bound_index(value);
        return position < coordinate_count && coordinates[position] == value;
    }

    std::size_t index(const T& value) const{
        const std::size_t position = lower_bound_index(value);
        if(position == coordinate_count || coordinates[position] != value)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: missing value (CoordinateCompression::index)."
            );
        }
        return position;
    }

    std::conditional_t<MAX_SIZE < 0 && std::is_same_v<T, bool>, bool, const T&>
    value(std::size_t index_) const{
        if(index_ >= coordinate_count)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (CoordinateCompression::value)."
            );
        }
        return coordinates[index_];
    }

    std::vector<std::size_t> compress(const std::vector<T>& source) const{
        std::vector<std::size_t> result;
        result.reserve(source.size());
        for(const T& item: source) result.push_back(index(item));
        return result;
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_UTILITY_COORDINATE_COMPRESSION_HPP_INCLUDED
