---
title: Rollback Dynamic Lazy Segment Tree (ロールバック可能動的遅延セグメント木)
documentation_of: ../../../../src/structure/segtree/rollback/rollback_dynamic_lazysegtree.hpp
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

## 時間計算量

- constructor: $O(\mathtt{MAX\_NODES}+\mathtt{MAX\_CHANGES})$
- `set`, 1点`apply`, 区間`apply`, `get`, `prod`, `max_right`, `min_left`: $O(\log \mathtt{MAX\_SIZE})$
- `all_prod`, `snapshot`, `size`, `history_size`, `nodes_used`, `changes_used`, `can_undo`: $O(1)$
- `undo`: 直前の更新が作ったノード・変更数に線形。$O(\log \mathtt{MAX\_SIZE})$
- `rollback`: 取り消す更新が作ったノード・変更数の合計に線形

## 注意点

点は列内、区間は半開区間 `[l,r)`。`snapshot()` は現在状態を表すtokenを返し、
`rollback(snapshot)` はそれ以後の更新を破棄する。`undo()` を持つ型では直前の更新を1つ戻し、
`can_undo()` はその可否を返す。境界探索のpredicateは単調かつ単位元に対してtrueが必要。

不正な点・区間・snapshot・predicate、空状態での `undo`、node/change/history容量超過では
`runtime_error`。失敗した更新は本体と履歴を更新前へ戻す。各APIの計算量は上記表の通り。

## 容量を省略する場合

容量引数の既定値は `-1`。`RollbackDynamicLazySegtree<Monoid> tree(n);` のように容量を省略すると、同じヘッダ内の可変長版を使用する。既存の非負容量を指定する固定配列版も利用できる。 モノイド・係数型・剰余の数・分岐数など、容量以外のテンプレート引数は引き続き指定する。

木の幅・高さは実行時の入力から求め、通常の密な木は要素数に比例する記憶領域を確保する。疎な木のノード、永続版の版情報、更新・rollbackの履歴は使用量に応じてvectorを拡張する。固定配列版と同じ更新・照会アルゴリズムを用い、拡張に伴う再配置は償却計算量に含める。整数添字の表現範囲と実メモリ容量は上限となる。
