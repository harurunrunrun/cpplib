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

`MAX_CHANGES`はノード状態と疎な子リンクの変更履歴数の上限。

# 計算量

- constructor: $O(\mathtt{MAX\_NODES}+\mathtt{MAX\_CHANGES})$
- `set`, 1点`apply`, 区間`apply`, `get`, `prod`, `max_right`, `min_left`: $O(\log \mathtt{MAX\_SIZE})$
- `all_prod`, `snapshot`, `size`, `history_size`, `nodes_used`, `changes_used`, `can_undo`: $O(1)$
- `undo`: 直前の更新が作ったノード・変更数に線形。$O(\log \mathtt{MAX\_SIZE})$
- `rollback`: 取り消す更新が作ったノード・変更数の合計に線形
