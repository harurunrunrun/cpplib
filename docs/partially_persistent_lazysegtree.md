---
title: Partially Persistent Lazy Segment Tree
documentation_of: ../src/structure/segtree/partially_persistent_lazysegtree.hpp
---

過去の状態を参照でき、最新の状態のみ更新できるlazy segment tree。

状態は0から始まるversionで指定する。version 0は初期状態。

# テンプレート引数

```cpp
PartiallyPersistentLazySegtree<Monoid_act, MAX_SIZE, MAX_UPDATE>
```

- 作用付きモノイド `Monoid_act`
- 配列長の上限 `MAX_SIZE`
- 更新回数の上限 `MAX_UPDATE`

オブジェクト自体のcopy/moveはできない。

# コンストラクタ

```cpp
(1) PartiallyPersistentLazySegtree(int n = MAX_SIZE)
(2) PartiallyPersistentLazySegtree(const vector<S>& v)
(3) PartiallyPersistentLazySegtree(const array<S, N>& v)
```

1. 長さ `n`、初期値 `Monoid_act.e()` で初期化する。
2. `v` で初期化する。
3. `v` で初期化する。

## 時間計算量

- $O(\mathrm{MAX\_SIZE})$

# 更新

```cpp
(1) int set(int k, const S& x)
(2) int apply(int k, const T& f)
(3) int apply(int l, int r, const T& f)
```

1. 最新versionの `k` 番目を `x` に変更した新しいversionを作る。
2. 最新versionの `k` 番目に `f` を作用させた新しいversionを作る。
3. 最新versionの区間 $[l, r)$ に `f` を作用させた新しいversionを作る。

いずれも新しいversion番号を返す。

## 時間計算量

- $O(\log \mathrm{MAX\_SIZE})$

# 区間取得

```cpp
(1) S get(int version, int k) const
(2) S get(int k) const
(3) S prod(int version, int l, int r) const
(4) S prod(int l, int r) const
(5) S all_prod(int version) const
(6) S all_prod() const
```

versionを指定した場合はその状態、指定しない場合は最新versionを参照する。

## 時間計算量

- (1), (2), (3), (4): $O(\log \mathrm{MAX\_SIZE})$
- (5), (6): $O(1)$

# 二分探索

```cpp
(1) int max_right(int version, int l, auto f, auto&&... args) const
(2) int max_right(int l, auto f, auto&&... args) const
(3) int min_left(int version, int r, auto f, auto&&... args) const
(4) int min_left(int r, auto f, auto&&... args) const
```

versionを指定した場合はその状態、指定しない場合は最新versionを参照する。

`f(Monoid_act.e(), args...)` はtrueでなければならない。

## 時間計算量

- $O(\log \mathrm{MAX\_SIZE})$

# version情報

```cpp
(1) int size() const
(2) int versions() const
(3) int latest_version() const
```

1. 配列長を返す。
2. 現在存在するversion数を返す。
3. 最後に作ったversionの番号を返す。

## 時間計算量

- $O(1)$
