#ifndef CPPLIB_SRC_STRUCTURE_SEGTREE_PERSISTENT_WIDESEGTREE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_SEGTREE_PERSISTENT_WIDESEGTREE_HPP_INCLUDED

#include "persistent_dynamic_segtree.hpp"
#include "wide_segtree_persistence_common.hpp"

template<long long MAX_SIZE, int MAX_NODES, int MAX_VERSIONS>
struct PersistentWideSegtree{
private:
    PersistentDynamicSegtree<wide_segtree_persistence_internal::sum_monoid, MAX_SIZE, MAX_NODES, MAX_VERSIONS> tree;
public:
    int versions() const{ return tree.versions(); }
    int latest_version() const{ return tree.latest_version(); }
    int nodes_used() const{ return tree.nodes_used(); }
    int set(int version, long long index, long long value){ return tree.set(version, index, value); }
    int set(long long index, long long value){ return tree.set(index, value); }
    int add(int version, long long index, long long value){ return tree.set(version, index, tree.get(version, index) + value); }
    int add(long long index, long long value){ return add(latest_version(), index, value); }
    int fork(int version){ return tree.fork(version); }
    long long get(int version, long long index) const{ return tree.get(version, index); }
    long long get(long long index) const{ return tree.get(index); }
    long long prod(int version, long long left, long long right) const{ return tree.prod(version, left, right); }
    long long prod(long long left, long long right) const{ return tree.prod(left, right); }
    long long all_prod(int version) const{ return tree.all_prod(version); }
    long long all_prod() const{ return tree.all_prod(); }
};

template<long long MAX_SIZE, int MAX_NODES, int MAX_VERSIONS>
using PersistentWide_Segtree = PersistentWideSegtree<MAX_SIZE, MAX_NODES, MAX_VERSIONS>;

#endif  // CPPLIB_SRC_STRUCTURE_SEGTREE_PERSISTENT_WIDESEGTREE_HPP_INCLUDED
