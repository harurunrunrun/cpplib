#pragma once

#include "partially_persistent_dynamic_segtree.hpp"
#include "wide_segtree_persistence_common.hpp"

template<long long MAX_SIZE, int MAX_NODES, int MAX_VERSIONS, int MAX_CHANGES>
struct PartiallyPersistentWideSegtree{
private:
    PartiallyPersistentDynamicSegtree<wide_segtree_persistence_internal::sum_monoid, MAX_SIZE, MAX_NODES, MAX_VERSIONS, MAX_CHANGES> tree;
public:
    int versions() const{ return tree.versions(); }
    int latest_version() const{ return tree.latest_version(); }
    int nodes_used() const{ return tree.nodes_used(); }
    int changes_used() const{ return tree.changes_used(); }
    int set(long long index, long long value){ return tree.set(index, value); }
    int add(long long index, long long value){ return tree.set(index, tree.get(index) + value); }
    long long get(int version, long long index) const{ return tree.get(version, index); }
    long long get(long long index) const{ return tree.get(index); }
    long long prod(int version, long long left, long long right) const{ return tree.prod(version, left, right); }
    long long prod(long long left, long long right) const{ return tree.prod(left, right); }
    long long all_prod(int version) const{ return tree.all_prod(version); }
    long long all_prod() const{ return tree.all_prod(); }
};

template<long long MAX_SIZE, int MAX_NODES, int MAX_VERSIONS, int MAX_CHANGES>
using PartiallyPersistentWide_Segtree = PartiallyPersistentWideSegtree<MAX_SIZE, MAX_NODES, MAX_VERSIONS, MAX_CHANGES>;
