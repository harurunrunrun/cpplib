#ifndef CPPLIB_SRC_ALGORITHM_OTHER_COORDINATE_COMPRESSION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_COORDINATE_COMPRESSION_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cstddef>
#include <span>
#include <stdexcept>
#include <vector>

template<class T, int MAX_SIZE>
struct CoordinateCompression{
    static_assert(MAX_SIZE >= 0);

private:
    std::array<T, static_cast<std::size_t>(MAX_SIZE)> coordinates{};
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
        std::copy(next.begin(), next.end(), coordinates.begin());
        coordinate_count = next.size();
    }

    std::size_t size() const noexcept{
        return coordinate_count;
    }

    bool empty() const noexcept{
        return coordinate_count == 0;
    }

    std::span<const T> values() const noexcept{
        return {coordinates.data(), coordinate_count};
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

    const T& value(std::size_t index_) const{
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

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_COORDINATE_COMPRESSION_HPP_INCLUDED
