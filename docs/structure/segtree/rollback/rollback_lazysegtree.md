---
title: Rollback Lazy Segment Tree (ロールバック可能遅延セグメント木)
documentation_of: ../../../../src/structure/segtree/rollback/rollback_lazysegtree.hpp
---

1 点更新と区間作用を取り消せる遅延セグメント木。

## テンプレート引数

```cpp
RollbackLazySegtree<Monoid_act, MAX_SIZE, MAX_UPDATE>
```

- `Monoid_act`: 作用付きモノイド
- `MAX_SIZE`: 配列長の上限
- `MAX_UPDATE`: 同時に保持する更新履歴数の上限

容量（`MAX_SIZE`, `MAX_UPDATE`）を両方省略すると可変長版になり、`RollbackLazySegtree<Monoid_act> tree(n);` のように長さを実行時に与える。履歴は使用量に応じて `std::vector` を拡張する。

オブジェクト自体のコピー・ムーブはできない。

## コンストラクタ

```cpp
(1) RollbackLazySegtree(int n = MAX_SIZE)  // 可変長版の既定値は 0
(2) RollbackLazySegtree(const vector<S>& v)
(3) RollbackLazySegtree(const array<S, N>& v)
```

1. 長さ `n`、初期値 `Monoid_act.e()` で初期化する。
2. `v` で初期化する。
3. `v` で初期化する。

## 時間計算量

- $O(\texttt{MAX_SIZE}+\texttt{MAX_UPDATE}\log \texttt{MAX_SIZE})$（可変長版は $O(n)$）

## 更新

```cpp
(1) void set(int k, const S& x)
(2) void apply(int k, const T& f)
(3) void apply(int l, int r, const T& f)
```

1. `k` 番目を `x` に変更する。
2. `k` 番目に `f` を作用させる。
3. 区間 $[l, r)$ に `f` を作用させる。

変更前の状態を履歴に追加する。

## 時間計算量

- $O(\log \texttt{MAX_SIZE})$

## 区間取得

```cpp
(1) S get(int k) const
(2) S prod(int l, int r) const
(3) S all_prod() const
```

1. `k` 番目を返す。
2. 区間 $[l, r)$ の総積を返す。
3. 全要素の総積を返す。

## 時間計算量

- (1), (2): $O(\log \texttt{MAX_SIZE})$
- (3): $O(1)$

## 二分探索

```cpp
(1) int max_right(int l, auto f, auto&&... args) const
(2) int min_left(int r, auto f, auto&&... args) const
```

1. `f(prod(l, r), args...)` が `true` となる最大の `r` を返す。
2. `f(prod(l, r), args...)` が `true` となる最小の `l` を返す。

`f(Monoid_act.e(), args...)` は `true` でなければならない。

## 時間計算量

- $O(\log \texttt{MAX_SIZE})$

## スナップショット

```cpp
using Snapshot = int;

Snapshot snapshot() const
```

現在の更新履歴数を返す。

`rollback` で取り消した履歴上のスナップショットは無効になる。

## 時間計算量

- $O(1)$

## ロールバック

```cpp
void rollback(Snapshot s)
```

`snapshot()` が `s` を返した時点まで更新を戻す。

## 時間計算量

- 戻す更新数を $k$ として $O(k \log \texttt{MAX_SIZE})$

## undo

```cpp
void undo()
```

直前の更新を取り消す。

## 時間計算量

- $O(\log \texttt{MAX_SIZE})$

## 履歴情報

```cpp
(1) bool can_undo() const
(2) int history_size() const
(3) int size() const
```

1. 取り消せる更新があるかを返す。
2. 現在保持している更新履歴数を返す。
3. 配列長を返す。

## 時間計算量

- $O(1)$

## 注意点

- 点は列内、区間は半開区間 $[l, r)$ で指定する。
- 境界探索の述語は単調で、単位元に対して `true` を返す必要がある。
- 不正な点・区間・スナップショット、述語が単位元で `false`、履歴が空の状態での `undo`、保持する更新数が `MAX_UPDATE` を超える場合は `std::runtime_error` を送出する。`std::array` の長さが `MAX_SIZE` を超える場合も `std::runtime_error` になる。可変長版で $n \gt 2^{28}$ の場合は `std::length_error` になる。
- 失敗した更新は本体と履歴を更新前に戻す。
