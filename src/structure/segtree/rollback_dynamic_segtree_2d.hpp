#ifndef CPPLIB_SRC_STRUCTURE_SEGTREE_ROLLBACK_DYNAMIC_SEGTREE_2D_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_SEGTREE_ROLLBACK_DYNAMIC_SEGTREE_2D_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <optional>
#include <stdexcept>
#include <type_traits>

template<auto Monoid, long long MAX_HEIGHT, long long MAX_WIDTH,
         int MAX_ROW_NODES, int MAX_COLUMN_NODES, int MAX_CHANGES>
struct RollbackDynamicSegtree2D{
    static_assert(0 < MAX_HEIGHT && MAX_HEIGHT < 0x3fffffffffffffffLL);
    static_assert(0 < MAX_WIDTH && MAX_WIDTH < 0x3fffffffffffffffLL);
    static_assert(MAX_ROW_NODES >= 0 && MAX_COLUMN_NODES >= 0 && MAX_CHANGES >= 0);
    using S = typename std::remove_cvref_t<decltype(Monoid)>::S;
    using Snapshot = int;

private:
    struct RowNode{
        int left = -1, right = -1, columns = -1, parent = -1;
        bool parent_right = false;
    };
    struct ColumnNode{
        int left = -1, right = -1, parent = -1, owner_row = -1;
        bool parent_right = false;
        S value;
        explicit ColumnNode(const S& value_): value(value_){}
    };
    std::array<std::optional<RowNode>, MAX_ROW_NODES> row_nodes;
    std::array<std::optional<ColumnNode>, MAX_COLUMN_NODES> column_nodes;
    std::array<int, MAX_CHANGES> changed_node{};
    std::array<std::optional<S>, MAX_CHANGES> old_value;
    std::array<int, MAX_CHANGES> operation_change{}, operation_rows{}, operation_columns{}, operation_root{};
    int root = -1, row_count = 0, column_count = 0, change_count = 0, operation_count = 0;

    static int path_length(long long size, long long position){
        int result = 1;
        long long left = 0, right = size;
        while(right - left > 1){
            const long long middle = left + (right - left) / 2;
            if(position < middle) right = middle;
            else left = middle;
            ++result;
        }
        return result;
    }
    int row_left(int node) const{ return node == -1 ? -1 : row_nodes[static_cast<std::size_t>(node)]->left; }
    int row_right(int node) const{ return node == -1 ? -1 : row_nodes[static_cast<std::size_t>(node)]->right; }
    int row_columns(int node) const{ return node == -1 ? -1 : row_nodes[static_cast<std::size_t>(node)]->columns; }
    int column_left(int node) const{ return node == -1 ? -1 : column_nodes[static_cast<std::size_t>(node)]->left; }
    int column_right(int node) const{ return node == -1 ? -1 : column_nodes[static_cast<std::size_t>(node)]->right; }
    S column_value(int node) const{ return node == -1 ? Monoid.e() : column_nodes[static_cast<std::size_t>(node)]->value; }

    int new_row(int parent, bool parent_right){
        const int result = row_count;
        row_nodes[static_cast<std::size_t>(result)].emplace();
        row_nodes[static_cast<std::size_t>(result)]->parent = parent;
        row_nodes[static_cast<std::size_t>(result)]->parent_right = parent_right;
        ++row_count;
        return result;
    }
    int new_column(int parent, bool parent_right, int owner_row){
        const int result = column_count;
        column_nodes[static_cast<std::size_t>(result)].emplace(Monoid.e());
        auto& node = *column_nodes[static_cast<std::size_t>(result)];
        node.parent = parent;
        node.parent_right = parent_right;
        node.owner_row = owner_row;
        ++column_count;
        return result;
    }
    void write(int node, const S& value){
        const int change = change_count;
        old_value[static_cast<std::size_t>(change)].emplace(column_nodes[static_cast<std::size_t>(node)]->value);
        changed_node[static_cast<std::size_t>(change)] = node;
        column_nodes[static_cast<std::size_t>(node)]->value = value;
        ++change_count;
    }
    int required_columns(int node, long long position) const{
        int required = 0;
        long long left = 0, right = MAX_WIDTH;
        while(true){
            if(node == -1) ++required;
            if(right - left == 1) break;
            const long long middle = left + (right - left) / 2;
            if(position < middle){ node = column_left(node); right = middle; }
            else{ node = column_right(node); left = middle; }
        }
        return required;
    }
    void required_topology(long long row, long long col, int& rows, int& columns) const{
        rows = columns = 0;
        int node = root;
        long long top = 0, bottom = MAX_HEIGHT;
        while(true){
            if(node == -1){ ++rows; columns += path_length(MAX_WIDTH, col); }
            else columns += required_columns(row_columns(node), col);
            if(bottom - top == 1) break;
            const long long middle = top + (bottom - top) / 2;
            if(row < middle){ node = row_left(node); bottom = middle; }
            else{ node = row_right(node); top = middle; }
        }
    }
    int ensure_row_path(long long row, std::array<int, 64>& path){
        if(root == -1) root = new_row(-1, false);
        int size = 0, node = root;
        long long top = 0, bottom = MAX_HEIGHT;
        while(true){
            path[static_cast<std::size_t>(size++)] = node;
            if(bottom - top == 1) break;
            const long long middle = top + (bottom - top) / 2;
            const bool go_right = middle <= row;
            int child = go_right ? row_right(node) : row_left(node);
            if(child == -1){
                child = new_row(node, go_right);
                if(go_right) row_nodes[static_cast<std::size_t>(node)]->right = child;
                else row_nodes[static_cast<std::size_t>(node)]->left = child;
            }
            node = child;
            if(go_right) top = middle;
            else bottom = middle;
        }
        return size;
    }
    int ensure_column_path(int owner, long long col, std::array<int, 64>& path){
        int& owner_root = row_nodes[static_cast<std::size_t>(owner)]->columns;
        if(owner_root == -1) owner_root = new_column(-1, false, owner);
        int size = 0, node = owner_root;
        long long left = 0, right = MAX_WIDTH;
        while(true){
            path[static_cast<std::size_t>(size++)] = node;
            if(right - left == 1) break;
            const long long middle = left + (right - left) / 2;
            const bool go_right = middle <= col;
            int child = go_right ? column_right(node) : column_left(node);
            if(child == -1){
                child = new_column(node, go_right, -1);
                if(go_right) column_nodes[static_cast<std::size_t>(node)]->right = child;
                else column_nodes[static_cast<std::size_t>(node)]->left = child;
            }
            node = child;
            if(go_right) left = middle;
            else right = middle;
        }
        return size;
    }
    void column_set_current(int owner, long long col, const S& value){
        std::array<int, 64> path{};
        const int size = ensure_column_path(owner, col, path);
        write(path[static_cast<std::size_t>(size - 1)], value);
        for(int position = size - 2; position >= 0; --position){
            const int node = path[static_cast<std::size_t>(position)];
            write(node, Monoid.op(column_value(column_left(node)), column_value(column_right(node))));
        }
    }
    S column_get(int node, long long position) const{
        long long left = 0, right = MAX_WIDTH;
        while(node != -1 && right - left > 1){
            const long long middle = left + (right - left) / 2;
            if(position < middle){ node = column_left(node); right = middle; }
            else{ node = column_right(node); left = middle; }
        }
        return column_value(node);
    }
    S column_prod(int node, long long left, long long right,
                  long long query_left, long long query_right) const{
        if(node == -1 || right <= query_left || query_right <= left) return Monoid.e();
        if(query_left <= left && right <= query_right) return column_value(node);
        const long long middle = left + (right - left) / 2;
        return Monoid.op(
            column_prod(column_left(node), left, middle, query_left, query_right),
            column_prod(column_right(node), middle, right, query_left, query_right)
        );
    }
    S row_prod(int node, long long top, long long bottom,
               long long row_begin, long long row_end,
               long long col_begin, long long col_end) const{
        if(node == -1 || bottom <= row_begin || row_end <= top) return Monoid.e();
        if(row_begin <= top && bottom <= row_end)
            return column_prod(row_columns(node), 0, MAX_WIDTH, col_begin, col_end);
        const long long middle = top + (bottom - top) / 2;
        return Monoid.op(
            row_prod(row_left(node), top, middle, row_begin, row_end, col_begin, col_end),
            row_prod(row_right(node), middle, bottom, row_begin, row_end, col_begin, col_end)
        );
    }
    void restore_changes(int snapshot){
        while(snapshot < change_count){
            const int change = --change_count;
            column_nodes[static_cast<std::size_t>(changed_node[static_cast<std::size_t>(change)])]->value =
                *old_value[static_cast<std::size_t>(change)];
            old_value[static_cast<std::size_t>(change)].reset();
        }
    }
    void reset_columns(int snapshot){
        while(snapshot < column_count){
            const int node = --column_count;
            const ColumnNode removed = *column_nodes[static_cast<std::size_t>(node)];
            if(removed.parent != -1){
                if(removed.parent_right) column_nodes[static_cast<std::size_t>(removed.parent)]->right = -1;
                else column_nodes[static_cast<std::size_t>(removed.parent)]->left = -1;
            }else if(removed.owner_row != -1){
                row_nodes[static_cast<std::size_t>(removed.owner_row)]->columns = -1;
            }
            column_nodes[static_cast<std::size_t>(node)].reset();
        }
    }
    void reset_rows(int snapshot){
        while(snapshot < row_count){
            const int node = --row_count;
            const RowNode removed = *row_nodes[static_cast<std::size_t>(node)];
            if(removed.parent != -1){
                if(removed.parent_right) row_nodes[static_cast<std::size_t>(removed.parent)]->right = -1;
                else row_nodes[static_cast<std::size_t>(removed.parent)]->left = -1;
            }
            row_nodes[static_cast<std::size_t>(node)].reset();
        }
    }
    void undo_one(){
        const int operation = --operation_count;
        restore_changes(operation_change[static_cast<std::size_t>(operation)]);
        reset_columns(operation_columns[static_cast<std::size_t>(operation)]);
        reset_rows(operation_rows[static_cast<std::size_t>(operation)]);
        root = operation_root[static_cast<std::size_t>(operation)];
    }
    static void check_point(long long row, long long col, const char* message){
        if(row < 0 || MAX_HEIGHT <= row || col < 0 || MAX_WIDTH <= col)[[unlikely]]
            throw std::runtime_error(message);
    }
    static void check_rectangle(long long rb, long long cb, long long re, long long ce,
                                const char* message){
        if(rb < 0 || re < rb || MAX_HEIGHT < re || cb < 0 || ce < cb || MAX_WIDTH < ce)[[unlikely]]
            throw std::runtime_error(message);
    }

public:
    RollbackDynamicSegtree2D() = default;
    RollbackDynamicSegtree2D(const RollbackDynamicSegtree2D&) = delete;
    RollbackDynamicSegtree2D& operator=(const RollbackDynamicSegtree2D&) = delete;
    int history_size() const{ return operation_count; }
    int row_nodes_used() const{ return row_count; }
    int column_nodes_used() const{ return column_count; }
    int changes_used() const{ return change_count; }
    bool can_undo() const{ return operation_count != 0; }

    void set(long long row, long long col, const S& value){
        check_point(row, col, "library assertion fault: range violation (RollbackDynamicSegtree2D::set). ");
        int rows_needed = 0, columns_needed = 0;
        required_topology(row, col, rows_needed, columns_needed);
        const long long changes_needed = static_cast<long long>(path_length(MAX_HEIGHT, row)) * path_length(MAX_WIDTH, col);
        if(static_cast<long long>(row_count) + rows_needed > MAX_ROW_NODES)[[unlikely]]
            throw std::runtime_error("library assertion fault: row node capacity exceeded (RollbackDynamicSegtree2D::set). ");
        if(static_cast<long long>(column_count) + columns_needed > MAX_COLUMN_NODES)[[unlikely]]
            throw std::runtime_error("library assertion fault: column node capacity exceeded (RollbackDynamicSegtree2D::set). ");
        if(static_cast<long long>(change_count) + changes_needed > MAX_CHANGES || operation_count == MAX_CHANGES)[[unlikely]]
            throw std::runtime_error("library assertion fault: history capacity exceeded (RollbackDynamicSegtree2D::set). ");
        const int row_snapshot = row_count, column_snapshot = column_count;
        const int change_snapshot = change_count, root_snapshot = root;
        try{
            std::array<int, 64> path{};
            const int size = ensure_row_path(row, path);
            for(int position = size - 1; position >= 0; --position){
                const int node = path[static_cast<std::size_t>(position)];
                S next = value;
                if(position + 1 != size){
                    next = Monoid.op(
                        column_get(row_columns(row_left(node)), col),
                        column_get(row_columns(row_right(node)), col)
                    );
                }
                column_set_current(node, col, next);
            }
        }catch(...){
            restore_changes(change_snapshot);
            reset_columns(column_snapshot);
            reset_rows(row_snapshot);
            root = root_snapshot;
            throw;
        }
        const int operation = operation_count++;
        operation_change[static_cast<std::size_t>(operation)] = change_snapshot;
        operation_rows[static_cast<std::size_t>(operation)] = row_snapshot;
        operation_columns[static_cast<std::size_t>(operation)] = column_snapshot;
        operation_root[static_cast<std::size_t>(operation)] = root_snapshot;
    }
    void apply(long long row, long long col, const S& value){
        set(row, col, Monoid.op(get(row, col), value));
    }
    S get(long long row, long long col) const{
        check_point(row, col, "library assertion fault: range violation (RollbackDynamicSegtree2D::get). ");
        int node = root;
        long long top = 0, bottom = MAX_HEIGHT;
        while(node != -1 && bottom - top > 1){
            const long long middle = top + (bottom - top) / 2;
            if(row < middle){ node = row_left(node); bottom = middle; }
            else{ node = row_right(node); top = middle; }
        }
        return column_get(row_columns(node), col);
    }
    S prod(long long rb, long long cb, long long re, long long ce) const{
        check_rectangle(rb, cb, re, ce, "library assertion fault: range violation (RollbackDynamicSegtree2D::prod). ");
        return row_prod(root, 0, MAX_HEIGHT, rb, re, cb, ce);
    }
    S all_prod() const{ return column_value(row_columns(root)); }
    Snapshot snapshot() const{ return operation_count; }
    void undo(){
        if(!can_undo())[[unlikely]]
            throw std::runtime_error("library assertion fault: undo history is empty (RollbackDynamicSegtree2D::undo). ");
        undo_one();
    }
    void rollback(Snapshot target){
        if(target < 0 || operation_count < target)[[unlikely]]
            throw std::runtime_error("library assertion fault: range violation (RollbackDynamicSegtree2D::rollback). ");
        while(target < operation_count) undo_one();
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_SEGTREE_ROLLBACK_DYNAMIC_SEGTREE_2D_HPP_INCLUDED
