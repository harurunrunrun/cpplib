#ifndef CPPLIB_SRC_STRUCTURE_DSU_PERSISTENT_DSU_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_DSU_PERSISTENT_DSU_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <utility>

template<int MAX_SIZE, int MAX_VERSION>
struct PersistentDSU{
    static_assert(MAX_SIZE > 0);
    static_assert(MAX_VERSION >= 0);
    static_assert(MAX_VERSION < std::numeric_limits<int>::max());

private:
    static constexpr int height = []{
        std::size_t width = 1;
        int h = 0;
        while(width < static_cast<std::size_t>(MAX_SIZE)){
            width <<= 1;
            h++;
        }
        return h;
    }();

    static constexpr std::size_t nodes_per_update =
        static_cast<std::size_t>(height) + 1;

    static_assert(
        static_cast<std::size_t>(MAX_VERSION) <=
        std::numeric_limits<std::size_t>::max() / 2 / nodes_per_update
    );

    static constexpr std::size_t node_capacity =
        static_cast<std::size_t>(MAX_VERSION) * 2 * nodes_per_update;

    static_assert(
        node_capacity <= static_cast<std::size_t>(std::numeric_limits<int>::max())
    );

    struct Node{
        int left;
        int right;
        int value;
    };

    int _n = 0;
    int version_count = 1;
    int node_count = 0;

    std::array<int, static_cast<std::size_t>(MAX_VERSION) + 1> root;
    std::array<int, static_cast<std::size_t>(MAX_VERSION) + 1> group_count;
    std::array<Node, node_capacity> nodes;

    void check_vertex(int v, const char* message) const{
        if(v < 0 || _n <= v)[[unlikely]] throw std::runtime_error(message);
    }

    void check_version(int version, const char* message) const{
        if(version < 0 || version_count <= version)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }

    int new_node(int source){
        if(static_cast<std::size_t>(node_count) >= node_capacity)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: node capacity exceeded (set)."
            );
        }

        int res = node_count++;
        if(source == -1){
            nodes[res] = {-1, -1, -1};
        }else{
            nodes[res] = nodes[source];
        }
        return res;
    }

    int set_impl(int source, int l, int r, int k, int x){
        int current = new_node(source);

        if(r - l == 1){
            nodes[current].value = x;
            return current;
        }

        int mid = l + (r - l) / 2;
        if(k < mid){
            int child = source == -1 ? -1 : nodes[source].left;
            nodes[current].left = set_impl(child, l, mid, k, x);
        }else{
            int child = source == -1 ? -1 : nodes[source].right;
            nodes[current].right = set_impl(child, mid, r, k, x);
        }
        return current;
    }

    int set_value(int source_root, int k, int x){
        return set_impl(source_root, 0, MAX_SIZE, k, x);
    }

    int get_value(int source_root, int k) const{
        int current = source_root;
        int l = 0;
        int r = MAX_SIZE;

        while(current != -1 && r - l > 1){
            int mid = l + (r - l) / 2;
            if(k < mid){
                current = nodes[current].left;
                r = mid;
            }else{
                current = nodes[current].right;
                l = mid;
            }
        }

        if(current == -1) return -1;
        return nodes[current].value;
    }

    int leader_unchecked(int source_root, int v) const{
        while(true){
            int p = get_value(source_root, v);
            if(p < 0) return v;
            v = p;
        }
    }

    int append_version(int source_root, int groups, const char* message){
        if(version_count > MAX_VERSION)[[unlikely]]{
            throw std::runtime_error(message);
        }

        int res = version_count++;
        root[res] = source_root;
        group_count[res] = groups;
        return res;
    }

    void check_version_capacity(const char* message) const{
        if(version_count > MAX_VERSION)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }

public:
    explicit PersistentDSU(int n = MAX_SIZE): _n(n){
        if(n < 0 || MAX_SIZE < n)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }
        root.fill(-1);
        group_count.fill(0);
        group_count[0] = _n;
    }

    int size() const{ return _n; }
    int versions() const{ return version_count; }
    int latest_version() const{ return version_count - 1; }
    int nodes_used() const{ return node_count; }

    int groups(int version) const{
        check_version(version, "library assertion fault: range violation (groups).");
        return group_count[version];
    }

    int leader(int version, int v) const{
        check_version(version, "library assertion fault: range violation (leader).");
        check_vertex(v, "library assertion fault: range violation (leader).");
        return leader_unchecked(root[version], v);
    }

    bool same(int version, int u, int v) const{
        check_version(version, "library assertion fault: range violation (same).");
        check_vertex(u, "library assertion fault: range violation (same).");
        check_vertex(v, "library assertion fault: range violation (same).");
        int source_root = root[version];
        return leader_unchecked(source_root, u) == leader_unchecked(source_root, v);
    }

    int component_size(int version, int v) const{
        check_version(
            version,
            "library assertion fault: range violation (component_size)."
        );
        check_vertex(
            v,
            "library assertion fault: range violation (component_size)."
        );
        int source_root = root[version];
        int r = leader_unchecked(source_root, v);
        return -get_value(source_root, r);
    }

    int fork(int version){
        check_version(version, "library assertion fault: range violation (fork).");
        return append_version(
            root[version],
            group_count[version],
            "library assertion fault: version capacity exceeded (fork)."
        );
    }

    int merge(int version, int u, int v){
        check_version(version, "library assertion fault: range violation (merge).");
        check_vertex(u, "library assertion fault: range violation (merge).");
        check_vertex(v, "library assertion fault: range violation (merge).");
        check_version_capacity(
            "library assertion fault: version capacity exceeded (merge)."
        );

        int source_root = root[version];
        u = leader_unchecked(source_root, u);
        v = leader_unchecked(source_root, v);

        if(u == v){
            return append_version(
                source_root,
                group_count[version],
                "library assertion fault: version capacity exceeded (merge)."
            );
        }

        int size_u = -get_value(source_root, u);
        int size_v = -get_value(source_root, v);
        if(size_u < size_v){
            std::swap(u, v);
            std::swap(size_u, size_v);
        }

        int node_snapshot = node_count;
        int new_root;
        try{
            new_root = set_value(source_root, u, -(size_u + size_v));
            new_root = set_value(new_root, v, u);
        }catch(...){
            node_count = node_snapshot;
            throw;
        }

        return append_version(
            new_root,
            group_count[version] - 1,
            "library assertion fault: version capacity exceeded (merge)."
        );
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_DSU_PERSISTENT_DSU_HPP_INCLUDED
