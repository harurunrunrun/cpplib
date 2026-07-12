---
title: Rollback Dynamic Lazy Segment Tree
documentation_of: ../src/structure/segtree/rollback_dynamic_lazysegtree.hpp
---

巨大な添字範囲を扱うrollback可能な遅延セグメント木。

```cpp
RollbackDynamicLazySegtree<MonoidActLen, MAX_SIZE, MAX_NODES, MAX_CHANGES>
```

```cpp
void set(long long index, S value)
void apply(long long index, T action)
void apply(long long l, long long r, T action)
S get(long long index)
S prod(long long l, long long r)
S all_prod()
Snapshot snapshot()
void undo()
void rollback(Snapshot snapshot)
```

`MAX_CHANGES`はノード状態と疎な子リンクの変更履歴数の上限。更新・`undo`は $O(\log MAX\_SIZE)$。`rollback`は取り消す更新回数に比例する。
