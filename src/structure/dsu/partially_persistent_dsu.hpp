#ifndef CPPLIB_SRC_STRUCTURE_DSU_PARTIALLY_PERSISTENT_DSU_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_DSU_PARTIALLY_PERSISTENT_DSU_HPP_INCLUDED

#include <array>
#include <limits>
#include <stdexcept>
#include <utility>
#include "../other/partially_persistent_storage.hpp"

template<int MAX_SIZE, int MAX_UPDATE>
struct PartiallyPersistentDSU{
    static_assert(MAX_SIZE > 0);
    static_assert(MAX_UPDATE >= 0);

private:
    static constexpr int INF = std::numeric_limits<int>::max();

    int _n = 0;
    int version_count = 1;
    std::array<int, MAX_SIZE> parent{};
    std::array<int, MAX_SIZE> parent_time{};
    std::array<int, static_cast<std::size_t>(MAX_UPDATE) + 1> group_count{};
    PartiallyPersistentStorage<int, MAX_SIZE, MAX_UPDATE> component_size_history;

    void check_vertex(int v, const char* message) const{
        if(v < 0 || _n <= v)[[unlikely]] throw std::runtime_error(message);
    }
    void check_version(int version, const char* message) const{
        if(version < 0 || version_count <= version)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }
    int leader_unchecked(int version, int v) const{
        while(parent_time[v] <= version) v = parent[v];
        return v;
    }

public:
    explicit PartiallyPersistentDSU(int n = MAX_SIZE): _n(n){
        if(n < 0 || MAX_SIZE < n)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
        for(int i = 0; i < _n; i++){
            parent[i] = i;
            parent_time[i] = INF;
            component_size_history.initialize(i, 1);
        }
        group_count[0] = _n;
    }

    int size() const{ return _n; }
    int versions() const{ return version_count; }
    int latest_version() const{ return version_count - 1; }

    int leader(int version, int v) const{
        check_version(version, "library assertion fault: range violation (leader).");
        check_vertex(v, "library assertion fault: range violation (leader).");
        return leader_unchecked(version, v);
    }
    bool same(int version, int u, int v) const{
        check_version(version, "library assertion fault: range violation (same).");
        check_vertex(u, "library assertion fault: range violation (same).");
        check_vertex(v, "library assertion fault: range violation (same).");
        return leader_unchecked(version, u) == leader_unchecked(version, v);
    }
    int component_size(int version, int v) const{
        check_version(version, "library assertion fault: range violation (component_size).");
        check_vertex(v, "library assertion fault: range violation (component_size).");
        int r = leader_unchecked(version, v);
        return component_size_history.get(r, version);
    }
    int groups(int version) const{
        check_version(version, "library assertion fault: range violation (groups).");
        return group_count[static_cast<std::size_t>(version)];
    }

    int merge(int u, int v){
        check_vertex(u, "library assertion fault: range violation (merge).");
        check_vertex(v, "library assertion fault: range violation (merge).");
        if(version_count > MAX_UPDATE)[[unlikely]]{
            throw std::runtime_error("library assertion fault: version capacity exceeded (merge).");
        }
        int new_version = version_count;
        int prev_version = version_count - 1;
        u = leader_unchecked(prev_version, u);
        v = leader_unchecked(prev_version, v);
        group_count[static_cast<std::size_t>(new_version)] =
            group_count[static_cast<std::size_t>(prev_version)];
        if(u != v){
            int size_u = component_size_history.get(u, prev_version);
            int size_v = component_size_history.get(v, prev_version);
            if(size_u < size_v){
                std::swap(u, v);
                std::swap(size_u, size_v);
            }
            parent[v] = u;
            parent_time[v] = new_version;
            component_size_history.write(u, new_version, size_u + size_v);
            group_count[static_cast<std::size_t>(new_version)]--;
        }
        version_count++;
        return new_version;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_DSU_PARTIALLY_PERSISTENT_DSU_HPP_INCLUDED
