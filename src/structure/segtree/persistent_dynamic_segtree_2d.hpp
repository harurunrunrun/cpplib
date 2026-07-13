#pragma once

#include <array>
#include <cstddef>
#include <limits>
#include <optional>
#include <stdexcept>
#include <type_traits>

template<
    auto Monoid,
    long long MAX_HEIGHT,
    long long MAX_WIDTH,
    int MAX_ROW_NODES,
    int MAX_COLUMN_NODES,
    int MAX_VERSIONS
>
struct PersistentDynamicSegtree2D{
    static_assert(0 < MAX_HEIGHT && MAX_HEIGHT < 0x3fffffffffffffffLL);
    static_assert(0 < MAX_WIDTH && MAX_WIDTH < 0x3fffffffffffffffLL);
    static_assert(MAX_ROW_NODES >= 0 && MAX_COLUMN_NODES >= 0 && MAX_VERSIONS >= 0);
    static_assert(MAX_VERSIONS < std::numeric_limits<int>::max());

    using monoid_type = std::remove_cvref_t<decltype(Monoid)>;
    using S = typename monoid_type::S;

private:
    struct ColumnNode{
        int left = -1;
        int right = -1;
        S value;
        explicit ColumnNode(const S& value_): value(value_){}
    };

    struct RowNode{
        int left = -1;
        int right = -1;
        int columns = -1;
    };

    std::array<std::optional<RowNode>, MAX_ROW_NODES> row_nodes;
    std::array<std::optional<ColumnNode>, MAX_COLUMN_NODES> column_nodes;
    std::array<int, MAX_VERSIONS + 1> roots{};
    int row_count = 0;
    int column_count = 0;
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

    int row_left(int node) const{
        return node == -1 ? -1 : row_nodes[static_cast<std::size_t>(node)]->left;
    }

    int row_right(int node) const{
        return node == -1 ? -1 : row_nodes[static_cast<std::size_t>(node)]->right;
    }

    int row_columns(int node) const{
        return node == -1 ? -1 : row_nodes[static_cast<std::size_t>(node)]->columns;
    }

    int column_left(int node) const{
        return node == -1 ? -1 : column_nodes[static_cast<std::size_t>(node)]->left;
    }

    int column_right(int node) const{
        return node == -1 ? -1 : column_nodes[static_cast<std::size_t>(node)]->right;
    }

    S column_value(int node) const{
        return node == -1 ? Monoid.e() : column_nodes[static_cast<std::size_t>(node)]->value;
    }

    int clone_row(int source){
        const int result = row_count++;
        row_nodes[static_cast<std::size_t>(result)].emplace();
        if(source != -1){
            *row_nodes[static_cast<std::size_t>(result)] =
                *row_nodes[static_cast<std::size_t>(source)];
        }
        return result;
    }

    int clone_column(int source){
        const int result = column_count++;
        column_nodes[static_cast<std::size_t>(result)].emplace(column_value(source));
        if(source != -1){
            column_nodes[static_cast<std::size_t>(result)]->left = column_left(source);
            column_nodes[static_cast<std::size_t>(result)]->right = column_right(source);
        }
        return result;
    }

    int column_set(
        int source,
        long long left,
        long long right,
        long long position,
        const S& value
    ){
        const int current = clone_column(source);
        if(right - left == 1){
            column_nodes[static_cast<std::size_t>(current)]->value = value;
            return current;
        }
        const long long middle = left + (right - left) / 2;
        if(position < middle){
            column_nodes[static_cast<std::size_t>(current)]->left = column_set(
                column_left(source), left, middle, position, value
            );
        }else{
            column_nodes[static_cast<std::size_t>(current)]->right = column_set(
                column_right(source), middle, right, position, value
            );
        }
        column_nodes[static_cast<std::size_t>(current)]->value = Monoid.op(
            column_value(column_left(current)),
            column_value(column_right(current))
        );
        return current;
    }

    S column_get(
        int node,
        long long left,
        long long right,
        long long position
    ) const{
        if(node == -1) return Monoid.e();
        if(right - left == 1) return column_value(node);
        const long long middle = left + (right - left) / 2;
        if(position < middle) return column_get(column_left(node), left, middle, position);
        return column_get(column_right(node), middle, right, position);
    }

    S column_prod(
        int node,
        long long left,
        long long right,
        long long query_left,
        long long query_right
    ) const{
        if(node == -1 || right <= query_left || query_right <= left) return Monoid.e();
        if(query_left <= left && right <= query_right) return column_value(node);
        const long long middle = left + (right - left) / 2;
        return Monoid.op(
            column_prod(column_left(node), left, middle, query_left, query_right),
            column_prod(column_right(node), middle, right, query_left, query_right)
        );
    }

    int row_set(
        int source,
        long long top,
        long long bottom,
        long long row,
        long long col,
        const S& value
    ){
        const int current = clone_row(source);
        if(bottom - top == 1){
            row_nodes[static_cast<std::size_t>(current)]->columns = column_set(
                row_columns(source), 0, MAX_WIDTH, col, value
            );
            return current;
        }
        const long long middle = top + (bottom - top) / 2;
        if(row < middle){
            row_nodes[static_cast<std::size_t>(current)]->left = row_set(
                row_left(source), top, middle, row, col, value
            );
        }else{
            row_nodes[static_cast<std::size_t>(current)]->right = row_set(
                row_right(source), middle, bottom, row, col, value
            );
        }
        const S left_value = column_get(
            row_columns(row_left(current)), 0, MAX_WIDTH, col
        );
        const S right_value = column_get(
            row_columns(row_right(current)), 0, MAX_WIDTH, col
        );
        row_nodes[static_cast<std::size_t>(current)]->columns = column_set(
            row_columns(source),
            0,
            MAX_WIDTH,
            col,
            Monoid.op(left_value, right_value)
        );
        return current;
    }

    S row_get(
        int node,
        long long top,
        long long bottom,
        long long row,
        long long col
    ) const{
        if(node == -1) return Monoid.e();
        if(bottom - top == 1){
            return column_get(row_columns(node), 0, MAX_WIDTH, col);
        }
        const long long middle = top + (bottom - top) / 2;
        if(row < middle) return row_get(row_left(node), top, middle, row, col);
        return row_get(row_right(node), middle, bottom, row, col);
    }

    S row_prod(
        int node,
        long long top,
        long long bottom,
        long long row_begin,
        long long row_end,
        long long col_begin,
        long long col_end
    ) const{
        if(node == -1 || bottom <= row_begin || row_end <= top) return Monoid.e();
        if(row_begin <= top && bottom <= row_end){
            return column_prod(row_columns(node), 0, MAX_WIDTH, col_begin, col_end);
        }
        const long long middle = top + (bottom - top) / 2;
        return Monoid.op(
            row_prod(
                row_left(node), top, middle,
                row_begin, row_end, col_begin, col_end
            ),
            row_prod(
                row_right(node), middle, bottom,
                row_begin, row_end, col_begin, col_end
            )
        );
    }

    void check_version(int version, const char* message) const{
        if(version < 0 || version_count <= version)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }

    static void check_point(long long row, long long col, const char* message){
        if(row < 0 || MAX_HEIGHT <= row || col < 0 || MAX_WIDTH <= col)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }

    static void check_rectangle(
        long long row_begin,
        long long col_begin,
        long long row_end,
        long long col_end,
        const char* message
    ){
        if(row_begin < 0 || row_end < row_begin || MAX_HEIGHT < row_end ||
            col_begin < 0 || col_end < col_begin || MAX_WIDTH < col_end)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }

    void reset_rows(int snapshot){
        while(snapshot < row_count){
            row_nodes[static_cast<std::size_t>(--row_count)].reset();
        }
    }

    void reset_columns(int snapshot){
        while(snapshot < column_count){
            column_nodes[static_cast<std::size_t>(--column_count)].reset();
        }
    }

public:
    PersistentDynamicSegtree2D(){ roots.fill(-1); }
    PersistentDynamicSegtree2D(const PersistentDynamicSegtree2D&) = delete;
    PersistentDynamicSegtree2D& operator=(const PersistentDynamicSegtree2D&) = delete;

    int versions() const{ return version_count; }
    int latest_version() const{ return version_count - 1; }
    int row_nodes_used() const{ return row_count; }
    int column_nodes_used() const{ return column_count; }

    int set(int version, long long row, long long col, const S& value){
        check_version(version, "library assertion fault: range violation (PersistentDynamicSegtree2D::set). ");
        check_point(row, col, "library assertion fault: range violation (PersistentDynamicSegtree2D::set). ");
        if(version_count > MAX_VERSIONS)[[unlikely]]{
            throw std::runtime_error("library assertion fault: version capacity exceeded (PersistentDynamicSegtree2D::set). ");
        }
        const int rows_needed = path_length(MAX_HEIGHT, row);
        const int columns_needed = rows_needed * path_length(MAX_WIDTH, col);
        if(static_cast<long long>(row_count) + rows_needed > MAX_ROW_NODES)[[unlikely]]{
            throw std::runtime_error("library assertion fault: row node capacity exceeded (PersistentDynamicSegtree2D::set). ");
        }
        if(static_cast<long long>(column_count) + columns_needed > MAX_COLUMN_NODES)[[unlikely]]{
            throw std::runtime_error("library assertion fault: column node capacity exceeded (PersistentDynamicSegtree2D::set). ");
        }

        const int row_snapshot = row_count;
        const int column_snapshot = column_count;
        try{
            roots[static_cast<std::size_t>(version_count)] = row_set(
                roots[static_cast<std::size_t>(version)],
                0,
                MAX_HEIGHT,
                row,
                col,
                value
            );
        }catch(...){
            reset_columns(column_snapshot);
            reset_rows(row_snapshot);
            throw;
        }
        return version_count++;
    }

    int set(long long row, long long col, const S& value){
        return set(latest_version(), row, col, value);
    }

    int apply(int version, long long row, long long col, const S& value){
        return set(version, row, col, Monoid.op(get(version, row, col), value));
    }

    int apply(long long row, long long col, const S& value){
        return apply(latest_version(), row, col, value);
    }

    int fork(int version){
        check_version(version, "library assertion fault: range violation (PersistentDynamicSegtree2D::fork). ");
        if(version_count > MAX_VERSIONS)[[unlikely]]{
            throw std::runtime_error("library assertion fault: version capacity exceeded (PersistentDynamicSegtree2D::fork). ");
        }
        roots[static_cast<std::size_t>(version_count)] = roots[static_cast<std::size_t>(version)];
        return version_count++;
    }

    S get(int version, long long row, long long col) const{
        check_version(version, "library assertion fault: range violation (PersistentDynamicSegtree2D::get). ");
        check_point(row, col, "library assertion fault: range violation (PersistentDynamicSegtree2D::get). ");
        return row_get(
            roots[static_cast<std::size_t>(version)],
            0,
            MAX_HEIGHT,
            row,
            col
        );
    }

    S get(long long row, long long col) const{
        return get(latest_version(), row, col);
    }

    S prod(
        int version,
        long long row_begin,
        long long col_begin,
        long long row_end,
        long long col_end
    ) const{
        check_version(version, "library assertion fault: range violation (PersistentDynamicSegtree2D::prod). ");
        check_rectangle(
            row_begin,
            col_begin,
            row_end,
            col_end,
            "library assertion fault: range violation (PersistentDynamicSegtree2D::prod). "
        );
        return row_prod(
            roots[static_cast<std::size_t>(version)],
            0,
            MAX_HEIGHT,
            row_begin,
            row_end,
            col_begin,
            col_end
        );
    }

    S prod(
        long long row_begin,
        long long col_begin,
        long long row_end,
        long long col_end
    ) const{
        return prod(latest_version(), row_begin, col_begin, row_end, col_end);
    }

    S all_prod(int version) const{
        check_version(version, "library assertion fault: range violation (PersistentDynamicSegtree2D::all_prod). ");
        return column_value(row_columns(roots[static_cast<std::size_t>(version)]));
    }

    S all_prod() const{ return all_prod(latest_version()); }
};
