---
title: Disjoint Set Union
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

- $O(N)$

# merge / same

```cpp
bool merge(int u, int v)
bool same(int u, int v)
```

`merge` は異なる集合が併合されたとき `true` を返す。

## 時間計算量

- amortized $O(\alpha(N))$

# leader / component_size

```cpp
int leader(int v)
int component_size(int v)
int groups()
```

## 時間計算量

- amortized $O(\alpha(N))$
