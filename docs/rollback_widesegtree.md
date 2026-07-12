---
title: Rollback Wide Segtree
documentation_of: ../src/structure/segtree/rollback_widesegtree.hpp
---

`long long`の和を扱うrollback可能な疎セグメント木。

```cpp
RollbackWideSegtree<MAX_SIZE, MAX_NODES, MAX_CHANGES>
```

```cpp
void set(long long index, long long value)
void add(long long index, long long value)
long long get(long long index)
long long prod(long long l, long long r)
long long all_prod()
Snapshot snapshot()
void undo()
void rollback(Snapshot snapshot)
```

更新と`undo`は $O(\log MAX\_SIZE)$。`rollback`は取り消す更新回数に比例する。
