#ifndef CPPLIB_SRC_STRUCTURE_SEGTREE_PARTIALLY_PERSISTENT_SEGTREE_2D_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_SEGTREE_PARTIALLY_PERSISTENT_SEGTREE_2D_HPP_INCLUDED

#include <cstddef>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include "../other/partially_persistent_storage.hpp"

namespace partially_persistent_segtree_2d_internal{

consteval int ceil_power_of_two(int value){
    int result = 1;
    while(result < value) result <<= 1;
    return result;
}

consteval int tree_height(int value){
    int size = 1;
    int height = 0;
    while(size < value){
        size <<= 1;
        ++height;
    }
    return height;
}

} // namespace partially_persistent_segtree_2d_internal

template<auto Monoid, int MAX_HEIGHT, int MAX_WIDTH, int MAX_UPDATE>
class PartiallyPersistentSegtree2D{
    static_assert(MAX_HEIGHT > 0);
    static_assert(MAX_WIDTH > 0);
    static_assert(MAX_UPDATE >= 0);

public:
    using monoid_type = std::remove_cvref_t<decltype(Monoid)>;
    using S = typename monoid_type::S;

private:
    static constexpr int ROW_SIZE =
        partially_persistent_segtree_2d_internal::ceil_power_of_two(MAX_HEIGHT);
    static constexpr int COL_SIZE =
        partially_persistent_segtree_2d_internal::ceil_power_of_two(MAX_WIDTH);
    static constexpr int ROW_HEIGHT =
        partially_persistent_segtree_2d_internal::tree_height(MAX_HEIGHT);
    static constexpr int COL_HEIGHT =
        partially_persistent_segtree_2d_internal::tree_height(MAX_WIDTH);
    static constexpr int COL_NODES = 2 * COL_SIZE;
    static constexpr std::size_t NODE_COUNT_SIZE =
        static_cast<std::size_t>(2 * ROW_SIZE) * COL_NODES;
    static constexpr std::size_t CHANGES_PER_UPDATE =
        (static_cast<std::size_t>(ROW_HEIGHT) + 1) *
        (static_cast<std::size_t>(COL_HEIGHT) + 1);

    static_assert(
        NODE_COUNT_SIZE <=
        static_cast<std::size_t>(std::numeric_limits<int>::max())
    );
    static_assert(
        static_cast<std::size_t>(MAX_UPDATE) <=
        static_cast<std::size_t>(std::numeric_limits<int>::max()) /
        CHANGES_PER_UPDATE
    );

    static constexpr int NODE_COUNT = static_cast<int>(NODE_COUNT_SIZE);
    static constexpr int CHANGE_CAPACITY = static_cast<int>(
        static_cast<std::size_t>(MAX_UPDATE) * CHANGES_PER_UPDATE
    );

    int _height = 0;
    int _width = 0;
    int version_count = 1;
    PartiallyPersistentStorage<S, NODE_COUNT, CHANGE_CAPACITY> data;

    static constexpr int index(int row_node, int col_node){
        return row_node * COL_NODES + col_node;
    }

    void check_shape(int height, int width, const char* message) const{
        if(height < 0 || MAX_HEIGHT < height ||
            width < 0 || MAX_WIDTH < width)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }

    void check_version(int version, const char* message) const{
        if(version < 0 || version_count <= version)[[unlikely]]{
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

    const S& value_at(int version, int node) const{
        return version == latest_version() ?
            data.current(node) : data.get(node, version);
    }

    void initialize(int height, int width){
        check_shape(
            height,
            width,
            "library assertion fault: range violation (PartiallyPersistentSegtree2D constructor)."
        );
        _height = height;
        _width = width;
        for(int node = 0; node < NODE_COUNT; ++node){
            data.initialize(node, Monoid.e());
        }
    }

    void build_values(const std::vector<std::vector<S>>& values){
        for(int row = 0; row < _height; ++row){
            const int row_node = row + ROW_SIZE;
            for(int col = 0; col < _width; ++col){
                data.initialize(
                    index(row_node, col + COL_SIZE),
                    values[static_cast<std::size_t>(row)]
                          [static_cast<std::size_t>(col)]
                );
            }
            for(int col_node = COL_SIZE - 1; col_node > 0; --col_node){
                data.initialize(
                    index(row_node, col_node),
                    Monoid.op(
                        data.current(index(row_node, col_node << 1)),
                        data.current(index(row_node, col_node << 1 | 1))
                    )
                );
            }
        }
        for(int row_node = ROW_SIZE - 1; row_node > 0; --row_node){
            for(int col_node = 1; col_node < COL_NODES; ++col_node){
                data.initialize(
                    index(row_node, col_node),
                    Monoid.op(
                        data.current(index(row_node << 1, col_node)),
                        data.current(index(row_node << 1 | 1, col_node))
                    )
                );
            }
        }
    }

    S column_prod(
        int version,
        int row_node,
        int col_begin,
        int col_end
    ) const{
        S left_result = Monoid.e();
        S right_result = Monoid.e();
        int left = col_begin + COL_SIZE;
        int right = col_end + COL_SIZE;
        while(left < right){
            if(left & 1){
                left_result = Monoid.op(
                    left_result,
                    value_at(version, index(row_node, left++))
                );
            }
            if(right & 1){
                right_result = Monoid.op(
                    value_at(version, index(row_node, --right)),
                    right_result
                );
            }
            left >>= 1;
            right >>= 1;
        }
        return Monoid.op(left_result, right_result);
    }

public:
    PartiallyPersistentSegtree2D(const PartiallyPersistentSegtree2D&) = delete;
    PartiallyPersistentSegtree2D& operator=(const PartiallyPersistentSegtree2D&) = delete;
    PartiallyPersistentSegtree2D(PartiallyPersistentSegtree2D&&) = delete;
    PartiallyPersistentSegtree2D& operator=(PartiallyPersistentSegtree2D&&) = delete;

    PartiallyPersistentSegtree2D(){
        initialize(0, 0);
    }

    PartiallyPersistentSegtree2D(int height, int width){
        initialize(height, width);
    }

    explicit PartiallyPersistentSegtree2D(
        const std::vector<std::vector<S>>& values
    ){
        const int height = static_cast<int>(values.size());
        const int width = height == 0 ? 0 : static_cast<int>(values[0].size());
        check_shape(
            height,
            width,
            "library assertion fault: range violation (PartiallyPersistentSegtree2D constructor)."
        );
        for(const auto& row : values){
            if(static_cast<int>(row.size()) != width)[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: rectangular violation (PartiallyPersistentSegtree2D constructor)."
                );
            }
        }
        initialize(height, width);
        build_values(values);
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

    int versions() const{
        return version_count;
    }

    int latest_version() const{
        return version_count - 1;
    }

    int set(int row, int col, const S& value){
        check_point(
            row,
            col,
            "library assertion fault: range violation (set)."
        );
        if(version_count > MAX_UPDATE)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: version capacity exceeded (set)."
            );
        }

        const int snapshot = data.changes();
        try{
            const int leaf_row = row + ROW_SIZE;
            const int leaf_col = col + COL_SIZE;
            data.write(index(leaf_row, leaf_col), version_count, value);
            for(int col_node = leaf_col >> 1;
                col_node > 0;
                col_node >>= 1){
                data.write(
                    index(leaf_row, col_node),
                    version_count,
                    Monoid.op(
                        data.current(index(leaf_row, col_node << 1)),
                        data.current(index(leaf_row, col_node << 1 | 1))
                    )
                );
            }
            for(int row_node = leaf_row >> 1;
                row_node > 0;
                row_node >>= 1){
                data.write(
                    index(row_node, leaf_col),
                    version_count,
                    Monoid.op(
                        data.current(index(row_node << 1, leaf_col)),
                        data.current(index(row_node << 1 | 1, leaf_col))
                    )
                );
                for(int col_node = leaf_col >> 1;
                    col_node > 0;
                    col_node >>= 1){
                    data.write(
                        index(row_node, col_node),
                        version_count,
                        Monoid.op(
                            data.current(index(row_node, col_node << 1)),
                            data.current(index(row_node, col_node << 1 | 1))
                        )
                    );
                }
            }
        }catch(...){
            data.rollback(snapshot);
            throw;
        }
        return version_count++;
    }

    int apply(int row, int col, const S& value){
        return set(row, col, Monoid.op(get(row, col), value));
    }

    S get(int version, int row, int col) const{
        check_version(
            version,
            "library assertion fault: range violation (get)."
        );
        check_point(
            row,
            col,
            "library assertion fault: range violation (get)."
        );
        return value_at(
            version,
            index(row + ROW_SIZE, col + COL_SIZE)
        );
    }

    S get(int row, int col) const{
        return get(latest_version(), row, col);
    }

    S prod(
        int version,
        int row_begin,
        int col_begin,
        int row_end,
        int col_end
    ) const{
        check_version(
            version,
            "library assertion fault: range violation (prod)."
        );
        check_rectangle(
            row_begin,
            col_begin,
            row_end,
            col_end,
            "library assertion fault: range violation (prod)."
        );
        S result = Monoid.e();
        int top = row_begin + ROW_SIZE;
        int bottom = row_end + ROW_SIZE;
        while(top < bottom){
            if(top & 1){
                result = Monoid.op(
                    result,
                    column_prod(version, top++, col_begin, col_end)
                );
            }
            if(bottom & 1){
                result = Monoid.op(
                    result,
                    column_prod(version, --bottom, col_begin, col_end)
                );
            }
            top >>= 1;
            bottom >>= 1;
        }
        return result;
    }

    S prod(
        int row_begin,
        int col_begin,
        int row_end,
        int col_end
    ) const{
        return prod(
            latest_version(),
            row_begin,
            col_begin,
            row_end,
            col_end
        );
    }

    S all_prod(int version) const{
        check_version(
            version,
            "library assertion fault: range violation (all_prod)."
        );
        return value_at(version, index(1, 1));
    }

    S all_prod() const{
        return all_prod(latest_version());
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_SEGTREE_PARTIALLY_PERSISTENT_SEGTREE_2D_HPP_INCLUDED
