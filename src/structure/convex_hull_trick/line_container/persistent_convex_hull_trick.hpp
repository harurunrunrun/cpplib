#ifndef CPPLIB_SRC_STRUCTURE_CONVEX_HULL_TRICK_LINE_CONTAINER_PERSISTENT_CONVEX_HULL_TRICK_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_CONVEX_HULL_TRICK_LINE_CONTAINER_PERSISTENT_CONVEX_HULL_TRICK_HPP_INCLUDED

#include <array>
#include "../../array/static_or_vector.hpp"
#include <limits>
#include <stdexcept>

template<int MAX_LINE = -1, int MAX_VERSION = -1, int MAX_NODE = -1>
struct PersistentConvexHullTrick{
    static_assert(MAX_LINE == -1 || MAX_LINE >= 0);
    static_assert(MAX_VERSION == -1 || MAX_VERSION > 0);
    static_assert(MAX_NODE == -1 || MAX_NODE > 0);

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
    struct Node{
        int left = 0;
        int right = 0;
        Line line{0, INF};
    };

    int version_count = 1;
    int node_count = 1;
    cpplib_detail::StaticOrVector<int, MAX_VERSION> roots{};
    cpplib_detail::StaticOrVector<int, MAX_VERSION> sizes{};
    cpplib_detail::StaticOrVector<Node, MAX_NODE> nodes{};

    cpplib_detail::StaticOrVector<long long, (MAX_LINE < 0 ? -1 : 0)> domains;

    long long domain_of(int version) const{
        if constexpr(MAX_LINE < 0) return domains[static_cast<std::size_t>(version)];
        else return MAX_LINE;
    }

    static bool product_ge(
        __int128 lhs_x, __int128 lhs_y,
        __int128 rhs_x, __int128 rhs_y
    ){
        bool lhs_negative = (lhs_x < 0) != (lhs_y < 0);
        bool rhs_negative = (rhs_x < 0) != (rhs_y < 0);
        unsigned __int128 lhs = static_cast<unsigned __int128>(
            lhs_x < 0 ? -lhs_x : lhs_x) * static_cast<unsigned __int128>(
            lhs_y < 0 ? -lhs_y : lhs_y);
        unsigned __int128 rhs = static_cast<unsigned __int128>(
            rhs_x < 0 ? -rhs_x : rhs_x) * static_cast<unsigned __int128>(
            rhs_y < 0 ? -rhs_y : rhs_y);
        if(lhs == 0) lhs_negative = false;
        if(rhs == 0) rhs_negative = false;
        if(lhs_negative != rhs_negative) return !lhs_negative;
        return lhs_negative ? lhs <= rhs : lhs >= rhs;
    }

    static bool unnecessary(const Line& f, const Line& g, const Line& h){
        return product_ge(
            static_cast<__int128>(g.b) - f.b,
            static_cast<__int128>(g.a) - h.a,
            static_cast<__int128>(h.b) - g.b,
            static_cast<__int128>(f.a) - g.a
        );
    }
    void check_version(int version) const{
        if(version < 0 || version_count <= version)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (version).");
        }
    }
    void check_not_empty(int version, const char* message) const{
        if(sizes[version] == 0)[[unlikely]] throw std::runtime_error(message);
    }
    int new_node(int from = 0){
        if constexpr(MAX_NODE < 0) nodes.ensure(cpplib_detail::checked_int_size(static_cast<std::size_t>(node_count) + 1));
        if(node_count == MAX_NODE)[[unlikely]]{
            throw std::runtime_error("library assertion fault: node capacity exceeded.");
        }
        int v = node_count++;
        nodes[v] = nodes[from];
        return v;
    }
    int new_version(int root, int size, long long domain){
        if(version_count == MAX_VERSION)[[unlikely]]{
            throw std::runtime_error("library assertion fault: version capacity exceeded.");
        }
        if constexpr(MAX_VERSION < 0){
            const int next = cpplib_detail::checked_int_size(static_cast<std::size_t>(version_count) + 1);
            roots.ensure(next); sizes.ensure(next);
        }
        if constexpr(MAX_LINE < 0){
            domains.ensure(cpplib_detail::checked_int_size(static_cast<std::size_t>(version_count) + 1));
            domains[version_count] = domain;
        }
        roots[version_count] = root;
        sizes[version_count] = size;
        return version_count++;
    }
    int set_line(int v, long long l, long long r, int p, Line line){
        int u = new_node(v);
        if(r - l == 1){
            nodes[u].line = line;
            return u;
        }
        long long m = (l + r) >> 1;
        if(p < m) nodes[u].left = set_line(nodes[u].left, l, m, p, line);
        else nodes[u].right = set_line(nodes[u].right, m, r, p, line);
        return u;
    }
    Line get_line(int v, long long l, long long r, int p) const{
        if(r - l == 1) return nodes[v].line;
        long long m = (l + r) >> 1;
        if(p < m) return get_line(nodes[v].left, l, m, p);
        return get_line(nodes[v].right, m, r, p);
    }
    Line line_at_root(int root, int index, long long domain) const{
        return get_line(root, 0, domain, index);
    }

public:
    PersistentConvexHullTrick(){
        roots.ensure(1); sizes.ensure(1); nodes.ensure(1);
        if constexpr(MAX_LINE < 0){domains.ensure(1); domains[0] = 1;}
    }

    int versions() const{ return version_count; }
    int nodes_used() const{ return node_count; }
    int size(int version) const{
        check_version(version);
        return sizes[version];
    }
    bool empty(int version) const{
        check_version(version);
        return sizes[version] == 0;
    }
    int fork(int version){
        check_version(version);
        return new_version(roots[version], sizes[version], domain_of(version));
    }

    int add_line(int version, long long a, long long b){
        return add_line(version, Line{a, b});
    }

    int add_line(int version, Line line){
        check_version(version);
        int root = roots[version];
        long long domain = domain_of(version);
        int count = sizes[version];

        if(count > 0){
            Line last = line_at_root(root, count - 1, domain);
            if(line.a > last.a)[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: monotonicity violation (add_line)."
                );
            }
            if(line.a == last.a){
                if(last.b <= line.b) return fork(version);
                count--;
            }
        }


        while(count >= 2){
            Line f = line_at_root(root, count - 2, domain);
            Line g = line_at_root(root, count - 1, domain);
            if(!unnecessary(f, g, line)) break;
            count--;
        }
        if((MAX_LINE >= 0 && count >= MAX_LINE))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: line capacity exceeded (add_line)."
            );
        }
        if(version_count == MAX_VERSION)[[unlikely]]{
            throw std::runtime_error("library assertion fault: version capacity exceeded.");
        }
        int node_snapshot = node_count;
        try{
            if constexpr(MAX_LINE < 0){
                cpplib_detail::checked_int_size(static_cast<std::size_t>(count) + 1);
                while(count >= domain){
                    const int expanded = new_node();
                    nodes[expanded].left = root;
                    root = expanded;
                    domain *= 2;
                }
            }
            root = set_line(root, 0, domain, count, line);
            return new_version(root, count + 1, domain);
        }catch(...){
            node_count = node_snapshot;
            throw;
        }
    }

    Line line_at(int version, int index) const{
        check_version(version);
        if(index < 0 || sizes[version] <= index)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (line_at).");
        }
        return line_at_root(roots[version], index, domain_of(version));
    }

    long long query(int version, long long x) const{
        check_version(version);
        check_not_empty(version, "library assertion fault: empty hull (query).");

        int low = 0;
        int high = sizes[version] - 1;
        int root = roots[version];
        long long domain = domain_of(version);
        while(low < high){
            int mid = (low + high) / 2;
            Line f = line_at_root(root, mid, domain);
            Line g = line_at_root(root, mid + 1, domain);
            if(f.eval128(x) <= g.eval128(x)){
                high = mid;
            }else{
                low = mid + 1;
            }
        }
        return line_at_root(root, low, domain).eval(x);
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_CONVEX_HULL_TRICK_LINE_CONTAINER_PERSISTENT_CONVEX_HULL_TRICK_HPP_INCLUDED
