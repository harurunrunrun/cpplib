---
title: Persistent Dual Segment Tree
documentation_of: ../src/structure/segtree/persistent_dualsegtree.hpp
---

任意の過去の状態を残したまま、1点更新または区間作用した新しい状態を作るdual segment tree。

version 0は初期状態。

# テンプレート引数

```cpp
PersistentDualSegtree<Monoid_act, MAX_SIZE, MAX_VERSION>
```

- 作用付きモノイド `Monoid_act`
- 配列長の上限 `MAX_SIZE`
- 追加できるversion数の上限 `MAX_VERSION`

# コンストラクタ

```cpp
(1) PersistentDualSegtree(int n = MAX_SIZE)
(2) PersistentDualSegtree(const vector<S>& v)
(3) PersistentDualSegtree(const array<S, N>& v)
```

## 時間計算量

- $O(\mathrm{MAX\_SIZE})$

# 更新

```cpp
(1) int set(int version, int k, const S& x)
(2) int apply(int version, int l, int r, const T& f)
(3) int fork(int version)
```

新しいversionを作り、その番号を返す。

## 時間計算量

- (1), (2): $O(\log \mathrm{MAX\_SIZE})$
- (3): $O(1)$

# get

```cpp
S get(int version, int k) const
```

`version` の `k` 番目を返す。

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
