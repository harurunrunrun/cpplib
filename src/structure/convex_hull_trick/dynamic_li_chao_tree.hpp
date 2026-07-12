#pragma once

#include <algorithm>
#include <array>
#include <limits>
#include <numeric>
#include <stdexcept>

template<long long X_MIN, long long X_MAX, int MAX_NODE>
struct DynamicLiChaoTree{
    static_assert(X_MIN <= X_MAX);
    static_assert(MAX_NODE >= 0);

    static constexpr long long INF =
        std::numeric_limits<long long>::max() / 4;

    struct Line{
        long long a;
        long long b;

        __int128 eval128(long long x) const{
            return static_cast<__int128>(a) * x + b;
        }

        long long eval(long long x) const{
            __int128 y = eval128(x);
            if(y > INF) return INF;
            if(y < -static_cast<__int128>(INF)) return -INF;
            return static_cast<long long>(y);
        }
    };

private:
    struct Node{
        Line line;
        int left;
        int right;
    };

    int root = -1;
    int node_count = 0;
    std::array<Node, MAX_NODE> nodes;

    static long long midpoint(long long l, long long r){
        return std::midpoint(l, r);
    }

    static bool less_at(const Line& f, const Line& g, long long x){
        return f.eval128(x) < g.eval128(x);
    }

    int new_node(Line line){
        if(node_count >= MAX_NODE)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: node capacity exceeded (add_line)."
            );
        }

        int res = node_count++;
        nodes[res] = {line, -1, -1};
        return res;
    }

    void add_line_impl(int& current, long long l, long long r, Line line){
        if(current == -1){
            current = new_node(line);
            return;
        }

        long long m = midpoint(l, r);

        bool left_better = less_at(line, nodes[current].line, l);
        bool mid_better = less_at(line, nodes[current].line, m);

        if(mid_better) std::swap(line, nodes[current].line);
        if(l == r) return;

        if(left_better != mid_better){
            add_line_impl(nodes[current].left, l, m, line);
        }else{
            add_line_impl(nodes[current].right, m + 1, r, line);
        }
    }

public:
    int nodes_used() const{ return node_count; }
    bool empty() const{ return root == -1; }

    void add_line(long long a, long long b){
        add_line({a, b});
    }

    void add_line(Line line){
        add_line_impl(root, X_MIN, X_MAX, line);
    }

    long long query(long long x) const{
        if(x < X_MIN || X_MAX < x)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (query)."
            );
        }

        long long res = INF;
        int current = root;
        long long l = X_MIN;
        long long r = X_MAX;

        while(current != -1){
            res = std::min(res, nodes[current].line.eval(x));
            if(l == r) break;

            long long m = midpoint(l, r);
            if(x <= m){
                current = nodes[current].left;
                r = m;
            }else{
                current = nodes[current].right;
                l = m + 1;
            }
        }

        return res;
    }
};
