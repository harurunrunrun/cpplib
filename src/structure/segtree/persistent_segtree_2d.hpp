#pragma once

#include <array>
#include <cstddef>
#include <limits>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <vector>

namespace persistent_segtree_2d_internal{

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

} // namespace persistent_segtree_2d_internal

template<auto Monoid, int MAX_HEIGHT, int MAX_WIDTH, int MAX_VERSION>
class PersistentSegtree2D{
    static_assert(MAX_HEIGHT > 0);
    static_assert(MAX_WIDTH > 0);
    static_assert(MAX_VERSION >= 0);

public:
    using monoid_type = std::remove_cvref_t<decltype(Monoid)>;
    using S = typename monoid_type::S;

private:
    static constexpr int ROW_SIZE =
        persistent_segtree_2d_internal::ceil_power_of_two(MAX_HEIGHT);
    static constexpr int COL_SIZE =
        persistent_segtree_2d_internal::ceil_power_of_two(MAX_WIDTH);
    static constexpr int ROW_HEIGHT =
        persistent_segtree_2d_internal::tree_height(MAX_HEIGHT);
    static constexpr int COL_HEIGHT =
        persistent_segtree_2d_internal::tree_height(MAX_WIDTH);
    static constexpr int COL_NODES = 2 * COL_SIZE;
    static constexpr std::size_t BASE_NODE_COUNT =
        static_cast<std::size_t>(2 * ROW_SIZE) * COL_NODES;
    static constexpr std::size_t ROW_NODES_PER_UPDATE = ROW_HEIGHT + 1;
    static constexpr std::size_t COL_NODES_PER_UPDATE =
        (static_cast<std::size_t>(ROW_HEIGHT) + 1) *
        (static_cast<std::size_t>(COL_HEIGHT) + 1);

    static_assert(
        static_cast<std::size_t>(MAX_VERSION) <=
        std::numeric_limits<std::size_t>::max() / ROW_NODES_PER_UPDATE
    );
    static_assert(
        static_cast<std::size_t>(MAX_VERSION) <=
        std::numeric_limits<std::size_t>::max() / COL_NODES_PER_UPDATE
    );

    static constexpr std::size_t ROW_NODE_CAPACITY =
        static_cast<std::size_t>(MAX_VERSION) * ROW_NODES_PER_UPDATE;
    static constexpr std::size_t COL_NODE_CAPACITY =
        static_cast<std::size_t>(MAX_VERSION) * COL_NODES_PER_UPDATE;

    static_assert(
        ROW_NODE_CAPACITY <=
        static_cast<std::size_t>(std::numeric_limits<int>::max())
    );
    static_assert(
        COL_NODE_CAPACITY <=
        static_cast<std::size_t>(std::numeric_limits<int>::max())
    );

    struct RowNode{
        int left;
        int right;
        int column_root;
    };

    struct ColNode{
        int left;
        int right;
    };

    int _height = 0;
    int _width = 0;
    int version_count = 1;
    int row_node_count = 0;
    int col_node_count = 0;

    std::array<std::optional<S>, BASE_NODE_COUNT> initial;
    std::array<int, static_cast<std::size_t>(MAX_VERSION) + 1> roots;
    std::array<RowNode, ROW_NODE_CAPACITY> row_nodes;
    std::array<ColNode, COL_NODE_CAPACITY> col_nodes;
    std::array<std::optional<S>, COL_NODE_CAPACITY> col_values;

    static constexpr std::size_t index(int row_node, int col_node){
        return static_cast<std::size_t>(row_node) * COL_NODES + col_node;
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

    void initialize(int height, int width){
        check_shape(
            height,
            width,
            "library assertion fault: range violation (PersistentSegtree2D constructor)."
        );
        _height = height;
        _width = width;
        roots[0] = -1;
        for(std::size_t node = 0; node < BASE_NODE_COUNT; ++node){
            initial[node].emplace(Monoid.e());
        }
    }

    void build_values(const std::vector<std::vector<S>>& values){
        for(int row = 0; row < _height; ++row){
            const int row_node = row + ROW_SIZE;
            for(int col = 0; col < _width; ++col){
                initial[index(row_node, col + COL_SIZE)].reset();
                initial[index(row_node, col + COL_SIZE)].emplace(
                    values[static_cast<std::size_t>(row)]
                          [static_cast<std::size_t>(col)]
                );
            }
            for(int col_node = COL_SIZE - 1; col_node > 0; --col_node){
                initial[index(row_node, col_node)].reset();
                initial[index(row_node, col_node)].emplace(Monoid.op(
                    *initial[index(row_node, col_node << 1)],
                    *initial[index(row_node, col_node << 1 | 1)]
                ));
            }
        }
        for(int row_node = ROW_SIZE - 1; row_node > 0; --row_node){
            for(int col_node = 1; col_node < COL_NODES; ++col_node){
                initial[index(row_node, col_node)].reset();
                initial[index(row_node, col_node)].emplace(Monoid.op(
                    *initial[index(row_node << 1, col_node)],
                    *initial[index(row_node << 1 | 1, col_node)]
                ));
            }
        }
    }

    int row_left(int node) const{
        return node == -1 ? -1 : row_nodes[static_cast<std::size_t>(node)].left;
    }

    int row_right(int node) const{
        return node == -1 ? -1 : row_nodes[static_cast<std::size_t>(node)].right;
    }

    int column_root(int node) const{
        return node == -1 ? -1 :
            row_nodes[static_cast<std::size_t>(node)].column_root;
    }

    int col_left(int node) const{
        return node == -1 ? -1 : col_nodes[static_cast<std::size_t>(node)].left;
    }

    int col_right(int node) const{
        return node == -1 ? -1 : col_nodes[static_cast<std::size_t>(node)].right;
    }

    const S& col_value(int node, int row_tree_index, int col_tree_index) const{
        return node == -1 ?
            *initial[index(row_tree_index, col_tree_index)] :
            *col_values[static_cast<std::size_t>(node)];
    }

    int new_row_node(int source){
        if(static_cast<std::size_t>(row_node_count) >= ROW_NODE_CAPACITY)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: row node capacity exceeded (set)."
            );
        }
        const int result = row_node_count++;
        row_nodes[static_cast<std::size_t>(result)] = source == -1 ?
            RowNode{-1, -1, -1} : row_nodes[static_cast<std::size_t>(source)];
        return result;
    }

    int new_col_node(int source, int row_tree_index, int col_tree_index){
        if(static_cast<std::size_t>(col_node_count) >= COL_NODE_CAPACITY)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: column node capacity exceeded (set)."
            );
        }
        const int result = col_node_count;
        col_values[static_cast<std::size_t>(result)].emplace(
            col_value(source, row_tree_index, col_tree_index)
        );
        col_nodes[static_cast<std::size_t>(result)] = source == -1 ?
            ColNode{-1, -1} : col_nodes[static_cast<std::size_t>(source)];
        ++col_node_count;
        return result;
    }

    int column_set(
        int source,
        int row_tree_index,
        int col_tree_index,
        int left,
        int right,
        int position,
        const S& value
    ){
        const int current = new_col_node(
            source,
            row_tree_index,
            col_tree_index
        );
        if(right - left == 1){
            *col_values[static_cast<std::size_t>(current)] = value;
            return current;
        }

        const int middle = left + (right - left) / 2;
        if(position < middle){
            col_nodes[static_cast<std::size_t>(current)].left = column_set(
                col_left(source),
                row_tree_index,
                col_tree_index << 1,
                left,
                middle,
                position,
                value
            );
        }else{
            col_nodes[static_cast<std::size_t>(current)].right = column_set(
                col_right(source),
                row_tree_index,
                col_tree_index << 1 | 1,
                middle,
                right,
                position,
                value
            );
        }
        *col_values[static_cast<std::size_t>(current)] = Monoid.op(
            col_value(
                col_nodes[static_cast<std::size_t>(current)].left,
                row_tree_index,
                col_tree_index << 1
            ),
            col_value(
                col_nodes[static_cast<std::size_t>(current)].right,
                row_tree_index,
                col_tree_index << 1 | 1
            )
        );
        return current;
    }

    S column_get(
        int node,
        int row_tree_index,
        int position
    ) const{
        int col_tree_index = 1;
        int left = 0;
        int right = COL_SIZE;
        while(right - left > 1){
            const int middle = left + (right - left) / 2;
            if(position < middle){
                node = col_left(node);
                col_tree_index <<= 1;
                right = middle;
            }else{
                node = col_right(node);
                col_tree_index = col_tree_index << 1 | 1;
                left = middle;
            }
        }
        return col_value(node, row_tree_index, col_tree_index);
    }

    S column_prod(
        int node,
        int row_tree_index,
        int col_tree_index,
        int left,
        int right,
        int query_left,
        int query_right
    ) const{
        if(right <= query_left || query_right <= left) return Monoid.e();
        if(query_left <= left && right <= query_right){
            return col_value(node, row_tree_index, col_tree_index);
        }
        const int middle = left + (right - left) / 2;
        return Monoid.op(
            column_prod(
                col_left(node),
                row_tree_index,
                col_tree_index << 1,
                left,
                middle,
                query_left,
                query_right
            ),
            column_prod(
                col_right(node),
                row_tree_index,
                col_tree_index << 1 | 1,
                middle,
                right,
                query_left,
                query_right
            )
        );
    }

    int set_impl(
        int source,
        int row_tree_index,
        int top,
        int bottom,
        int row,
        int col,
        const S& value
    ){
        const int current = new_row_node(source);
        if(bottom - top == 1){
            row_nodes[static_cast<std::size_t>(current)].column_root = column_set(
                column_root(source),
                row_tree_index,
                1,
                0,
                COL_SIZE,
                col,
                value
            );
            return current;
        }

        const int middle = top + (bottom - top) / 2;
        if(row < middle){
            row_nodes[static_cast<std::size_t>(current)].left = set_impl(
                row_left(source),
                row_tree_index << 1,
                top,
                middle,
                row,
                col,
                value
            );
        }else{
            row_nodes[static_cast<std::size_t>(current)].right = set_impl(
                row_right(source),
                row_tree_index << 1 | 1,
                middle,
                bottom,
                row,
                col,
                value
            );
        }

        const S merged = Monoid.op(
            column_get(
                column_root(
                    row_nodes[static_cast<std::size_t>(current)].left
                ),
                row_tree_index << 1,
                col
            ),
            column_get(
                column_root(
                    row_nodes[static_cast<std::size_t>(current)].right
                ),
                row_tree_index << 1 | 1,
                col
            )
        );
        row_nodes[static_cast<std::size_t>(current)].column_root = column_set(
            column_root(source),
            row_tree_index,
            1,
            0,
            COL_SIZE,
            col,
            merged
        );
        return current;
    }

    S prod_impl(
        int node,
        int row_tree_index,
        int top,
        int bottom,
        int row_begin,
        int row_end,
        int col_begin,
        int col_end
    ) const{
        if(bottom <= row_begin || row_end <= top) return Monoid.e();
        if(row_begin <= top && bottom <= row_end){
            return column_prod(
                column_root(node),
                row_tree_index,
                1,
                0,
                COL_SIZE,
                col_begin,
                col_end
            );
        }
        const int middle = top + (bottom - top) / 2;
        return Monoid.op(
            prod_impl(
                row_left(node),
                row_tree_index << 1,
                top,
                middle,
                row_begin,
                row_end,
                col_begin,
                col_end
            ),
            prod_impl(
                row_right(node),
                row_tree_index << 1 | 1,
                middle,
                bottom,
                row_begin,
                row_end,
                col_begin,
                col_end
            )
        );
    }

    void restore_nodes(int row_snapshot, int col_snapshot){
        row_node_count = row_snapshot;
        while(col_snapshot < col_node_count){
            --col_node_count;
            col_values[static_cast<std::size_t>(col_node_count)].reset();
        }
    }

public:
    PersistentSegtree2D(const PersistentSegtree2D&) = delete;
    PersistentSegtree2D& operator=(const PersistentSegtree2D&) = delete;
    PersistentSegtree2D(PersistentSegtree2D&&) = delete;
    PersistentSegtree2D& operator=(PersistentSegtree2D&&) = delete;

    PersistentSegtree2D(){
        initialize(0, 0);
    }

    PersistentSegtree2D(int height, int width){
        initialize(height, width);
    }

    explicit PersistentSegtree2D(const std::vector<std::vector<S>>& values){
        const int height = static_cast<int>(values.size());
        const int width = height == 0 ? 0 : static_cast<int>(values[0].size());
        check_shape(
            height,
            width,
            "library assertion fault: range violation (PersistentSegtree2D constructor)."
        );
        for(const auto& row : values){
            if(static_cast<int>(row.size()) != width)[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: rectangular violation (PersistentSegtree2D constructor)."
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

    int set(int version, int row, int col, const S& value){
        check_version(
            version,
            "library assertion fault: range violation (set)."
        );
        check_point(
            row,
            col,
            "library assertion fault: range violation (set)."
        );
        if(version_count > MAX_VERSION)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: version capacity exceeded (set)."
            );
        }

        const int row_snapshot = row_node_count;
        const int col_snapshot = col_node_count;
        int new_root;
        try{
            new_root = set_impl(
                roots[static_cast<std::size_t>(version)],
                1,
                0,
                ROW_SIZE,
                row,
                col,
                value
            );
        }catch(...){
            restore_nodes(row_snapshot, col_snapshot);
            throw;
        }
        const int new_version = version_count++;
        roots[static_cast<std::size_t>(new_version)] = new_root;
        return new_version;
    }

    int apply(int version, int row, int col, const S& value){
        return set(version, row, col, Monoid.op(get(version, row, col), value));
    }

    int fork(int version){
        check_version(
            version,
            "library assertion fault: range violation (fork)."
        );
        if(version_count > MAX_VERSION)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: version capacity exceeded (fork)."
            );
        }
        const int new_version = version_count++;
        roots[static_cast<std::size_t>(new_version)] =
            roots[static_cast<std::size_t>(version)];
        return new_version;
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
        int node = roots[static_cast<std::size_t>(version)];
        int row_tree_index = 1;
        int top = 0;
        int bottom = ROW_SIZE;
        while(bottom - top > 1){
            const int middle = top + (bottom - top) / 2;
            if(row < middle){
                node = row_left(node);
                row_tree_index <<= 1;
                bottom = middle;
            }else{
                node = row_right(node);
                row_tree_index = row_tree_index << 1 | 1;
                top = middle;
            }
        }
        return column_get(column_root(node), row_tree_index, col);
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
        return prod_impl(
            roots[static_cast<std::size_t>(version)],
            1,
            0,
            ROW_SIZE,
            row_begin,
            row_end,
            col_begin,
            col_end
        );
    }

    S all_prod(int version) const{
        check_version(
            version,
            "library assertion fault: range violation (all_prod)."
        );
        const int root = roots[static_cast<std::size_t>(version)];
        return col_value(column_root(root), 1, 1);
    }
};
