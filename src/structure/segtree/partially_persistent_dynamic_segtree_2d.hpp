#ifndef CPPLIB_SRC_STRUCTURE_SEGTREE_PARTIALLY_PERSISTENT_DYNAMIC_SEGTREE_2D_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_SEGTREE_PARTIALLY_PERSISTENT_DYNAMIC_SEGTREE_2D_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <limits>
#include <optional>
#include <stdexcept>
#include <type_traits>

template<auto Monoid, long long MAX_HEIGHT, long long MAX_WIDTH,
         int MAX_ROW_NODES, int MAX_COLUMN_NODES,
         int MAX_VERSIONS, int MAX_CHANGES>
struct PartiallyPersistentDynamicSegtree2D{
    static_assert(0 < MAX_HEIGHT && MAX_HEIGHT < 0x3fffffffffffffffLL);
    static_assert(0 < MAX_WIDTH && MAX_WIDTH < 0x3fffffffffffffffLL);
    static_assert(MAX_ROW_NODES >= 0 && MAX_COLUMN_NODES >= 0);
    static_assert(MAX_VERSIONS >= 0 && MAX_CHANGES >= 0);
    static_assert(MAX_VERSIONS < std::numeric_limits<int>::max());
    using S = typename std::remove_cvref_t<decltype(Monoid)>::S;

private:
    static constexpr int history_log = []{
        int value = MAX_CHANGES, result = 1;
        while(value > 1){ value >>= 1; ++result; }
        return result;
    }();
    struct RowNode{
        int left = -1, right = -1, columns = -1, parent = -1;
        bool parent_right = false;
    };
    struct ColumnNode{
        int left = -1, right = -1, head = -1, parent = -1, owner_row = -1;
        bool parent_right = false;
    };
    std::array<std::optional<RowNode>, MAX_ROW_NODES> row_nodes;
    std::array<std::optional<ColumnNode>, MAX_COLUMN_NODES> column_nodes;
    std::array<std::optional<S>, MAX_CHANGES> history_value;
    std::array<int, MAX_CHANGES> history_node{}, history_version{}, history_previous{};
    std::array<std::array<int, MAX_CHANGES>, history_log> jump{};
    int root = -1, row_count = 0, column_count = 0, history_count = 0;
    int version_count = 1;

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

    S value_at(int node, int version) const{
        if(node == -1) return Monoid.e();
        int change = column_nodes[static_cast<std::size_t>(node)]->head;
        if(change == -1) return Monoid.e();
        if(history_version[static_cast<std::size_t>(change)] <= version)
            return *history_value[static_cast<std::size_t>(change)];
        for(int level = history_log - 1; level >= 0; --level){
            const int ancestor = jump[static_cast<std::size_t>(level)][static_cast<std::size_t>(change)];
            if(ancestor != -1 && history_version[static_cast<std::size_t>(ancestor)] > version)
                change = ancestor;
        }
        change = history_previous[static_cast<std::size_t>(change)];
        return change == -1 ? Monoid.e() : *history_value[static_cast<std::size_t>(change)];
    }
    S current_value(int node) const{
        if(node == -1) return Monoid.e();
        const int change = column_nodes[static_cast<std::size_t>(node)]->head;
        return change == -1 ? Monoid.e() : *history_value[static_cast<std::size_t>(change)];
    }
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
        column_nodes[static_cast<std::size_t>(result)].emplace();
        auto& node = *column_nodes[static_cast<std::size_t>(result)];
        node.parent = parent;
        node.parent_right = parent_right;
        node.owner_row = owner_row;
        ++column_count;
        return result;
    }
    void write(int node, int version, const S& value){
        const int change = history_count;
        history_value[static_cast<std::size_t>(change)].emplace(value);
        history_node[static_cast<std::size_t>(change)] = node;
        history_version[static_cast<std::size_t>(change)] = version;
        history_previous[static_cast<std::size_t>(change)] = column_nodes[static_cast<std::size_t>(node)]->head;
        jump[0][static_cast<std::size_t>(change)] = history_previous[static_cast<std::size_t>(change)];
        for(int level = 1; level < history_log; ++level){
            const int ancestor = jump[static_cast<std::size_t>(level - 1)][static_cast<std::size_t>(change)];
            jump[static_cast<std::size_t>(level)][static_cast<std::size_t>(change)] = ancestor == -1
                ? -1 : jump[static_cast<std::size_t>(level - 1)][static_cast<std::size_t>(ancestor)];
        }
        column_nodes[static_cast<std::size_t>(node)]->head = change;
        ++history_count;
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
    void column_set_current(int owner, long long col, const S& value, int version){
        std::array<int, 64> path{};
        const int size = ensure_column_path(owner, col, path);
        write(path[static_cast<std::size_t>(size - 1)], version, value);
        for(int position = size - 2; position >= 0; --position){
            const int node = path[static_cast<std::size_t>(position)];
            write(node, version, Monoid.op(current_value(column_left(node)), current_value(column_right(node))));
        }
    }
    S column_get_impl(int node, long long position, int version, bool current) const{
        long long left = 0, right = MAX_WIDTH;
        while(node != -1 && right - left > 1){
            const long long middle = left + (right - left) / 2;
            if(position < middle){ node = column_left(node); right = middle; }
            else{ node = column_right(node); left = middle; }
        }
        return current ? current_value(node) : value_at(node, version);
    }
    S column_prod(int node, int version, long long left, long long right,
                  long long query_left, long long query_right) const{
        if(node == -1 || right <= query_left || query_right <= left) return Monoid.e();
        if(query_left <= left && right <= query_right) return value_at(node, version);
        const long long middle = left + (right - left) / 2;
        return Monoid.op(
            column_prod(column_left(node), version, left, middle, query_left, query_right),
            column_prod(column_right(node), version, middle, right, query_left, query_right)
        );
    }
    S row_prod(int node, int version, long long top, long long bottom,
               long long row_begin, long long row_end,
               long long col_begin, long long col_end) const{
        if(node == -1 || bottom <= row_begin || row_end <= top) return Monoid.e();
        if(row_begin <= top && bottom <= row_end)
            return column_prod(row_columns(node), version, 0, MAX_WIDTH, col_begin, col_end);
        const long long middle = top + (bottom - top) / 2;
        return Monoid.op(
            row_prod(row_left(node), version, top, middle, row_begin, row_end, col_begin, col_end),
            row_prod(row_right(node), version, middle, bottom, row_begin, row_end, col_begin, col_end)
        );
    }

    void rollback_history(int snapshot){
        while(snapshot < history_count){
            const int change = --history_count;
            const int node = history_node[static_cast<std::size_t>(change)];
            column_nodes[static_cast<std::size_t>(node)]->head = history_previous[static_cast<std::size_t>(change)];
            history_value[static_cast<std::size_t>(change)].reset();
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
    void check_version(int version, const char* message) const{
        if(version < 0 || version_count <= version)[[unlikely]] throw std::runtime_error(message);
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
    PartiallyPersistentDynamicSegtree2D(){ for(auto& level: jump) level.fill(-1); }
    PartiallyPersistentDynamicSegtree2D(const PartiallyPersistentDynamicSegtree2D&) = delete;
    PartiallyPersistentDynamicSegtree2D& operator=(const PartiallyPersistentDynamicSegtree2D&) = delete;
    int versions() const{ return version_count; }
    int latest_version() const{ return version_count - 1; }
    int row_nodes_used() const{ return row_count; }
    int column_nodes_used() const{ return column_count; }
    int changes_used() const{ return history_count; }

    int set(long long row, long long col, const S& value){
        check_point(row, col, "library assertion fault: range violation (PartiallyPersistentDynamicSegtree2D::set). ");
        if(version_count > MAX_VERSIONS)[[unlikely]]
            throw std::runtime_error("library assertion fault: version capacity exceeded (PartiallyPersistentDynamicSegtree2D::set). ");
        int rows_needed = 0, columns_needed = 0;
        required_topology(row, col, rows_needed, columns_needed);
        const long long changes_needed = static_cast<long long>(path_length(MAX_HEIGHT, row)) * path_length(MAX_WIDTH, col);
        if(static_cast<long long>(row_count) + rows_needed > MAX_ROW_NODES)[[unlikely]]
            throw std::runtime_error("library assertion fault: row node capacity exceeded (PartiallyPersistentDynamicSegtree2D::set). ");
        if(static_cast<long long>(column_count) + columns_needed > MAX_COLUMN_NODES)[[unlikely]]
            throw std::runtime_error("library assertion fault: column node capacity exceeded (PartiallyPersistentDynamicSegtree2D::set). ");
        if(static_cast<long long>(history_count) + changes_needed > MAX_CHANGES)[[unlikely]]
            throw std::runtime_error("library assertion fault: history capacity exceeded (PartiallyPersistentDynamicSegtree2D::set). ");

        const int row_snapshot = row_count, column_snapshot = column_count;
        const int history_snapshot = history_count, old_root = root;
        try{
            std::array<int, 64> path{};
            const int size = ensure_row_path(row, path);
            for(int position = size - 1; position >= 0; --position){
                const int node = path[static_cast<std::size_t>(position)];
                S next = value;
                if(position + 1 != size){
                    next = Monoid.op(
                        column_get_impl(row_columns(row_left(node)), col, 0, true),
                        column_get_impl(row_columns(row_right(node)), col, 0, true)
                    );
                }
                column_set_current(node, col, next, version_count);
            }
        }catch(...){
            rollback_history(history_snapshot);
            reset_columns(column_snapshot);
            reset_rows(row_snapshot);
            root = old_root;
            throw;
        }
        return version_count++;
    }
    int apply(long long row, long long col, const S& value){
        return set(row, col, Monoid.op(get(row, col), value));
    }
    S get(int version, long long row, long long col) const{
        check_version(version, "library assertion fault: range violation (PartiallyPersistentDynamicSegtree2D::get). ");
        check_point(row, col, "library assertion fault: range violation (PartiallyPersistentDynamicSegtree2D::get). ");
        int node = root;
        long long top = 0, bottom = MAX_HEIGHT;
        while(node != -1 && bottom - top > 1){
            const long long middle = top + (bottom - top) / 2;
            if(row < middle){ node = row_left(node); bottom = middle; }
            else{ node = row_right(node); top = middle; }
        }
        return column_get_impl(row_columns(node), col, version, false);
    }
    S get(long long row, long long col) const{ return get(latest_version(), row, col); }
    S prod(int version, long long rb, long long cb, long long re, long long ce) const{
        check_version(version, "library assertion fault: range violation (PartiallyPersistentDynamicSegtree2D::prod). ");
        check_rectangle(rb, cb, re, ce, "library assertion fault: range violation (PartiallyPersistentDynamicSegtree2D::prod). ");
        return row_prod(root, version, 0, MAX_HEIGHT, rb, re, cb, ce);
    }
    S prod(long long rb, long long cb, long long re, long long ce) const{
        return prod(latest_version(), rb, cb, re, ce);
    }
    S all_prod(int version) const{
        check_version(version, "library assertion fault: range violation (PartiallyPersistentDynamicSegtree2D::all_prod). ");
        return value_at(row_columns(root), version);
    }
    S all_prod() const{ return all_prod(latest_version()); }
};

#endif  // CPPLIB_SRC_STRUCTURE_SEGTREE_PARTIALLY_PERSISTENT_DYNAMIC_SEGTREE_2D_HPP_INCLUDED
