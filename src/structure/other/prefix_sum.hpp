#ifndef CPPLIB_SRC_STRUCTURE_OTHER_PREFIX_SUM_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_OTHER_PREFIX_SUM_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <stdexcept>
#include <vector>

template<class T, int MAX_SIZE>
struct PrefixSum{
    static_assert(MAX_SIZE >= 0);

private:
    int _size = 0;
    std::array<T, static_cast<std::size_t>(MAX_SIZE) + 1> prefix{};

    void check_size(int n, const char* message) const{
        if(n < 0 || MAX_SIZE < n)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }

    void check_prefix(int r, const char* message) const{
        if(r < 0 || _size < r)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }

public:
    PrefixSum() = default;

    explicit PrefixSum(int n): _size(n){
        check_size(n, "library assertion fault: range violation (PrefixSum constructor).");
    }

    explicit PrefixSum(const std::vector<T>& values){
        build(values);
    }

    void build(const std::vector<T>& values){
        const int n = static_cast<int>(values.size());
        check_size(n, "library assertion fault: range violation (PrefixSum::build).");
        _size = n;
        prefix[0] = T();
        for(int i = 0; i < n; i++){
            prefix[static_cast<std::size_t>(i + 1)] =
                prefix[static_cast<std::size_t>(i)] + values[static_cast<std::size_t>(i)];
        }
    }

    int size() const{
        return _size;
    }

    bool empty() const{
        return _size == 0;
    }

    T prefix_sum(int r) const{
        check_prefix(r, "library assertion fault: range violation (PrefixSum::prefix_sum).");
        return prefix[static_cast<std::size_t>(r)];
    }

    T sum(int l, int r) const{
        check_prefix(l, "library assertion fault: range violation (PrefixSum::sum).");
        check_prefix(r, "library assertion fault: range violation (PrefixSum::sum).");
        if(r < l)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (PrefixSum::sum).");
        }
        return prefix[static_cast<std::size_t>(r)] - prefix[static_cast<std::size_t>(l)];
    }

    T get(int index) const{
        if(index < 0 || _size <= index)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (PrefixSum::get).");
        }
        return sum(index, index + 1);
    }
};

template<class T, int MAX_HEIGHT, int MAX_WIDTH>
struct PrefixSum2D{
    static_assert(MAX_HEIGHT >= 0);
    static_assert(MAX_WIDTH >= 0);

private:
    int _height = 0;
    int _width = 0;
    std::array<
        T,
        (static_cast<std::size_t>(MAX_HEIGHT) + 1) *
            (static_cast<std::size_t>(MAX_WIDTH) + 1)
    > prefix{};

    static constexpr std::size_t index(int row, int col){
        return static_cast<std::size_t>(row) *
            (static_cast<std::size_t>(MAX_WIDTH) + 1) + col;
    }

    void check_shape(int height, int width, const char* message) const{
        if(height < 0 || MAX_HEIGHT < height || width < 0 || MAX_WIDTH < width)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }

    void check_corner(int row, int col, const char* message) const{
        if(row < 0 || _height < row || col < 0 || _width < col)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }

public:
    PrefixSum2D() = default;

    PrefixSum2D(int height, int width): _height(height), _width(width){
        check_shape(
            height,
            width,
            "library assertion fault: range violation (PrefixSum2D constructor)."
        );
    }

    explicit PrefixSum2D(const std::vector<std::vector<T>>& values){
        build(values);
    }

    void build(const std::vector<std::vector<T>>& values){
        const int height = static_cast<int>(values.size());
        const int width = height == 0 ? 0 : static_cast<int>(values[0].size());
        check_shape(
            height,
            width,
            "library assertion fault: range violation (PrefixSum2D::build)."
        );
        for(const auto& row: values){
            if(static_cast<int>(row.size()) != width)[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: rectangular violation (PrefixSum2D::build)."
                );
            }
        }

        _height = height;
        _width = width;
        for(int row = 0; row <= height; row++) prefix[index(row, 0)] = T();
        for(int col = 1; col <= width; col++) prefix[index(0, col)] = T();
        for(int row = 0; row < height; row++){
            for(int col = 0; col < width; col++){
                prefix[index(row + 1, col + 1)] =
                    values[static_cast<std::size_t>(row)][static_cast<std::size_t>(col)] +
                    prefix[index(row, col + 1)] + prefix[index(row + 1, col)] -
                    prefix[index(row, col)];
            }
        }
    }

    int height() const{
        return _height;
    }

    int width() const{
        return _width;
    }

    bool empty() const{
        return _height == 0 || _width == 0;
    }

    T sum(int row_begin, int col_begin, int row_end, int col_end) const{
        check_corner(
            row_begin,
            col_begin,
            "library assertion fault: range violation (PrefixSum2D::sum)."
        );
        check_corner(
            row_end,
            col_end,
            "library assertion fault: range violation (PrefixSum2D::sum)."
        );
        if(row_end < row_begin || col_end < col_begin)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (PrefixSum2D::sum).");
        }
        return prefix[index(row_end, col_end)] - prefix[index(row_begin, col_end)] -
            prefix[index(row_end, col_begin)] + prefix[index(row_begin, col_begin)];
    }

    T get(int row, int col) const{
        if(row < 0 || _height <= row || col < 0 || _width <= col)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (PrefixSum2D::get).");
        }
        return sum(row, col, row + 1, col + 1);
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_OTHER_PREFIX_SUM_HPP_INCLUDED
