---
title: Partially Persistent Segment Tree
documentation_of: ../src/structure/segtree/partially_persistent_segtree.hpp
---

過去の状態を参照でき、最新の状態のみ更新できるsegment tree。

状態は0から始まるversionで指定する。version 0は初期状態。

# テンプレート引数

```cpp
PartiallyPersistentSegtree<Monoid, MAX_SIZE, MAX_UPDATE>
```

- モノイド `Monoid`
- 配列長の上限 `MAX_SIZE`
- 更新回数の上限 `MAX_UPDATE`

オブジェクト自体のcopy/moveはできない。

# コンストラクタ

```cpp
(1) PartiallyPersistentSegtree(int n = MAX_SIZE)
(2) PartiallyPersistentSegtree(const vector<S>& v)
(3) PartiallyPersistentSegtree(const array<S, N>& v)
```

1. 長さ `n`、初期値 `Monoid.e()` で初期化する。
2. `v` で初期化する。
3. `v` で初期化する。

## 時間計算量

- $O(\mathrm{MAX\_SIZE})$

# set

```cpp
int set(int k, const S& x)
```

最新versionの `k` 番目を `x` に変更した新しいversionを作り、その番号を返す。

## 時間計算量

- $O(\log \mathrm{MAX\_SIZE})$

# get

```cpp
(1) S get(int version, int k) const
(2) S get(int k) const
```

1. `version` の `k` 番目を返す。
2. 最新versionの `k` 番目を返す。

## 時間計算量

- $O(\log \mathrm{MAX\_SIZE})$

# prod

```cpp
(1) S prod(int version, int l, int r) const
(2) S prod(int l, int r) const
```

1. `version` の区間 $[l, r)$ の積を返す。
2. 最新versionの区間 $[l, r)$ の積を返す。

## 時間計算量

- $O(\log \mathrm{MAX\_SIZE})$

# all_prod

```cpp
(1) S all_prod(int version) const
(2) S all_prod() const
```

1. `version` の全要素の積を返す。
2. 最新versionの全要素の積を返す。

## 時間計算量

- $O(1)$

# max_right

```cpp
(1) int max_right(int version, int l, auto f, auto&&... args) const
(2) int max_right(int l, auto f, auto&&... args) const
```

1. `version` に対して `f(prod(version, l, r), args...)` がtrueとなる最大の `r` を返す。
2. 最新versionに対して同様の値を返す。

`f(Monoid.e(), args...)` はtrueでなければならない。

## 時間計算量

- $O(\log \mathrm{MAX\_SIZE})$

# min_left

```cpp
(1) int min_left(int version, int r, auto f, auto&&... args) const
(2) int min_left(int r, auto f, auto&&... args) const
```

1. `version` に対して `f(prod(version, l, r), args...)` がtrueとなる最小の `l` を返す。
2. 最新versionに対して同様の値を返す。

`f(Monoid.e(), args...)` はtrueでなければならない。

## 時間計算量

- $O(\log \mathrm{MAX\_SIZE})$

# size

```cpp
int size() const
```

配列長を返す。

## 時間計算量

- $O(1)$

# versions

```cpp
int versions() const
```

現在存在するversion数を返す。

## 時間計算量

- $O(1)$

# latest_version

```cpp
int latest_version() const
```

最後に作ったversionの番号を返す。

## 時間計算量

- $O(1)$
