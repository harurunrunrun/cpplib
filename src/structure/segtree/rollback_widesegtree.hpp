#pragma once

#include "rollback_dynamic_segtree.hpp"
#include "wide_segtree_persistence_common.hpp"

template<long long MAX_SIZE, int MAX_NODES, int MAX_CHANGES>
struct RollbackWideSegtree{
private:
    RollbackDynamicSegtree<wide_segtree_persistence_internal::sum_monoid, MAX_SIZE, MAX_NODES, MAX_CHANGES> tree;
public:
    using Snapshot = typename decltype(tree)::Snapshot;
    int history_size() const{ return tree.history_size(); }
    int nodes_used() const{ return tree.nodes_used(); }
    int changes_used() const{ return tree.changes_used(); }
    bool can_undo() const{ return tree.can_undo(); }
    void set(long long index, long long value){ tree.set(index, value); }
    void add(long long index, long long value){ tree.set(index, tree.get(index) + value); }
    long long get(long long index) const{ return tree.get(index); }
    long long prod(long long left, long long right) const{ return tree.prod(left, right); }
    long long all_prod() const{ return tree.all_prod(); }
    Snapshot snapshot() const{ return tree.snapshot(); }
    void undo(){ tree.undo(); }
    void rollback(Snapshot snapshot){ tree.rollback(snapshot); }
};

template<long long MAX_SIZE, int MAX_NODES, int MAX_CHANGES>
using RollbackWide_Segtree = RollbackWideSegtree<MAX_SIZE, MAX_NODES, MAX_CHANGES>;
