---
title: Disjoint Set Union (素集合データ構造)
documentation_of: ../src/structure/dsu/dsu.hpp
---

素集合データ構造。

# テンプレート引数

```cpp
DSU<MAX_SIZE>
```

- 頂点数の上限 `MAX_SIZE`

# コンストラクタ

```cpp
DSU(int n = MAX_SIZE)
```

`n` 頂点の集合を作る。

## 時間計算量

- $O(\mathtt{MAX\_SIZE})$

# merge / same

```cpp
bool merge(int u, int v)
bool same(int u, int v)
```

`merge` は異なる集合が併合されたとき `true` を返す。

## 時間計算量

- amortized $O(\alpha(N))$

`size`, `groups` は $O(1)$。

# leader / component_size

```cpp
int leader(int v)
int component_size(int v)
int groups()
```

## 時間計算量

- amortized $O(\alpha(N))$

# 前提・例外・容量

- `0 <= n <= MAX_SIZE`。constructorで範囲外なら例外。
- 頂点引数はすべて`[0,n)`。範囲外なら例外。
- `leader`は代表、`component_size`は成分頂点数、`groups`は成分数を返す。
- 固定配列は$O(MAX\_SIZE)$で、`merge`は追加領域を消費しない。
