#ifndef CPPLIB_SRC_STRUCTURE_SEGTREE_SEGTREE_2D_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_SEGTREE_SEGTREE_2D_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <stdexcept>
#include <type_traits>
#include <vector>

namespace segtree_2d_internal{

consteval int ceil_power_of_two(int value){
    int result = 1;
    while(result < value) result <<= 1;
    return result;
}

} // namespace segtree_2d_internal

template<auto Monoid, int MAX_HEIGHT, int MAX_WIDTH>
struct Segtree2D{
    static_assert(MAX_HEIGHT >= 0);
    static_assert(MAX_WIDTH >= 0);

    using monoid_type = std::remove_cvref_t<decltype(Monoid)>;
    using S = typename monoid_type::S;

private:
    static constexpr int ROW_SIZE =
        segtree_2d_internal::ceil_power_of_two(MAX_HEIGHT);
    static constexpr int COL_SIZE =
        segtree_2d_internal::ceil_power_of_two(MAX_WIDTH);
    static constexpr int COL_NODES = 2 * COL_SIZE;
    static constexpr std::size_t NODE_COUNT =
        static_cast<std::size_t>(2 * ROW_SIZE) * COL_NODES;

    int _height = 0;
    int _width = 0;
    std::array<S, NODE_COUNT> data;

    static constexpr std::size_t index(int row_node, int col_node){
        return static_cast<std::size_t>(row_node) * COL_NODES + col_node;
    }

    void check_shape(int height, int width, const char* message) const{
        if(height < 0 || MAX_HEIGHT < height || width < 0 || MAX_WIDTH < width)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }

    void check_point(int row, int col, const char* message) const{
        if(row < 0 || _height <= row || col < 0 || _width <= col)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }

    void check_rectangle(
        int row_begin,
        int col_begin,
        int row_end,
        int col_end,
        const char* message
    ) const{
        if(row_begin < 0 || row_end < row_begin || _height < row_end ||
            col_begin < 0 || col_end < col_begin || _width < col_end)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }

    S column_prod(int row_node, int col_begin, int col_end) const{
        S left_result = Monoid.e();
        S right_result = Monoid.e();
        int left = col_begin + COL_SIZE;
        int right = col_end + COL_SIZE;
        while(left < right){
            if(left & 1){
                left_result = Monoid.op(left_result, data[index(row_node, left++)]);
            }
            if(right & 1){
                right_result = Monoid.op(data[index(row_node, --right)], right_result);
            }
            left >>= 1;
            right >>= 1;
        }
        return Monoid.op(left_result, right_result);
    }

public:
    Segtree2D(): Segtree2D(0, 0){}

    Segtree2D(int height, int width): _height(height), _width(width){
        check_shape(
            height,
            width,
            "library assertion fault: range violation (Segtree2D constructor)."
        );
        data.fill(Monoid.e());
    }

    explicit Segtree2D(const std::vector<std::vector<S>>& values): Segtree2D(){
        build(values);
    }

    void build(const std::vector<std::vector<S>>& values){
        const int height = static_cast<int>(values.size());
        const int width = height == 0 ? 0 : static_cast<int>(values[0].size());
        check_shape(height, width, "library assertion fault: range violation (Segtree2D::build).");
        for(const auto& row: values){
            if(static_cast<int>(row.size()) != width)[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: rectangular violation (Segtree2D::build)."
                );
            }
        }

        _height = height;
        _width = width;
        data.fill(Monoid.e());
        for(int row = 0; row < height; row++){
            const int row_node = row + ROW_SIZE;
            for(int col = 0; col < width; col++){
                data[index(row_node, col + COL_SIZE)] =
                    values[static_cast<std::size_t>(row)][static_cast<std::size_t>(col)];
            }
            for(int col_node = COL_SIZE - 1; col_node > 0; col_node--){
                data[index(row_node, col_node)] = Monoid.op(
                    data[index(row_node, col_node << 1)],
                    data[index(row_node, col_node << 1 | 1)]
                );
            }
        }
        for(int row_node = ROW_SIZE - 1; row_node > 0; row_node--){
            for(int col_node = 1; col_node < COL_NODES; col_node++){
                data[index(row_node, col_node)] = Monoid.op(
                    data[index(row_node << 1, col_node)],
                    data[index(row_node << 1 | 1, col_node)]
                );
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

    S get(int row, int col) const{
        check_point(row, col, "library assertion fault: range violation (Segtree2D::get).");
        return data[index(row + ROW_SIZE, col + COL_SIZE)];
    }

    void set(int row, int col, const S& value){
        check_point(row, col, "library assertion fault: range violation (Segtree2D::set).");
        const int leaf_row = row + ROW_SIZE;
        const int leaf_col = col + COL_SIZE;
        data[index(leaf_row, leaf_col)] = value;
        for(int col_node = leaf_col >> 1; col_node > 0; col_node >>= 1){
            data[index(leaf_row, col_node)] = Monoid.op(
                data[index(leaf_row, col_node << 1)],
                data[index(leaf_row, col_node << 1 | 1)]
            );
        }
        for(int row_node = leaf_row >> 1; row_node > 0; row_node >>= 1){
            data[index(row_node, leaf_col)] = Monoid.op(
                data[index(row_node << 1, leaf_col)],
                data[index(row_node << 1 | 1, leaf_col)]
            );
            for(int col_node = leaf_col >> 1; col_node > 0; col_node >>= 1){
                data[index(row_node, col_node)] = Monoid.op(
                    data[index(row_node, col_node << 1)],
                    data[index(row_node, col_node << 1 | 1)]
                );
            }
        }
    }

    void apply(int row, int col, const S& value){
        set(row, col, Monoid.op(get(row, col), value));
    }

    S prod(int row_begin, int col_begin, int row_end, int col_end) const{
        check_rectangle(
            row_begin,
            col_begin,
            row_end,
            col_end,
            "library assertion fault: range violation (Segtree2D::prod)."
        );
        S result = Monoid.e();
        int top = row_begin + ROW_SIZE;
        int bottom = row_end + ROW_SIZE;
        while(top < bottom){
            if(top & 1){
                result = Monoid.op(result, column_prod(top++, col_begin, col_end));
            }
            if(bottom & 1){
                result = Monoid.op(result, column_prod(--bottom, col_begin, col_end));
            }
            top >>= 1;
            bottom >>= 1;
        }
        return result;
    }

    S all_prod() const{
        return data[index(1, 1)];
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_SEGTREE_SEGTREE_2D_HPP_INCLUDED
