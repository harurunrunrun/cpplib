---
title: Partially Persistent Lazy Segment Tree
documentation_of: ../src/structure/segtree/partially_persistent_lazysegtree.hpp
---

過去の状態を参照でき、最新の状態のみ更新できるlazy segment tree。状態は0から始まるversionで指定する。

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
int set(int k, const S& x)
int apply(int k, const T& f)
int apply(int l, int r, const T& f)
```

最新versionを更新した新しいversionを作り、その番号を返す。

## 時間計算量

- $O(\log \mathrm{MAX\_SIZE})$

# 区間取得

```cpp
S get(int version, int k) const
S get(int k) const
S prod(int version, int l, int r) const
S prod(int l, int r) const
S all_prod(int version) const
S all_prod() const
```

versionを指定した場合はその状態、指定しない場合は最新versionを参照する。

## 時間計算量

- 過去versionの `get`, `prod`: $O(\log \mathrm{MAX\_SIZE}\log \mathrm{MAX\_UPDATE})$
- 過去versionの `all_prod`: $O(\log \mathrm{MAX\_UPDATE})$
- 最新versionでは順に $O(\log \mathrm{MAX\_SIZE})$, $O(\log \mathrm{MAX\_SIZE})$, $O(1)$

# 二分探索

```cpp
int max_right(int version, int l, auto f, auto&&... args) const
int max_right(int l, auto f, auto&&... args) const
int min_left(int version, int r, auto f, auto&&... args) const
int min_left(int r, auto f, auto&&... args) const
```

`f(Monoid_act.e(), args...)` はtrueでなければならない。

## 時間計算量

- 過去version: $O(\log \mathrm{MAX\_SIZE}\log \mathrm{MAX\_UPDATE})$
- 最新version: $O(\log \mathrm{MAX\_SIZE})$

# version情報

```cpp
int size() const
int versions() const
int latest_version() const
```

## 時間計算量

- $O(1)$

# 空間計算量

- $O(\mathrm{MAX\_SIZE}+\mathrm{MAX\_UPDATE}\log \mathrm{MAX\_SIZE})$
