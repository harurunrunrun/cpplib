#ifndef CPPLIB_SRC_STRUCTURE_CONVEX_HULL_TRICK_PERSISTENT_DYNAMIC_LI_CHAO_TREE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_CONVEX_HULL_TRICK_PERSISTENT_DYNAMIC_LI_CHAO_TREE_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <limits>
#include <numeric>
#include <stdexcept>

template<long long X_MIN, long long X_MAX, int MAX_VERSION>
struct PersistentDynamicLiChaoTree{
    static_assert(X_MIN <= X_MAX);
    static_assert(MAX_VERSION >= 0);

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
            if(y > std::numeric_limits<long long>::max()){
                return std::numeric_limits<long long>::max();
            }
            if(y < std::numeric_limits<long long>::min()){
                return std::numeric_limits<long long>::min();
            }
            return static_cast<long long>(y);
        }
    };

private:
    static constexpr int height = []{
        long long l = X_MIN;
        long long r = X_MAX;
        int h = 0;
        while(l < r){
            long long m = std::midpoint(l, r);
            l = m + 1;
            h++;
        }
        return h;
    }();

    static constexpr int nodes_per_update = height + 1;
    static constexpr int node_capacity = MAX_VERSION * nodes_per_update;

    struct Node{
        Line line;
        int left;
        int right;
    };

    int version_count = 1;
    int node_count = 0;

    std::array<int, static_cast<std::size_t>(MAX_VERSION) + 1> root;
    std::array<Node, static_cast<std::size_t>(node_capacity)> nodes;

    static long long midpoint(long long l, long long r){
        return std::midpoint(l, r);
    }

    static bool less_at(const Line& f, const Line& g, long long x){
        return f.eval128(x) < g.eval128(x);
    }

    void check_version(int version, const char* message) const{
        if(version < 0 || version_count <= version)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }

    int new_node(Line line){
        if(node_count >= node_capacity)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: node capacity exceeded (add_line)."
            );
        }

        int res = node_count++;
        nodes[res] = {line, -1, -1};
        return res;
    }

    int clone_node(int source){
        if(node_count >= node_capacity)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: node capacity exceeded (add_line)."
            );
        }

        int res = node_count++;
        nodes[res] = nodes[source];
        return res;
    }

    int add_line_impl(int source, long long l, long long r, Line line){
        if(source == -1) return new_node(line);

        int current = clone_node(source);
        long long m = midpoint(l, r);

        bool left_better = less_at(line, nodes[current].line, l);
        bool mid_better = less_at(line, nodes[current].line, m);

        if(mid_better) std::swap(line, nodes[current].line);
        if(l == r) return current;

        if(left_better != mid_better){
            nodes[current].left =
                add_line_impl(nodes[current].left, l, m, line);
        }else{
            nodes[current].right =
                add_line_impl(nodes[current].right, m + 1, r, line);
        }
        return current;
    }

    int append_version(int new_root, const char* message){
        if(version_count > MAX_VERSION)[[unlikely]]{
            throw std::runtime_error(message);
        }

        int res = version_count++;
        root[res] = new_root;
        return res;
    }

public:
    PersistentDynamicLiChaoTree(){
        root.fill(-1);
    }

    int versions() const{ return version_count; }
    int latest_version() const{ return version_count - 1; }
    int nodes_used() const{ return node_count; }

    bool empty(int version) const{
        check_version(version, "library assertion fault: range violation (empty).");
        return root[version] == -1;
    }

    int fork(int version){
        check_version(version, "library assertion fault: range violation (fork).");
        return append_version(
            root[version],
            "library assertion fault: version capacity exceeded (fork)."
        );
    }

    int add_line(int version, long long a, long long b){
        return add_line(version, {a, b});
    }

    int add_line(int version, Line line){
        check_version(
            version,
            "library assertion fault: range violation (add_line)."
        );
        if(version_count > MAX_VERSION)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: version capacity exceeded (add_line)."
            );
        }

        int node_snapshot = node_count;
        int new_root;
        try{
            new_root = add_line_impl(root[version], X_MIN, X_MAX, line);
        }catch(...){
            node_count = node_snapshot;
            throw;
        }

        return append_version(
            new_root,
            "library assertion fault: version capacity exceeded (add_line)."
        );
    }

    long long query(int version, long long x) const{
        check_version(version, "library assertion fault: range violation (query).");
        if(x < X_MIN || X_MAX < x)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (query)."
            );
        }

        int current = root[version];
        if(current == -1) return INF;
        long long res = std::numeric_limits<long long>::max();
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

#endif  // CPPLIB_SRC_STRUCTURE_CONVEX_HULL_TRICK_PERSISTENT_DYNAMIC_LI_CHAO_TREE_HPP_INCLUDED
