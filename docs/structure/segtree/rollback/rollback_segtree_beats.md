---
title: Rollback Segment Tree Beats (ロールバック可能セグメント木Beats)
documentation_of: ../../../../src/structure/segtree/rollback/rollback_segtree_beats.hpp
---

変更前のノードを履歴に記録する、ロールバック可能な Segment Tree Beats。

```cpp
RollbackSegmentTreeBeats<T, MAX_SIZE, MAX_CHANGES, MAX_OPERATIONS>
```

- `MAX_SIZE`: 列の長さの上限
- `MAX_CHANGES`: ノード変更数の合計の上限
- `MAX_OPERATIONS`: ロールバックされていない更新回数の上限（省略時は `MAX_CHANGES`）

容量（3 つのテンプレート引数）をすべて省略すると可変長版になり、`RollbackSegmentTreeBeats<T> tree(n);` のように長さを実行時に与える。履歴は使用量に応じて `std::vector` を拡張する。

## コンストラクタ

```cpp
explicit RollbackSegmentTreeBeats(int n = MAX_SIZE)  // 可変長版の既定値は 0
explicit RollbackSegmentTreeBeats(const std::vector<T>& values)
explicit RollbackSegmentTreeBeats(const std::array<T, N>& values)
```

長さ `n` の全 `T{}` の列、または `values` で初期化する。

## 更新

```cpp
void range_chmin(int l, int r, const T& x)
void range_chmax(int l, int r, const T& x)
void range_add(int l, int r, const T& x)
void set(int k, const T& x)
```

容量不足で失敗した場合は、変更履歴と配列を更新前に戻す。

## ロールバック

```cpp
using Snapshot = int;
Snapshot snapshot() const
void rollback(Snapshot snapshot)
```

`rollback(s)` は `snapshot()` が `s` を返した時点まで状態を戻す。

## クエリ

```cpp
T get(int k) const
T range_sum(int l, int r) const
T range_min(int l, int r) const
T range_max(int l, int r) const
T all_sum() const
T all_min() const
T all_max() const
```

クエリは履歴を追加せず、祖先ノードの加算・上限制約・下限制約を子へ順に合成して値を求める。

```cpp
int size() const
int operations() const
int changes_used() const
```

順に列の長さ、ロールバックされていない更新回数、使用中のノード変更記録数を返す。

## 時間計算量

$N$ を列の長さとする。

- コンストラクタ: $O(\texttt{MAX_SIZE}+\texttt{MAX_CHANGES}+\texttt{MAX_OPERATIONS})$
- `range_chmin`, `range_chmax`: 償却 $O(\log^2 N)$
- `range_add`, `set`, `get`, `range_sum`, `range_min`, `range_max`: $O(\log N)$
- `all_sum`, `all_min`, `all_max`, `snapshot`, 状態・容量情報: $O(1)$
- `rollback`: 取り消すノード変更数を $K$ として $O(K)$

空間計算量は $O(\texttt{MAX_SIZE}+\texttt{MAX_CHANGES}+\texttt{MAX_OPERATIONS})$ である。

## 注意点

- 点は列内、区間は半開区間 $[l, r)$ で指定する。
- `rollback(s)` は `s` より後の更新をすべて破棄する。取り消した先の状態には戻れない。
- 空区間に対する `range_min`, `range_max`、空列に対する `all_min`, `all_max` は `std::runtime_error` を送出する。
- 不正な点・区間・スナップショット、ノード変更数・更新回数の容量超過では `std::runtime_error` を送出する。
