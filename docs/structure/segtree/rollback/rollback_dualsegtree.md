---
title: Rollback Dual Segment Tree (ロールバック可能双対セグメント木)
documentation_of: ../../../../src/structure/segtree/rollback/rollback_dualsegtree.hpp
---

1 点更新と区間作用を取り消せる双対セグメント木。

## テンプレート引数

```cpp
RollbackDualSegtree<Monoid_act, MAX_SIZE, MAX_UPDATE>
```

- `Monoid_act`: 作用付きモノイド
- `MAX_SIZE`: 配列長の上限
- `MAX_UPDATE`: 同時に履歴に保持できる更新回数の上限（`undo`・`rollback` で取り消した分は再利用できる）

容量（`MAX_SIZE`, `MAX_UPDATE`）を両方省略すると可変長版になり、`RollbackDualSegtree<Monoid_act> tree(n);` のように長さを実行時に与える。履歴は使用量に応じて `std::vector` を拡張する。

## コンストラクタ

```cpp
(1) RollbackDualSegtree(int n = MAX_SIZE)  // 可変長版の既定値は 0
(2) RollbackDualSegtree(const vector<S>& v)
(3) RollbackDualSegtree(const array<S, N>& v)
```

1. 長さ `n`、初期値 `Monoid_act.e()` で初期化する。
2. `v` で初期化する。
3. `v` で初期化する。

## 時間計算量

- $O(\texttt{MAX_SIZE}+\texttt{MAX_UPDATE}\log \texttt{MAX_SIZE})$（可変長版は $O(n)$）

## 更新と取得

```cpp
(1) void set(int k, const S& x)
(2) void apply(int l, int r, const T& f)
(3) S get(int k) const
```

1. `k` 番目の値を `x` に置き換える。
2. 半開区間 $[l, r)$ に作用 `f` を適用する。空区間でも更新 1 回として履歴に記録する。
3. `k` 番目の値を返す。

## 時間計算量

- $O(\log \texttt{MAX_SIZE})$

## ロールバック

```cpp
using Snapshot = int;
Snapshot snapshot() const
void rollback(Snapshot s)
void undo()
```

`snapshot()` は現在の状態を表す値（履歴に残っている更新回数）を返す。`rollback(s)` は `s` より後の更新をすべて取り消し、`undo()` は直前の更新を 1 つ取り消す。

## 時間計算量

- `snapshot`: $O(1)$
- `undo`: $O(\log \texttt{MAX_SIZE})$
- 戻す更新数を $k$ としたときの `rollback`: $O(k \log \texttt{MAX_SIZE})$

## 情報

```cpp
int size() const
int history_size() const
bool can_undo() const
```

順に列の長さ、履歴に残っている更新回数、`undo` できるかどうかを返す。

## 時間計算量

- $O(1)$

## 注意点

- 点は列内、区間は半開区間 $[l, r)$ で指定する。
- 不正な点・区間・snapshot、履歴が空の状態での `undo`、更新回数が `MAX_UPDATE` を超える場合は `std::runtime_error` を送出する。可変長版で $n \gt 2^{28}$ の場合は `std::length_error` になる。
- 失敗した更新は本体と履歴を更新前に戻す。
