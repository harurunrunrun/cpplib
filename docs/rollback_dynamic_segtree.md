---
title: Rollback Dynamic Segment Tree
documentation_of: ../src/structure/segtree/rollback_dynamic_segtree.hpp
---

巨大な添字範囲を扱うrollback可能セグメント木。

```cpp
RollbackDynamicSegtree<Monoid, MAX_SIZE, MAX_NODES, MAX_CHANGES>
```

```cpp
void set(long long index, S value)
S get(long long index)
S prod(long long l, long long r)
S all_prod()
Snapshot snapshot()
void undo()
void rollback(Snapshot snapshot)
```

`MAX_NODES`は疎ノード数、`MAX_CHANGES`はノード変更履歴数の上限。更新と`undo`は $O(\log MAX\_SIZE)$。`rollback`は取り消す更新回数に比例する。
