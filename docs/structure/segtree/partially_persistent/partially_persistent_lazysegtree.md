---
title: Partially Persistent Lazy Segment Tree (部分永続遅延セグメント木)
documentation_of: ../../../../src/structure/segtree/partially_persistent/partially_persistent_lazysegtree.hpp
---

過去の状態を参照でき、最新の状態だけを更新できる遅延セグメント木。状態は 0 から始まる version 番号で指定する。

## テンプレート引数

```cpp
PartiallyPersistentLazySegtree<Monoid_act, MAX_SIZE, MAX_UPDATE>
```

- `Monoid_act`: 作用付きモノイド
- `MAX_SIZE`: 配列長の上限
- `MAX_UPDATE`: 更新回数の上限

容量（`MAX_SIZE`, `MAX_UPDATE`）を両方省略すると可変長版になり、`PartiallyPersistentLazySegtree<Monoid_act> tree(n);` のように長さを実行時に与える。更新回数の上限はなく、履歴は使用量に応じて `std::vector` を拡張する。

オブジェクト自体のコピー・ムーブはできない。

## コンストラクタ

```cpp
(1) PartiallyPersistentLazySegtree(int n = MAX_SIZE)  // 可変長版の既定値は 0
(2) PartiallyPersistentLazySegtree(const vector<S>& v)
(3) PartiallyPersistentLazySegtree(const array<S, N>& v)
```

1. 長さ `n`、初期値 `Monoid_act.e()` で初期化する。
2. `v` で初期化する。
3. `v` で初期化する（$N \le \texttt{MAX_SIZE}$ を `static_assert` で確認する）。

## 時間計算量

- $O(\texttt{MAX_SIZE}+\texttt{MAX_UPDATE}\log \texttt{MAX_SIZE})$（可変長版は $O(n)$）

## 更新

```cpp
int set(int k, const S& x)
int apply(int k, const T& f)
int apply(int l, int r, const T& f)
```

最新 version を更新した新しい version を作り、その番号を返す。空区間への `apply` も version を 1 つ消費する。

## 時間計算量

- $O(\log \texttt{MAX_SIZE})$

## 区間取得

```cpp
S get(int version, int k) const
S get(int k) const
S prod(int version, int l, int r) const
S prod(int l, int r) const
S all_prod(int version) const
S all_prod() const
```

version を指定した場合はその状態を、指定しない場合は最新 version を参照する。

## 時間計算量

- 過去 version の `get`, `prod`: $O(\log \texttt{MAX_SIZE}\log \texttt{MAX_UPDATE})$
- 過去 version の `all_prod`: $O(\log \texttt{MAX_UPDATE})$
- 最新 version では `get`, `prod` が $O(\log \texttt{MAX_SIZE})$、`all_prod` が $O(1)$

## 二分探索

```cpp
int max_right(int version, int l, auto f, auto&&... args) const
int max_right(int l, auto f, auto&&... args) const
int min_left(int version, int r, auto f, auto&&... args) const
int min_left(int r, auto f, auto&&... args) const
```

`f(Monoid_act.e(), args...)` は `true` でなければならない。

## 時間計算量

- 過去 version: $O(\log \texttt{MAX_SIZE}\log \texttt{MAX_UPDATE})$
- 最新 version: $O(\log \texttt{MAX_SIZE})$

## version 情報

```cpp
int size() const
int versions() const
int latest_version() const
```

## 時間計算量

- $O(1)$

## 空間計算量

- $O(\texttt{MAX_SIZE}+\texttt{MAX_UPDATE}\log \texttt{MAX_SIZE})$

## 注意点

- version 0 が初期状態で、クエリは `0 <= version < versions()` の任意の version を参照できる。
- 更新は常に `latest_version()` を元に行い、新しい version 番号を返す。過去の version を更新元にする API はない。
- 点は列内、区間は半開区間 $[l, r)$ である。
- 境界探索の述語は単調で、単位元に対して `true` を返す必要がある。
- 不正な version・点・区間、述語が単位元で `false`、更新回数が `MAX_UPDATE` を超える場合は `std::runtime_error` を送出する。
- 可変長版で $n \gt 2^{28}$ の場合は `std::length_error` になる。
- 失敗した更新は履歴の使用量を更新前に戻し、version 数も変化しない。
