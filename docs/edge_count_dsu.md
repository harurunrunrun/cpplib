---
title: Edge Count Disjoint Set Union
documentation_of: ../src/structure/dsu/edge_count_dsu.hpp
---

辺追加を扱う素集合データ構造。

各連結成分について、頂点数と辺数を持つ。

# テンプレート引数

```cpp
EdgeCountDSU<MAX_SIZE>
```

- 頂点数の上限 `MAX_SIZE`

# コンストラクタ

```cpp
EdgeCountDSU(int n = MAX_SIZE)
```

`n` 頂点の空グラフを作る。

## 時間計算量

- $O(N)$

# add_edge

```cpp
bool add_edge(int u, int v)
```

辺 `(u, v)` を追加する。
異なる連結成分が併合されたとき `true` を返す。

## 時間計算量

- amortized $O(\alpha(N))$

# same / leader

```cpp
bool same(int u, int v)
int leader(int v)
```

## 時間計算量

- amortized $O(\alpha(N))$

# 成分情報

```cpp
int component_size(int v)
int edges(int v)
int excess_edges(int v)
bool is_tree(int v)
bool is_unicyclic(int v)
int groups()
```

`excess_edges(v)` は `edges(v) - component_size(v) + 1` を返す。

## 時間計算量

- amortized $O(\alpha(N))$
