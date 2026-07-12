---
title: Online Dynamic Connectivity
documentation_of: ../src/structure/graph/online_dynamic_connectivity.hpp
---

辺の追加削除をオンラインに処理する動的連結性。

連結成分番号を持つため、連結判定は定数時間で行う。辺削除で連結成分が分割される可能性があるときだけ探索する。

# テンプレート引数

```cpp
OnlineDynamicConnectivity<MAX_SIZE>
```

- 頂点数の上限 `MAX_SIZE`

# コンストラクタ

```cpp
OnlineDynamicConnectivity(int n = MAX_SIZE)
```

`n` 頂点の空グラフを作る。

# 辺の追加削除

```cpp
bool add_edge(int u, int v)
bool erase_edge(int u, int v)
bool link(int u, int v)
bool cut(int u, int v)
```

`add_edge` は辺を追加する。同じ辺がすでに存在する場合、多重辺として数える。

`erase_edge` は辺を 1 本削除する。辺が存在しなければ `false` を返す。

`link` は `add_edge`、`cut` は `erase_edge` と同じ。

## 時間計算量

- `add_edge`: 併合される小さい成分の頂点数に比例
- `erase_edge`: 削除辺を含む連結成分の辺数に比例

# 連結判定

```cpp
bool same(int u, int v)
int component_size(int v)
int groups()
int edge_multiplicity(int u, int v)
```

## 時間計算量

- `same`: $O(1)$
- `component_size`: $O(1)$
- `groups`: $O(1)$
- `edge_multiplicity`: $O(\log M)$
