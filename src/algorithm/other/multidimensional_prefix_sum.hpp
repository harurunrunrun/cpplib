#ifndef CPPLIB_SRC_ALGORITHM_OTHER_MULTIDIMENSIONAL_PREFIX_SUM_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_MULTIDIMENSIONAL_PREFIX_SUM_HPP_INCLUDED

#include <array>
#include <bit>
#include <cstddef>
#include <limits>
#include <stdexcept>

template<class T, std::size_t Dimension, std::size_t MAX_STORAGE_SIZE>
class MultidimensionalPrefixSum{
    static_assert(Dimension > 0);
    static_assert(Dimension < std::numeric_limits<std::size_t>::digits);
    static_assert(MAX_STORAGE_SIZE > 0);

public:
    using coordinate_type = std::array<std::size_t, Dimension>;

private:
    coordinate_type extents{};
    coordinate_type padded_extents{};
    coordinate_type strides{};
    std::size_t cell_count = 0;
    std::size_t used_storage = 0;
    bool prefix_built = false;
    std::array<T, MAX_STORAGE_SIZE> data{};

    static std::size_t checked_product(
        std::size_t left,
        std::size_t right,
        std::size_t limit,
        const char* message
    ){
        if(right != 0 && left > limit / right)[[unlikely]]{
            throw std::runtime_error(message);
        }
        return left * right;
    }

    void check_cell(const coordinate_type& coordinate, const char* message) const{
        for(std::size_t axis = 0; axis < Dimension; ++axis){
            if(coordinate[axis] >= extents[axis])[[unlikely]]{
                throw std::runtime_error(message);
            }
        }
    }

    void check_corner(const coordinate_type& coordinate, const char* message) const{
        for(std::size_t axis = 0; axis < Dimension; ++axis){
            if(coordinate[axis] > extents[axis])[[unlikely]]{
                throw std::runtime_error(message);
            }
        }
    }

    void check_mutable(const char* message) const{
        if(prefix_built)[[unlikely]] throw std::runtime_error(message);
    }

    void check_built(const char* message) const{
        if(!prefix_built)[[unlikely]] throw std::runtime_error(message);
    }

    std::size_t flatten_corner(const coordinate_type& coordinate) const noexcept{
        std::size_t result = 0;
        for(std::size_t axis = 0; axis < Dimension; ++axis){
            result += coordinate[axis] * strides[axis];
        }
        return result;
    }

    std::size_t flatten_cell(const coordinate_type& coordinate) const noexcept{
        std::size_t result = 0;
        for(std::size_t axis = 0; axis < Dimension; ++axis){
            result += (coordinate[axis] + 1) * strides[axis];
        }
        return result;
    }

public:
    explicit MultidimensionalPrefixSum(const coordinate_type& shape): extents(shape){
        std::size_t storage_product = 1;
        std::size_t value_product = 1;
        for(std::size_t axis = Dimension; axis-- > 0;){
            if(extents[axis] == std::numeric_limits<std::size_t>::max())[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: capacity overflow (MultidimensionalPrefixSum)."
                );
            }
            padded_extents[axis] = extents[axis] + 1;
            strides[axis] = storage_product;
            storage_product = checked_product(
                storage_product,
                padded_extents[axis],
                MAX_STORAGE_SIZE,
                "library assertion fault: capacity violation (MultidimensionalPrefixSum)."
            );
            if(value_product != 0){
                value_product = checked_product(
                    value_product,
                    extents[axis],
                    std::numeric_limits<std::size_t>::max(),
                    "library assertion fault: capacity overflow (MultidimensionalPrefixSum)."
                );
            }
        }
        used_storage = storage_product;
        cell_count = value_product;
    }

    const coordinate_type& shape() const noexcept{ return extents; }
    std::size_t size() const noexcept{ return cell_count; }
    bool empty() const noexcept{ return cell_count == 0; }
    bool built() const noexcept{ return prefix_built; }
    std::size_t storage_size() const noexcept{ return used_storage; }
    static constexpr std::size_t capacity() noexcept{ return MAX_STORAGE_SIZE; }

    void set(const coordinate_type& coordinate, const T& value){
        check_mutable(
            "library assertion fault: build-state violation (MultidimensionalPrefixSum::set)."
        );
        check_cell(
            coordinate,
            "library assertion fault: range violation (MultidimensionalPrefixSum::set)."
        );
        data[flatten_cell(coordinate)] = value;
    }

    void add(const coordinate_type& coordinate, const T& value){
        check_mutable(
            "library assertion fault: build-state violation (MultidimensionalPrefixSum::add)."
        );
        check_cell(
            coordinate,
            "library assertion fault: range violation (MultidimensionalPrefixSum::add)."
        );
        data[flatten_cell(coordinate)] += value;
    }

    void build(){
        check_mutable(
            "library assertion fault: build-state violation (MultidimensionalPrefixSum::build)."
        );
        for(std::size_t axis = 0; axis < Dimension; ++axis){
            const std::size_t stride = strides[axis];
            const std::size_t extent = padded_extents[axis];
            const std::size_t block_size = stride * extent;
            for(std::size_t block_begin = 0; block_begin < used_storage;
                block_begin += block_size){
                const std::size_t block_end = block_begin + block_size;
                for(std::size_t offset = 0; offset < stride; ++offset){
                    for(std::size_t index = block_begin + stride + offset;
                        index < block_end; index += stride){
                        data[index] += data[index - stride];
                    }
                }
            }
        }
        prefix_built = true;
    }

    T get(const coordinate_type& coordinate) const{
        check_cell(
            coordinate,
            "library assertion fault: range violation (MultidimensionalPrefixSum::get)."
        );
        if(!prefix_built) return data[flatten_cell(coordinate)];
        coordinate_type end = coordinate;
        for(std::size_t axis = 0; axis < Dimension; ++axis) ++end[axis];
        return range_sum(coordinate, end);
    }

    T prefix_sum(const coordinate_type& end) const{
        check_built(
            "library assertion fault: build-state violation (MultidimensionalPrefixSum::prefix_sum)."
        );
        check_corner(
            end,
            "library assertion fault: range violation (MultidimensionalPrefixSum::prefix_sum)."
        );
        return data[flatten_corner(end)];
    }

    T range_sum(const coordinate_type& begin, const coordinate_type& end) const{
        check_built(
            "library assertion fault: build-state violation (MultidimensionalPrefixSum::range_sum)."
        );
        check_corner(
            begin,
            "library assertion fault: range violation (MultidimensionalPrefixSum::range_sum)."
        );
        check_corner(
            end,
            "library assertion fault: range violation (MultidimensionalPrefixSum::range_sum)."
        );
        bool is_empty = false;
        for(std::size_t axis = 0; axis < Dimension; ++axis){
            if(begin[axis] > end[axis])[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: range violation (MultidimensionalPrefixSum::range_sum)."
                );
            }
            is_empty = is_empty || begin[axis] == end[axis];
        }
        if(is_empty) return T{};

        constexpr std::size_t corner_count = std::size_t{1} << Dimension;
        std::size_t index = flatten_corner(end);
        std::size_t previous_gray = 0;
        bool subtract = false;
        T result{};
        for(std::size_t step = 0; step < corner_count; ++step){
            if(step != 0){
                const std::size_t gray = step ^ (step >> 1);
                const std::size_t changed = gray ^ previous_gray;
                const std::size_t axis = std::countr_zero(changed);
                if(axis >= Dimension)[[unlikely]]{
                    throw std::runtime_error(
                        "library assertion fault: internal Gray-code violation "
                        "(MultidimensionalPrefixSum::range_sum)."
                    );
                }
                const std::size_t delta =
                    (end[axis] - begin[axis]) * strides[axis];
                if((gray & changed) != 0) index -= delta;
                else index += delta;
                previous_gray = gray;
                subtract = !subtract;
            }
            if(subtract) result -= data[index];
            else result += data[index];
        }
        return result;
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_MULTIDIMENSIONAL_PREFIX_SUM_HPP_INCLUDED
