#ifndef CPPLIB_SRC_STRUCTURE_ARRAY_PERSISTENT_ARRAY_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_ARRAY_PERSISTENT_ARRAY_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <limits>
#include <optional>
#include <stdexcept>
#include <vector>

template<class T, int MAX_SIZE, int MAX_VERSION>
struct PersistentArray{
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
        std::numeric_limits<std::size_t>::max() / nodes_per_update
    );

    static constexpr std::size_t node_capacity =
        static_cast<std::size_t>(MAX_VERSION) * nodes_per_update;

    static_assert(
        node_capacity <= static_cast<std::size_t>(std::numeric_limits<int>::max())
    );

    struct Node{
        int left;
        int right;
    };

    int _n = 0;
    int version_count = 1;
    int node_count = 0;
    int value_count = 0;

    std::array<std::optional<T>, MAX_SIZE> initial;
    std::array<int, static_cast<std::size_t>(MAX_VERSION) + 1> root;
    std::array<Node, node_capacity> nodes;
    std::array<std::optional<T>, MAX_VERSION> updated_values;

    void initialize(int n){
        if(n < 0 || MAX_SIZE < n)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }

        _n = n;
        root.fill(-1);
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
            nodes[res] = {-1, -1};
        }else{
            nodes[res] = nodes[source];
        }
        return res;
    }

    int set_impl(int source, int l, int r, int k, int value_index){
        int current = new_node(source);

        if(r - l == 1){
            nodes[current].left = value_index;
            return current;
        }

        int mid = l + (r - l) / 2;
        if(k < mid){
            int child = source == -1 ? -1 : nodes[source].left;
            nodes[current].left = set_impl(child, l, mid, k, value_index);
        }else{
            int child = source == -1 ? -1 : nodes[source].right;
            nodes[current].right = set_impl(child, mid, r, k, value_index);
        }
        return current;
    }

public:
    PersistentArray(const PersistentArray&) = delete;
    PersistentArray& operator=(const PersistentArray&) = delete;
    PersistentArray(PersistentArray&&) = delete;
    PersistentArray& operator=(PersistentArray&&) = delete;

    explicit PersistentArray(int n = MAX_SIZE){
        initialize(n);
        for(int i = 0; i < _n; i++){
            initial[i].emplace();
        }
    }

    PersistentArray(int n, const T& value){
        initialize(n);
        for(int i = 0; i < _n; i++){
            initial[i].emplace(value);
        }
    }

    explicit PersistentArray(const std::vector<T>& v){
        if(v.size() > static_cast<std::size_t>(MAX_SIZE))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }

        initialize(static_cast<int>(v.size()));
        for(int i = 0; i < _n; i++){
            initial[i].emplace(v[static_cast<std::size_t>(i)]);
        }
    }

    template<std::size_t N>
    explicit PersistentArray(const std::array<T, N>& v){
        if(N > static_cast<std::size_t>(MAX_SIZE))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }

        initialize(static_cast<int>(N));
        for(int i = 0; i < _n; i++){
            initial[i].emplace(v[static_cast<std::size_t>(i)]);
        }
    }

    int size() const{
        return _n;
    }

    int versions() const{
        return version_count;
    }

    int latest_version() const{
        return version_count - 1;
    }

    const T& get(int version, int k) const{
        check_version(
            version,
            "library assertion fault: range violation (get)."
        );
        if(k < 0 || _n <= k)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (get)."
            );
        }

        int current = root[version];
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

        if(current != -1){
            int value_index = nodes[current].left;
            if(value_index != -1){
                return *updated_values[value_index];
            }
        }
        return *initial[k];
    }

    int set(int version, int k, const T& x){
        check_version(
            version,
            "library assertion fault: range violation (set)."
        );
        if(k < 0 || _n <= k)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (set)."
            );
        }
        if(version_count > MAX_VERSION)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: version capacity exceeded (set)."
            );
        }

        int new_value_index = value_count;
        updated_values[new_value_index].emplace(x);
        value_count++;

        int node_snapshot = node_count;
        int new_root;
        try{
            new_root = set_impl(
                root[version], 0, MAX_SIZE, k, new_value_index
            );
        }catch(...){
            node_count = node_snapshot;
            value_count--;
            updated_values[value_count].reset();
            throw;
        }

        int new_version = version_count++;
        root[new_version] = new_root;
        return new_version;
    }

    int fork(int version){
        check_version(
            version,
            "library assertion fault: range violation (fork)."
        );
        if(version_count > MAX_VERSION)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: version capacity exceeded (fork)."
            );
        }

        int new_version = version_count++;
        root[new_version] = root[version];
        return new_version;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_ARRAY_PERSISTENT_ARRAY_HPP_INCLUDED
