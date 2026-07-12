#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <vector>

template<class T, int MAX_SIZE>
struct SegmentTreeBeats{
    static_assert(MAX_SIZE > 0);

    static constexpr int sz = []{
        int res = 1;
        while(res < MAX_SIZE){
            res <<= 1;
        }
        return res;
    }();

private:
    struct Node{
        T sum{};
        T max_value{};
        T second_max_value{};
        T min_value{};
        T second_min_value{};
        T add{};
        int max_count = 0;
        int min_count = 0;
        int length = 0;
    };

    int _n;
    std::array<Node, 2 * sz> data{};

    static Node make_leaf(const T& value){
        Node res;
        res.sum = value;
        res.max_value = value;
        res.min_value = value;
        res.max_count = 1;
        res.min_count = 1;
        res.length = 1;
        return res;
    }

    static Node merge(const Node& a, const Node& b){
        if(a.length == 0){
            Node res = b;
            res.add = T{};
            return res;
        }
        if(b.length == 0){
            Node res = a;
            res.add = T{};
            return res;
        }

        Node res;
        res.sum = a.sum + b.sum;
        res.length = a.length + b.length;

        if(a.max_value == b.max_value){
            res.max_value = a.max_value;
            res.max_count = a.max_count + b.max_count;

            bool a_has_second = a.max_count < a.length;
            bool b_has_second = b.max_count < b.length;
            if(a_has_second && b_has_second){
                res.second_max_value = std::max(
                    a.second_max_value, b.second_max_value
                );
            }else if(a_has_second){
                res.second_max_value = a.second_max_value;
            }else if(b_has_second){
                res.second_max_value = b.second_max_value;
            }
        }else if(a.max_value > b.max_value){
            res.max_value = a.max_value;
            res.max_count = a.max_count;
            res.second_max_value = b.max_value;
            if(a.max_count < a.length){
                res.second_max_value = std::max(
                    res.second_max_value, a.second_max_value
                );
            }
        }else{
            res.max_value = b.max_value;
            res.max_count = b.max_count;
            res.second_max_value = a.max_value;
            if(b.max_count < b.length){
                res.second_max_value = std::max(
                    res.second_max_value, b.second_max_value
                );
            }
        }

        if(a.min_value == b.min_value){
            res.min_value = a.min_value;
            res.min_count = a.min_count + b.min_count;

            bool a_has_second = a.min_count < a.length;
            bool b_has_second = b.min_count < b.length;
            if(a_has_second && b_has_second){
                res.second_min_value = std::min(
                    a.second_min_value, b.second_min_value
                );
            }else if(a_has_second){
                res.second_min_value = a.second_min_value;
            }else if(b_has_second){
                res.second_min_value = b.second_min_value;
            }
        }else if(a.min_value < b.min_value){
            res.min_value = a.min_value;
            res.min_count = a.min_count;
            res.second_min_value = b.min_value;
            if(a.min_count < a.length){
                res.second_min_value = std::min(
                    res.second_min_value, a.second_min_value
                );
            }
        }else{
            res.min_value = b.min_value;
            res.min_count = b.min_count;
            res.second_min_value = a.min_value;
            if(b.min_count < b.length){
                res.second_min_value = std::min(
                    res.second_min_value, b.second_min_value
                );
            }
        }

        return res;
    }

    void update(int k){
        data[k] = merge(data[2 * k], data[2 * k + 1]);
    }

    void apply_add(int k, const T& x){
        Node& node = data[k];
        if(node.length == 0){
            return;
        }

        node.sum += x * static_cast<T>(node.length);
        node.max_value += x;
        if(node.max_count < node.length){
            node.second_max_value += x;
        }
        node.min_value += x;
        if(node.min_count < node.length){
            node.second_min_value += x;
        }
        node.add += x;
    }

    void apply_chmin(int k, const T& x){
        Node& node = data[k];
        if(node.length == 0 || node.max_value <= x){
            return;
        }

        T old_max = node.max_value;
        node.sum += (x - old_max) * static_cast<T>(node.max_count);
        if(node.min_value == old_max){
            node.min_value = x;
        }else if(
            node.min_count < node.length &&
            node.second_min_value == old_max
        ){
            node.second_min_value = x;
        }
        node.max_value = x;
    }

    void apply_chmax(int k, const T& x){
        Node& node = data[k];
        if(node.length == 0 || x <= node.min_value){
            return;
        }

        T old_min = node.min_value;
        node.sum += (x - old_min) * static_cast<T>(node.min_count);
        if(node.max_value == old_min){
            node.max_value = x;
        }else if(
            node.max_count < node.length &&
            node.second_max_value == old_min
        ){
            node.second_max_value = x;
        }
        node.min_value = x;
    }

    void push(int k){
        if(k >= sz || data[k].length == 0){
            return;
        }

        if(data[k].add != T{}){
            apply_add(2 * k, data[k].add);
            apply_add(2 * k + 1, data[k].add);
            data[k].add = T{};
        }

        for(int child: {2 * k, 2 * k + 1}){
            if(data[child].length == 0){
                continue;
            }
            if(data[k].max_value < data[child].max_value){
                apply_chmin(child, data[k].max_value);
            }
            if(data[child].min_value < data[k].min_value){
                apply_chmax(child, data[k].min_value);
            }
        }
    }

    void range_chmin_impl(
        int ql, int qr, const T& x, int k, int l, int r
    ){
        Node& node = data[k];
        if(r <= ql || qr <= l || node.length == 0 || node.max_value <= x){
            return;
        }
        if(
            ql <= l && r <= qr &&
            (node.max_count == node.length || node.second_max_value < x)
        ){
            apply_chmin(k, x);
            return;
        }

        push(k);
        int mid = l + (r - l) / 2;
        range_chmin_impl(ql, qr, x, 2 * k, l, mid);
        range_chmin_impl(ql, qr, x, 2 * k + 1, mid, r);
        update(k);
    }

    void range_chmax_impl(
        int ql, int qr, const T& x, int k, int l, int r
    ){
        Node& node = data[k];
        if(r <= ql || qr <= l || node.length == 0 || x <= node.min_value){
            return;
        }
        if(
            ql <= l && r <= qr &&
            (node.min_count == node.length || x < node.second_min_value)
        ){
            apply_chmax(k, x);
            return;
        }

        push(k);
        int mid = l + (r - l) / 2;
        range_chmax_impl(ql, qr, x, 2 * k, l, mid);
        range_chmax_impl(ql, qr, x, 2 * k + 1, mid, r);
        update(k);
    }

    void range_add_impl(
        int ql, int qr, const T& x, int k, int l, int r
    ){
        if(r <= ql || qr <= l || data[k].length == 0){
            return;
        }
        if(ql <= l && r <= qr){
            apply_add(k, x);
            return;
        }

        push(k);
        int mid = l + (r - l) / 2;
        range_add_impl(ql, qr, x, 2 * k, l, mid);
        range_add_impl(ql, qr, x, 2 * k + 1, mid, r);
        update(k);
    }

    T range_sum_impl(int ql, int qr, int k, int l, int r){
        if(r <= ql || qr <= l || data[k].length == 0){
            return T{};
        }
        if(ql <= l && r <= qr){
            return data[k].sum;
        }

        push(k);
        int mid = l + (r - l) / 2;
        return
            range_sum_impl(ql, qr, 2 * k, l, mid) +
            range_sum_impl(ql, qr, 2 * k + 1, mid, r);
    }

    T range_min_impl(int ql, int qr, int k, int l, int r){
        if(ql <= l && r <= qr){
            return data[k].min_value;
        }

        push(k);
        int mid = l + (r - l) / 2;
        if(qr <= mid){
            return range_min_impl(ql, qr, 2 * k, l, mid);
        }
        if(mid <= ql){
            return range_min_impl(ql, qr, 2 * k + 1, mid, r);
        }
        return std::min(
            range_min_impl(ql, qr, 2 * k, l, mid),
            range_min_impl(ql, qr, 2 * k + 1, mid, r)
        );
    }

    T range_max_impl(int ql, int qr, int k, int l, int r){
        if(ql <= l && r <= qr){
            return data[k].max_value;
        }

        push(k);
        int mid = l + (r - l) / 2;
        if(qr <= mid){
            return range_max_impl(ql, qr, 2 * k, l, mid);
        }
        if(mid <= ql){
            return range_max_impl(ql, qr, 2 * k + 1, mid, r);
        }
        return std::max(
            range_max_impl(ql, qr, 2 * k, l, mid),
            range_max_impl(ql, qr, 2 * k + 1, mid, r)
        );
    }

    void set_impl(int p, const T& x, int k, int l, int r){
        if(r - l == 1){
            data[k] = make_leaf(x);
            return;
        }

        push(k);
        int mid = l + (r - l) / 2;
        if(p < mid){
            set_impl(p, x, 2 * k, l, mid);
        }else{
            set_impl(p, x, 2 * k + 1, mid, r);
        }
        update(k);
    }

    T get_impl(int p, int k, int l, int r){
        if(r - l == 1){
            return data[k].sum;
        }

        push(k);
        int mid = l + (r - l) / 2;
        if(p < mid){
            return get_impl(p, 2 * k, l, mid);
        }
        return get_impl(p, 2 * k + 1, mid, r);
    }

    void check_range(int l, int r, const char* message) const{
        if(l < 0 || r < l || _n < r)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }

    void build(){
        for(int k = sz - 1; k > 0; k--){
            update(k);
        }
    }

public:
    SegmentTreeBeats(const SegmentTreeBeats&) = delete;
    SegmentTreeBeats& operator=(const SegmentTreeBeats&) = delete;
    SegmentTreeBeats(SegmentTreeBeats&&) = delete;
    SegmentTreeBeats& operator=(SegmentTreeBeats&&) = delete;

    explicit SegmentTreeBeats(int n = MAX_SIZE): SegmentTreeBeats(n, T{}){}

    SegmentTreeBeats(int n, const T& value): _n(n){
        if(_n < 0 || MAX_SIZE < _n)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }
        for(int k = 0; k < _n; k++){
            data[k + sz] = make_leaf(value);
        }
        build();
    }

    explicit SegmentTreeBeats(const std::vector<T>& v): _n((int)v.size()){
        if(v.size() > static_cast<std::size_t>(MAX_SIZE))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }
        for(int k = 0; k < _n; k++){
            data[k + sz] = make_leaf(v[static_cast<std::size_t>(k)]);
        }
        build();
    }

    template<std::size_t N>
    explicit SegmentTreeBeats(const std::array<T, N>& v): _n((int)N){
        if(N > static_cast<std::size_t>(MAX_SIZE))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }
        for(int k = 0; k < _n; k++){
            data[k + sz] = make_leaf(v[static_cast<std::size_t>(k)]);
        }
        build();
    }

    int size() const{
        return _n;
    }

    void range_chmin(int l, int r, const T& x){
        check_range(
            l, r,
            "library assertion fault: range violation (range_chmin)."
        );
        if(l == r){
            return;
        }
        range_chmin_impl(l, r, x, 1, 0, sz);
    }

    void range_chmax(int l, int r, const T& x){
        check_range(
            l, r,
            "library assertion fault: range violation (range_chmax)."
        );
        if(l == r){
            return;
        }
        range_chmax_impl(l, r, x, 1, 0, sz);
    }

    void range_add(int l, int r, const T& x){
        check_range(
            l, r,
            "library assertion fault: range violation (range_add)."
        );
        if(l == r){
            return;
        }
        range_add_impl(l, r, x, 1, 0, sz);
    }

    T range_sum(int l, int r){
        check_range(
            l, r,
            "library assertion fault: range violation (range_sum)."
        );
        if(l == r){
            return T{};
        }
        return range_sum_impl(l, r, 1, 0, sz);
    }

    T range_min(int l, int r){
        check_range(
            l, r,
            "library assertion fault: range violation (range_min)."
        );
        if(l == r)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: empty range (range_min)."
            );
        }
        return range_min_impl(l, r, 1, 0, sz);
    }

    T range_max(int l, int r){
        check_range(
            l, r,
            "library assertion fault: range violation (range_max)."
        );
        if(l == r)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: empty range (range_max)."
            );
        }
        return range_max_impl(l, r, 1, 0, sz);
    }

    void set(int k, const T& x){
        if(k < 0 || _n <= k)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (set)."
            );
        }
        set_impl(k, x, 1, 0, sz);
    }

    T get(int k){
        if(k < 0 || _n <= k)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (get)."
            );
        }
        return get_impl(k, 1, 0, sz);
    }

    T all_sum() const{
        return data[1].sum;
    }

    T all_min() const{
        if(_n == 0)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: empty range (all_min)."
            );
        }
        return data[1].min_value;
    }

    T all_max() const{
        if(_n == 0)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: empty range (all_max)."
            );
        }
        return data[1].max_value;
    }
};
