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

# 計算量

- constructor: $O(\mathtt{MAX\_NODES}+\mathtt{MAX\_CHANGES})$
- `set`, `add`, `get`, `prod`: $O(\log \mathtt{MAX\_SIZE})$
- `all_prod`, `snapshot`, `history_size`, `nodes_used`, `changes_used`, `can_undo`: $O(1)$
- `undo`: 直前の更新が作ったノード・変更数に線形。$O(\log \mathtt{MAX\_SIZE})$
- `rollback`: 取り消す更新が作ったノード・変更数の合計に線形
