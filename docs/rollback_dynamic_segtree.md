---
title: Rollback Dynamic Segment Tree (ロールバック可能動的セグメント木)
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

`MAX_NODES`は疎ノード数、`MAX_CHANGES`はノード変更履歴数の上限。

# 時間計算量

- constructor: $O(\mathtt{MAX\_NODES}+\mathtt{MAX\_CHANGES})$
- `set`, `get`, `prod`, `max_right`, `min_left`: $O(\log \mathtt{MAX\_SIZE})$
- `all_prod`, `snapshot`, `history_size`, `nodes_used`, `changes_used`, `can_undo`: $O(1)$
- `undo`: 直前の更新が作ったノード・変更数に線形。$O(\log \mathtt{MAX\_SIZE})$
- `rollback`: 取り消す更新が作ったノード・変更数の合計に線形

## 注意点

点は列内、区間は半開区間 `[l,r)`。`snapshot()` は現在状態を表すtokenを返し、
`rollback(snapshot)` はそれ以後の更新を破棄する。`undo()` を持つ型では直前の更新を1つ戻し、
`can_undo()` はその可否を返す。境界探索のpredicateは単調かつ単位元に対してtrueが必要。

不正な点・区間・snapshot・predicate、空状態での `undo`、node/change/history容量超過では
`runtime_error`。失敗した更新は本体と履歴を更新前へ戻す。各APIの計算量は上記表の通り。
