---
title: Partially Persistent Dual Segment Tree
documentation_of: ../src/structure/segtree/partially_persistent_dualsegtree.hpp
---

過去の状態を参照でき、最新の状態のみ更新できるdual segment tree。

# テンプレート引数

```cpp
PartiallyPersistentDualSegtree<Monoid_act, MAX_SIZE, MAX_UPDATE>
```

# コンストラクタ

```cpp
(1) PartiallyPersistentDualSegtree(int n = MAX_SIZE)
(2) PartiallyPersistentDualSegtree(const vector<S>& v)
(3) PartiallyPersistentDualSegtree(const array<S, N>& v)
```

## 時間計算量

- $O(\mathrm{MAX\_SIZE})$

# 更新

```cpp
(1) int set(int k, const S& x)
(2) int apply(int l, int r, const T& f)
```

最新versionを更新した新しいversionを作り、その番号を返す。

## 時間計算量

- $O(\log \mathrm{MAX\_SIZE})$

# get

```cpp
(1) S get(int version, int k) const
(2) S get(int k) const
```

1. 指定versionの値を返す。
2. 最新versionの値を返す。

## 時間計算量

- $O(\log \mathrm{MAX\_SIZE})$

# version情報

```cpp
int size() const
int versions() const
int latest_version() const
```

## 時間計算量

- $O(1)$
