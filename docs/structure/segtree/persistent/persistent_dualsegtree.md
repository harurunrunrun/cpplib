---
title: Persistent Dual Segment Tree (完全永続双対セグメント木)
documentation_of: ../../../../src/structure/segtree/persistent/persistent_dualsegtree.hpp
---

任意の過去の状態を残したまま、1 点更新または区間作用を行った新しい状態を作る双対セグメント木。

version 0 が初期状態である。

## テンプレート引数

```cpp
PersistentDualSegtree<Monoid_act, MAX_SIZE, MAX_VERSION>
```

- `Monoid_act`: 作用付きモノイド
- `MAX_SIZE`: 配列長の上限
- `MAX_VERSION`: 追加できる version 数の上限（version 0 を除く）

容量（`MAX_SIZE`, `MAX_VERSION`）を両方省略すると可変長版になり、`PersistentDualSegtree<Monoid_act> tree(n);` のように長さを実行時に与える。ノードと version は使用量に応じて `std::vector` を拡張する。

オブジェクトのコピー・ムーブはできない。

## コンストラクタ

```cpp
(1) PersistentDualSegtree(int n = MAX_SIZE)  // 可変長版の既定値は 0
(2) PersistentDualSegtree(const vector<S>& v)
(3) PersistentDualSegtree(const array<S, N>& v)
```

1. 長さ `n`、初期値 `Monoid_act.e()` で初期化する。
2. `v` で初期化する。
3. `v` で初期化する（$N \le \texttt{MAX_SIZE}$ を `static_assert` で確認する）。

## 時間計算量

- $O(\texttt{MAX_SIZE}+\texttt{MAX_VERSION}\log \texttt{MAX_SIZE})$（可変長版は $O(n)$）

## 更新

```cpp
(1) int set(int version, int k, const S& x)
(2) int apply(int version, int l, int r, const T& f)
(3) int fork(int version)
```

指定 version から新しい version を作り、その番号を返す。(1) は 1 点を `x` に置き換え、(2) は半開区間 $[l, r)$ に作用 `f` を適用し、(3) は内容を変えずに複製する。空区間への `apply` は `fork` と同じである。

## 時間計算量

- (1), (2): $O(\log \texttt{MAX_SIZE})$
- (3): $O(1)$

## get

```cpp
S get(int version, int k) const
```

`version` の `k` 番目を返す。

## 時間計算量

- $O(\log \texttt{MAX_SIZE})$

## version 情報

```cpp
int size() const
int versions() const
int latest_version() const
```

## 時間計算量

- $O(1)$

## 注意点

- version 0 が初期状態で、`0 <= version < versions()` の version だけを参照できる。
- 更新と `fork` は指定した version を変更せず、新しい version 番号を返す。`latest_version()` は `versions() - 1` である。
- 点は実際の列内、区間は半開区間 $[l, r)$ で指定する。
- 不正な version・点・区間、version の容量超過では `std::runtime_error` を送出する。可変長版で $n \gt 2^{28}$ の場合は `std::length_error` になる。
- 更新が失敗した場合、version 数と使用ノード数は更新前に戻る。
