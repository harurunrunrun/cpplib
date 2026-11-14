---
title: Rollback Dynamic Lazy Segment Tree (ロールバック可能動的遅延セグメント木)
documentation_of: ../../../../src/structure/segtree/rollback/rollback_dynamic_lazysegtree.hpp
---

巨大な添字範囲を扱う、ロールバック可能な遅延セグメント木。

```cpp
RollbackDynamicLazySegtree<Monoid, MAX_SIZE, MAX_NODES, MAX_CHANGES>
```

`Monoid` は区間長付きの作用モノイド（`MonoidActLen` 等）のオブジェクトである。

- `MAX_SIZE`: 添字範囲の長さの上限
- `MAX_NODES`: 疎ノード数の上限
- `MAX_CHANGES`: ノードの状態と疎な子リンクの変更履歴数の上限

容量（3 つのテンプレート引数）をすべて省略すると可変長版になり、`RollbackDynamicLazySegtree<Monoid> tree(n);` のように長さを実行時に与える。ノードと履歴は使用量に応じて `std::vector` を拡張する。

## API

```cpp
explicit RollbackDynamicLazySegtree(long long n = MAX_SIZE)  // 可変長版の既定値は 0
void set(long long index, S value)
void apply(long long index, T action)
void apply(long long l, long long r, T action)
S get(long long index) const
S prod(long long l, long long r) const
S all_prod() const
long long max_right(long long l, F f, Args... args) const
long long min_left(long long r, F f, Args... args) const
Snapshot snapshot() const
void undo()
void rollback(Snapshot snapshot)
long long size() const
int history_size() const
int nodes_used() const
int changes_used() const
bool can_undo() const
```

`Snapshot` は `int` の別名である。空区間への `apply` も更新 1 回として履歴に記録する。

## 時間計算量

- コンストラクタ: $O(\texttt{MAX_NODES}+\texttt{MAX_CHANGES})$
- `set`, 1 点 `apply`, 区間 `apply`, `get`, `prod`, `max_right`, `min_left`: $O(\log \texttt{MAX_SIZE})$
- `all_prod`, `snapshot`, `size`, `history_size`, `nodes_used`, `changes_used`, `can_undo`: $O(1)$
- `undo`: 直前の更新が作ったノード数・変更数に線形で、$O(\log \texttt{MAX_SIZE})$
- `rollback`: 取り消す更新が作ったノード数・変更数の合計に線形

## 注意点

- 点は範囲内、区間は半開区間 $[l, r)$ で指定する。
- `snapshot()` は現在の状態を表す値（履歴に残っている更新回数）を返し、`rollback(s)` は `s` より後の更新をすべて破棄する。`undo()` は直前の更新を 1 つ取り消し、`can_undo()` はその可否を返す。
- 境界探索の述語は単調で、単位元に対して `true` を返す必要がある。
- 不正な点・区間・snapshot、述語が単位元で `false`、履歴が空の状態での `undo`、ノード・変更履歴の容量超過では `std::runtime_error` を送出する。
- 失敗した更新は本体と履歴を更新前に戻す。
- コピーコンストラクタとコピー代入は削除されている。
