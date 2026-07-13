#pragma once

#include <array>
#include <cstddef>
#include <limits>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <vector>

namespace rollback_segtree_2d_internal{

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

} // namespace rollback_segtree_2d_internal

template<auto Monoid, int MAX_HEIGHT, int MAX_WIDTH, int MAX_UPDATE>
class RollbackSegtree2D{
    static_assert(MAX_HEIGHT > 0);
    static_assert(MAX_WIDTH > 0);
    static_assert(MAX_UPDATE >= 0);

public:
    using monoid_type = std::remove_cvref_t<decltype(Monoid)>;
    using S = typename monoid_type::S;
    using Snapshot = int;

private:
    static constexpr int ROW_SIZE =
        rollback_segtree_2d_internal::ceil_power_of_two(MAX_HEIGHT);
    static constexpr int COL_SIZE =
        rollback_segtree_2d_internal::ceil_power_of_two(MAX_WIDTH);
    static constexpr int ROW_HEIGHT =
        rollback_segtree_2d_internal::tree_height(MAX_HEIGHT);
    static constexpr int COL_HEIGHT =
        rollback_segtree_2d_internal::tree_height(MAX_WIDTH);
    static constexpr int COL_NODES = 2 * COL_SIZE;
    static constexpr std::size_t NODE_COUNT =
        static_cast<std::size_t>(2 * ROW_SIZE) * COL_NODES;
    static constexpr std::size_t CHANGES_PER_UPDATE =
        (static_cast<std::size_t>(ROW_HEIGHT) + 1) *
        (static_cast<std::size_t>(COL_HEIGHT) + 1);

    static_assert(
        NODE_COUNT <=
        static_cast<std::size_t>(std::numeric_limits<int>::max())
    );
    static_assert(
        static_cast<std::size_t>(MAX_UPDATE) <=
        std::numeric_limits<std::size_t>::max() / CHANGES_PER_UPDATE
    );

    static constexpr std::size_t HISTORY_CAPACITY =
        static_cast<std::size_t>(MAX_UPDATE) * CHANGES_PER_UPDATE;

    static_assert(
        HISTORY_CAPACITY <=
        static_cast<std::size_t>(std::numeric_limits<int>::max())
    );

    struct History{
        int tree_index;
        int old_value_ref;
    };

    int _height = 0;
    int _width = 0;
    int operation_count = 0;
    int node_history_size = 0;

    std::array<std::optional<S>, NODE_COUNT> initial;
    std::array<int, NODE_COUNT> current_value_ref;
    std::array<std::optional<S>, HISTORY_CAPACITY> updated_values;
    std::array<History, HISTORY_CAPACITY> history;
    std::array<int, MAX_UPDATE> operation_history;

    static constexpr int index(int row_node, int col_node){
        return row_node * COL_NODES + col_node;
    }

    static int initial_value_ref(int tree_index){
        return -tree_index - 1;
    }

    void check_shape(int height, int width, const char* message) const{
        if(height < 0 || MAX_HEIGHT < height ||
            width < 0 || MAX_WIDTH < width)[[unlikely]]{
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
            "library assertion fault: range violation (RollbackSegtree2D constructor)."
        );
        _height = height;
        _width = width;
        for(int node = 0; node < static_cast<int>(NODE_COUNT); ++node){
            initial[static_cast<std::size_t>(node)].emplace(Monoid.e());
            current_value_ref[static_cast<std::size_t>(node)] =
                initial_value_ref(node);
        }
    }

    void build_values(const std::vector<std::vector<S>>& values){
        for(int row = 0; row < _height; ++row){
            const int row_node = row + ROW_SIZE;
            for(int col = 0; col < _width; ++col){
                initial[static_cast<std::size_t>(
                    index(row_node, col + COL_SIZE)
                )].reset();
                initial[static_cast<std::size_t>(
                    index(row_node, col + COL_SIZE)
                )].emplace(
                    values[static_cast<std::size_t>(row)]
                          [static_cast<std::size_t>(col)]
                );
            }
            for(int col_node = COL_SIZE - 1; col_node > 0; --col_node){
                initial[static_cast<std::size_t>(index(row_node, col_node))].reset();
                initial[static_cast<std::size_t>(index(row_node, col_node))].emplace(
                    Monoid.op(
                        current_value(index(row_node, col_node << 1)),
                        current_value(index(row_node, col_node << 1 | 1))
                    )
                );
            }
        }
        for(int row_node = ROW_SIZE - 1; row_node > 0; --row_node){
            for(int col_node = 1; col_node < COL_NODES; ++col_node){
                initial[static_cast<std::size_t>(index(row_node, col_node))].reset();
                initial[static_cast<std::size_t>(index(row_node, col_node))].emplace(
                    Monoid.op(
                        current_value(index(row_node << 1, col_node)),
                        current_value(index(row_node << 1 | 1, col_node))
                    )
                );
            }
        }
    }

    const S& current_value(int tree_index) const{
        const int value_ref =
            current_value_ref[static_cast<std::size_t>(tree_index)];
        return value_ref < 0 ?
            *initial[static_cast<std::size_t>(-value_ref - 1)] :
            *updated_values[static_cast<std::size_t>(value_ref)];
    }

    void write(int tree_index, const S& value){
        if(static_cast<std::size_t>(node_history_size) >= HISTORY_CAPACITY)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: history capacity exceeded (set)."
            );
        }
        updated_values[static_cast<std::size_t>(node_history_size)].emplace(value);
        history[static_cast<std::size_t>(node_history_size)] = {
            tree_index,
            current_value_ref[static_cast<std::size_t>(tree_index)]
        };
        current_value_ref[static_cast<std::size_t>(tree_index)] =
            node_history_size;
        ++node_history_size;
    }

    void restore_nodes(int snapshot){
        while(snapshot < node_history_size){
            --node_history_size;
            const History& entry =
                history[static_cast<std::size_t>(node_history_size)];
            current_value_ref[static_cast<std::size_t>(entry.tree_index)] =
                entry.old_value_ref;
            updated_values[static_cast<std::size_t>(node_history_size)].reset();
        }
    }

    void undo_one(){
        if constexpr(MAX_UPDATE == 0){
            throw std::runtime_error(
                "library assertion fault: undo history is empty (undo)."
            );
        }else{
            const int previous = operation_count - 1;
            if(previous < 0 || MAX_UPDATE <= previous)[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: undo history is empty (undo)."
                );
            }
            operation_count = previous;
            restore_nodes(
                operation_history[static_cast<std::size_t>(previous)]
            );
        }
    }

    S column_prod(int row_node, int col_begin, int col_end) const{
        S left_result = Monoid.e();
        S right_result = Monoid.e();
        int left = col_begin + COL_SIZE;
        int right = col_end + COL_SIZE;
        while(left < right){
            if(left & 1){
                left_result = Monoid.op(
                    left_result,
                    current_value(index(row_node, left++))
                );
            }
            if(right & 1){
                right_result = Monoid.op(
                    current_value(index(row_node, --right)),
                    right_result
                );
            }
            left >>= 1;
            right >>= 1;
        }
        return Monoid.op(left_result, right_result);
    }

public:
    RollbackSegtree2D(const RollbackSegtree2D&) = delete;
    RollbackSegtree2D& operator=(const RollbackSegtree2D&) = delete;
    RollbackSegtree2D(RollbackSegtree2D&&) = delete;
    RollbackSegtree2D& operator=(RollbackSegtree2D&&) = delete;

    RollbackSegtree2D(){
        initialize(0, 0);
    }

    RollbackSegtree2D(int height, int width){
        initialize(height, width);
    }

    explicit RollbackSegtree2D(const std::vector<std::vector<S>>& values){
        const int height = static_cast<int>(values.size());
        const int width = height == 0 ? 0 : static_cast<int>(values[0].size());
        check_shape(
            height,
            width,
            "library assertion fault: range violation (RollbackSegtree2D constructor)."
        );
        for(const auto& row : values){
            if(static_cast<int>(row.size()) != width)[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: rectangular violation (RollbackSegtree2D constructor)."
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

    int history_size() const{
        return operation_count;
    }

    bool can_undo() const{
        return operation_count != 0;
    }

    void set(int row, int col, const S& value){
        check_point(
            row,
            col,
            "library assertion fault: range violation (set)."
        );
        if(operation_count == MAX_UPDATE)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: capacity violation (set)."
            );
        }

        const int snapshot = node_history_size;
        try{
            const int leaf_row = row + ROW_SIZE;
            const int leaf_col = col + COL_SIZE;
            write(index(leaf_row, leaf_col), value);
            for(int col_node = leaf_col >> 1;
                col_node > 0;
                col_node >>= 1){
                write(
                    index(leaf_row, col_node),
                    Monoid.op(
                        current_value(index(leaf_row, col_node << 1)),
                        current_value(index(leaf_row, col_node << 1 | 1))
                    )
                );
            }
            for(int row_node = leaf_row >> 1;
                row_node > 0;
                row_node >>= 1){
                write(
                    index(row_node, leaf_col),
                    Monoid.op(
                        current_value(index(row_node << 1, leaf_col)),
                        current_value(index(row_node << 1 | 1, leaf_col))
                    )
                );
                for(int col_node = leaf_col >> 1;
                    col_node > 0;
                    col_node >>= 1){
                    write(
                        index(row_node, col_node),
                        Monoid.op(
                            current_value(index(row_node, col_node << 1)),
                            current_value(index(row_node, col_node << 1 | 1))
                        )
                    );
                }
            }
        }catch(...){
            restore_nodes(snapshot);
            throw;
        }
        operation_history[static_cast<std::size_t>(operation_count++)] = snapshot;
    }

    void apply(int row, int col, const S& value){
        set(row, col, Monoid.op(get(row, col), value));
    }

    S get(int row, int col) const{
        check_point(
            row,
            col,
            "library assertion fault: range violation (get)."
        );
        return current_value(index(row + ROW_SIZE, col + COL_SIZE));
    }

    S prod(
        int row_begin,
        int col_begin,
        int row_end,
        int col_end
    ) const{
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
                    column_prod(top++, col_begin, col_end)
                );
            }
            if(bottom & 1){
                result = Monoid.op(
                    result,
                    column_prod(--bottom, col_begin, col_end)
                );
            }
            top >>= 1;
            bottom >>= 1;
        }
        return result;
    }

    S all_prod() const{
        return current_value(index(1, 1));
    }

    Snapshot snapshot() const{
        return operation_count;
    }

    void undo(){
        if(!can_undo())[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: undo history is empty (undo)."
            );
        }
        undo_one();
    }

    void rollback(Snapshot target){
        if(target < 0 || operation_count < target)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (rollback)."
            );
        }
        while(target < operation_count) undo_one();
    }
};
