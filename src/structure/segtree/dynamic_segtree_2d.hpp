#pragma once

#include <cstddef>
#include <stdexcept>
#include <type_traits>

template<auto Monoid, long long MAX_HEIGHT, long long MAX_WIDTH>
struct DynamicSegtree2D{
    static_assert(0 < MAX_HEIGHT && MAX_HEIGHT < 0x3fffffffffffffffLL);
    static_assert(0 < MAX_WIDTH && MAX_WIDTH < 0x3fffffffffffffffLL);

    using monoid_type = std::remove_cvref_t<decltype(Monoid)>;
    using S = typename monoid_type::S;

private:
    struct ColumnNode{
        S value;
        ColumnNode* left = nullptr;
        ColumnNode* right = nullptr;

        ColumnNode(): value(Monoid.e()){}
    };

    struct RowNode{
        RowNode* left = nullptr;
        RowNode* right = nullptr;
        ColumnNode* columns = nullptr;
    };

    std::size_t _row_nodes = 0;
    std::size_t _column_nodes = 0;
    RowNode* root = nullptr;

    RowNode* make_row_node(){
        RowNode* node = new RowNode();
        _row_nodes++;
        return node;
    }

    ColumnNode* make_column_node(){
        ColumnNode* node = new ColumnNode();
        _column_nodes++;
        return node;
    }

    static void delete_columns(ColumnNode* node){
        if(node == nullptr) return;
        delete_columns(node->left);
        delete_columns(node->right);
        delete node;
    }

    static void delete_rows(RowNode* node){
        if(node == nullptr) return;
        delete_rows(node->left);
        delete_rows(node->right);
        delete_columns(node->columns);
        delete node;
    }

    void check_point(long long row, long long col, const char* message) const{
        if(row < 0 || MAX_HEIGHT <= row || col < 0 || MAX_WIDTH <= col)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }

    void check_rectangle(
        long long row_begin,
        long long col_begin,
        long long row_end,
        long long col_end,
        const char* message
    ) const{
        if(row_begin < 0 || row_end < row_begin || MAX_HEIGHT < row_end ||
            col_begin < 0 || col_end < col_begin || MAX_WIDTH < col_end)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }

    void column_set(
        ColumnNode*& node,
        long long left,
        long long right,
        long long position,
        const S& value
    ){
        if(node == nullptr) node = make_column_node();
        if(right - left == 1){
            node->value = value;
            return;
        }
        const long long middle = (left + right) >> 1;
        if(position < middle){
            column_set(node->left, left, middle, position, value);
        }else{
            column_set(node->right, middle, right, position, value);
        }
        const S left_value = node->left == nullptr ? Monoid.e() : node->left->value;
        const S right_value = node->right == nullptr ? Monoid.e() : node->right->value;
        node->value = Monoid.op(left_value, right_value);
    }

    S column_get(
        const ColumnNode* node,
        long long left,
        long long right,
        long long position
    ) const{
        if(node == nullptr) return Monoid.e();
        if(right - left == 1) return node->value;
        const long long middle = (left + right) >> 1;
        if(position < middle) return column_get(node->left, left, middle, position);
        return column_get(node->right, middle, right, position);
    }

    S column_prod(
        const ColumnNode* node,
        long long left,
        long long right,
        long long query_left,
        long long query_right
    ) const{
        if(node == nullptr || right <= query_left || query_right <= left) return Monoid.e();
        if(query_left <= left && right <= query_right) return node->value;
        const long long middle = (left + right) >> 1;
        return Monoid.op(
            column_prod(node->left, left, middle, query_left, query_right),
            column_prod(node->right, middle, right, query_left, query_right)
        );
    }

    void row_set(
        RowNode* node,
        long long top,
        long long bottom,
        long long row,
        long long col,
        const S& value
    ){
        if(bottom - top == 1){
            column_set(node->columns, 0, MAX_WIDTH, col, value);
            return;
        }
        const long long middle = (top + bottom) >> 1;
        if(row < middle){
            if(node->left == nullptr) node->left = make_row_node();
            row_set(node->left, top, middle, row, col, value);
        }else{
            if(node->right == nullptr) node->right = make_row_node();
            row_set(node->right, middle, bottom, row, col, value);
        }
        const S left_value = node->left == nullptr
            ? Monoid.e()
            : column_get(node->left->columns, 0, MAX_WIDTH, col);
        const S right_value = node->right == nullptr
            ? Monoid.e()
            : column_get(node->right->columns, 0, MAX_WIDTH, col);
        column_set(node->columns, 0, MAX_WIDTH, col, Monoid.op(left_value, right_value));
    }

    S row_get(
        const RowNode* node,
        long long top,
        long long bottom,
        long long row,
        long long col
    ) const{
        if(node == nullptr) return Monoid.e();
        if(bottom - top == 1) return column_get(node->columns, 0, MAX_WIDTH, col);
        const long long middle = (top + bottom) >> 1;
        if(row < middle) return row_get(node->left, top, middle, row, col);
        return row_get(node->right, middle, bottom, row, col);
    }

    S row_prod(
        const RowNode* node,
        long long top,
        long long bottom,
        long long row_begin,
        long long row_end,
        long long col_begin,
        long long col_end
    ) const{
        if(node == nullptr || bottom <= row_begin || row_end <= top) return Monoid.e();
        if(row_begin <= top && bottom <= row_end){
            return column_prod(node->columns, 0, MAX_WIDTH, col_begin, col_end);
        }
        const long long middle = (top + bottom) >> 1;
        return Monoid.op(
            row_prod(node->left, top, middle, row_begin, row_end, col_begin, col_end),
            row_prod(node->right, middle, bottom, row_begin, row_end, col_begin, col_end)
        );
    }

public:
    DynamicSegtree2D(){
        root = make_row_node();
    }

    ~DynamicSegtree2D(){
        delete_rows(root);
    }

    DynamicSegtree2D(const DynamicSegtree2D&) = delete;
    DynamicSegtree2D& operator=(const DynamicSegtree2D&) = delete;

    void clear(){
        delete_rows(root);
        _row_nodes = 0;
        _column_nodes = 0;
        root = make_row_node();
    }

    std::size_t row_node_count() const{
        return _row_nodes;
    }

    std::size_t column_node_count() const{
        return _column_nodes;
    }

    void set(long long row, long long col, const S& value){
        check_point(row, col, "library assertion fault: range violation (DynamicSegtree2D::set).");
        row_set(root, 0, MAX_HEIGHT, row, col, value);
    }

    void apply(long long row, long long col, const S& value){
        set(row, col, Monoid.op(get(row, col), value));
    }

    S get(long long row, long long col) const{
        check_point(row, col, "library assertion fault: range violation (DynamicSegtree2D::get).");
        return row_get(root, 0, MAX_HEIGHT, row, col);
    }

    S prod(
        long long row_begin,
        long long col_begin,
        long long row_end,
        long long col_end
    ) const{
        check_rectangle(
            row_begin,
            col_begin,
            row_end,
            col_end,
            "library assertion fault: range violation (DynamicSegtree2D::prod)."
        );
        return row_prod(
            root,
            0,
            MAX_HEIGHT,
            row_begin,
            row_end,
            col_begin,
            col_end
        );
    }

    S all_prod() const{
        return root->columns == nullptr ? Monoid.e() : root->columns->value;
    }
};
