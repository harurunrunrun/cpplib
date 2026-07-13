---
title: Weighted DSU
documentation_of: ../src/structure/dsu/weighted_dsu.hpp
---

差分制約付き素集合データ構造。

# テンプレート引数

```cpp
WeightedDSU<T, MAX_SIZE>
```

- ポテンシャル差の型 `T`
- 頂点数の上限 `MAX_SIZE`

`T` は `+`, `-`, `==`, `T()` を持つ。

# コンストラクタ

```cpp
WeightedDSU(int n = MAX_SIZE)
```

`n` 頂点の集合を作る。

## 時間計算量

- $O(\mathtt{MAX\_SIZE})$

# merge / diff

```cpp
bool merge(int u, int v, T w)
T diff(int u, int v)
```

`merge(u, v, w)` は `potential(v) - potential(u) = w` を追加する。

矛盾しない場合 `true`、矛盾する場合 `false` を返す。

`diff(u, v)` は `potential(v) - potential(u)` を返す。

## 時間計算量

- amortized $O(\alpha(N))$

`weight`, `component_size` も amortized $O(\alpha(N))$、`size`, `groups` は $O(1)$。

# same / leader

```cpp
bool same(int u, int v)
int leader(int v)
T weight(int v)
int component_size(int v)
int groups()
```

`weight(v)` は `potential(v) - potential(leader(v))` を返す。

## 時間計算量

- amortized $O(\alpha(N))$
