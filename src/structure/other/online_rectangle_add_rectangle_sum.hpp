#ifndef CPPLIB_SRC_STRUCTURE_OTHER_ONLINE_RECTANGLE_ADD_RECTANGLE_SUM_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_OTHER_ONLINE_RECTANGLE_ADD_RECTANGLE_SUM_HPP_INCLUDED

#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <unordered_map>

template<class T>
struct OnlineRectangleAddRectangleSum{
    using coordinate_type = std::int64_t;

private:
    struct IndexPair{
        std::uint64_t first;
        std::uint64_t second;

        bool operator==(const IndexPair&) const = default;
    };

    struct IndexPairHash{
        static std::uint64_t mix(std::uint64_t value){
            value += 0x9e3779b97f4a7c15ULL;
            value = (value ^ (value >> 30)) * 0xbf58476d1ce4e5b9ULL;
            value = (value ^ (value >> 27)) * 0x94d049bb133111ebULL;
            return value ^ (value >> 31);
        }

        std::size_t operator()(const IndexPair& key) const{
            const std::uint64_t first_hash = mix(key.first);
            const std::uint64_t second_hash = mix(key.second);
            return static_cast<std::size_t>(
                first_hash ^ (second_hash + 0x9e3779b97f4a7c15ULL +
                              (first_hash << 6) + (first_hash >> 2))
            );
        }
    };

    struct Coefficients{
        T constant{};
        T x_coefficient{};
        T y_coefficient{};
        T xy_coefficient{};
    };

    coordinate_type lower_bound;
    coordinate_type upper_bound;
    std::uint64_t coordinate_count;
    std::unordered_map<IndexPair, Coefficients, IndexPairHash> fenwick;

    static std::uint64_t lowbit(std::uint64_t value){
        return value & (~value + 1);
    }

    static T scale(const T& value, coordinate_type multiplier){
        return value * multiplier;
    }

    std::uint64_t distance_from_lower(coordinate_type coordinate) const{
        return static_cast<std::uint64_t>(
            static_cast<__int128_t>(coordinate) - lower_bound
        );
    }

    void check_coordinate(coordinate_type coordinate, const char* message) const{
        if(coordinate < lower_bound || upper_bound < coordinate)[[unlikely]]{
            throw std::out_of_range(message);
        }
    }

    void check_rectangle(
        coordinate_type x_left,
        coordinate_type x_right,
        coordinate_type y_lower,
        coordinate_type y_upper,
        const char* message
    ) const{
        check_coordinate(x_left, message);
        check_coordinate(x_right, message);
        check_coordinate(y_lower, message);
        check_coordinate(y_upper, message);
        if(x_right < x_left || y_upper < y_lower)[[unlikely]]{
            throw std::invalid_argument(message);
        }
    }

    void add_fenwick_point(
        std::uint64_t x_index,
        std::uint64_t y_index,
        const Coefficients& value
    ){
        for(std::uint64_t x = x_index; x <= coordinate_count;){
            for(std::uint64_t y = y_index; y <= coordinate_count;){
                auto [iterator, inserted] = fenwick.try_emplace(
                    IndexPair{x, y},
                    Coefficients{}
                );
                (void) inserted;
                Coefficients& current = iterator->second;
                current.constant = current.constant + value.constant;
                current.x_coefficient =
                    current.x_coefficient + value.x_coefficient;
                current.y_coefficient =
                    current.y_coefficient + value.y_coefficient;
                current.xy_coefficient =
                    current.xy_coefficient + value.xy_coefficient;

                const std::uint64_t step = lowbit(y);
                if(step > coordinate_count - y) break;
                y += step;
            }
            const std::uint64_t step = lowbit(x);
            if(step > coordinate_count - x) break;
            x += step;
        }
    }

    void add_corner(
        coordinate_type x,
        coordinate_type y,
        const T& value
    ){
        if(x == upper_bound || y == upper_bound) return;
        Coefficients coefficients;
        coefficients.constant = value;
        const coordinate_type normalized_x = static_cast<coordinate_type>(
            distance_from_lower(x)
        );
        const coordinate_type normalized_y = static_cast<coordinate_type>(
            distance_from_lower(y)
        );
        coefficients.x_coefficient = scale(value, normalized_x);
        coefficients.y_coefficient = scale(value, normalized_y);
        coefficients.xy_coefficient =
            scale(coefficients.x_coefficient, normalized_y);
        add_fenwick_point(
            distance_from_lower(x) + 1,
            distance_from_lower(y) + 1,
            coefficients
        );
    }

    Coefficients prefix_coefficients(
        coordinate_type x_bound,
        coordinate_type y_bound
    ) const{
        Coefficients result;
        for(std::uint64_t x = distance_from_lower(x_bound); x > 0; x -= lowbit(x)){
            for(std::uint64_t y = distance_from_lower(y_bound);
                y > 0;
                y -= lowbit(y)){
                const auto iterator = fenwick.find(IndexPair{x, y});
                if(iterator == fenwick.end()) continue;
                const Coefficients& current = iterator->second;
                result.constant = result.constant + current.constant;
                result.x_coefficient =
                    result.x_coefficient + current.x_coefficient;
                result.y_coefficient =
                    result.y_coefficient + current.y_coefficient;
                result.xy_coefficient =
                    result.xy_coefficient + current.xy_coefficient;
            }
        }
        return result;
    }

    T prefix_sum(coordinate_type x_bound, coordinate_type y_bound) const{
        const Coefficients coefficients =
            prefix_coefficients(x_bound, y_bound);
        const coordinate_type normalized_x = static_cast<coordinate_type>(
            distance_from_lower(x_bound)
        );
        const coordinate_type normalized_y = static_cast<coordinate_type>(
            distance_from_lower(y_bound)
        );
        return scale(scale(coefficients.constant, normalized_x), normalized_y)
            - scale(coefficients.x_coefficient, normalized_y)
            - scale(coefficients.y_coefficient, normalized_x)
            + coefficients.xy_coefficient;
    }

public:
    OnlineRectangleAddRectangleSum(
        coordinate_type coordinate_lower_bound,
        coordinate_type coordinate_upper_bound
    ):
        lower_bound(coordinate_lower_bound),
        upper_bound(coordinate_upper_bound),
        coordinate_count(0){
        const __int128_t width = static_cast<__int128_t>(upper_bound) - lower_bound;
        if(upper_bound <= lower_bound || width > std::numeric_limits<coordinate_type>::max())[[unlikely]]{
            throw std::invalid_argument(
                "OnlineRectangleAddRectangleSum: invalid coordinate bounds"
            );
        }
        coordinate_count = static_cast<std::uint64_t>(width);
    }

    coordinate_type coordinate_low() const{ return lower_bound; }
    coordinate_type coordinate_high() const{ return upper_bound; }

    void reserve(std::size_t expected_fenwick_cells){
        fenwick.reserve(expected_fenwick_cells);
    }

    void clear(){ fenwick.clear(); }

    std::size_t stored_node_count() const{ return fenwick.size(); }

    void add_rectangle(
        coordinate_type x_left,
        coordinate_type x_right,
        coordinate_type y_lower,
        coordinate_type y_upper,
        const T& value
    ){
        check_rectangle(
            x_left,
            x_right,
            y_lower,
            y_upper,
            "OnlineRectangleAddRectangleSum: rectangle out of range"
        );
        if(x_left == x_right || y_lower == y_upper) return;
        const T negative_value = T{} - value;
        add_corner(x_left, y_lower, value);
        add_corner(x_right, y_lower, negative_value);
        add_corner(x_left, y_upper, negative_value);
        add_corner(x_right, y_upper, value);
    }

    T rectangle_sum(
        coordinate_type x_left,
        coordinate_type x_right,
        coordinate_type y_lower,
        coordinate_type y_upper
    ) const{
        check_rectangle(
            x_left,
            x_right,
            y_lower,
            y_upper,
            "OnlineRectangleAddRectangleSum: rectangle out of range"
        );
        if(x_left == x_right || y_lower == y_upper) return T{};
        return prefix_sum(x_right, y_upper)
            - prefix_sum(x_left, y_upper)
            - prefix_sum(x_right, y_lower)
            + prefix_sum(x_left, y_lower);
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_OTHER_ONLINE_RECTANGLE_ADD_RECTANGLE_SUM_HPP_INCLUDED
