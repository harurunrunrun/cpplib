---
title: Dynamic Tree Edge Path Sum (動的木辺パス和) [ABC294_G]
documentation_of: ../src/structure/tree/dynamic_tree_edge_path_sum.hpp
---

静的な無向木の辺重みを変更しながら、2頂点間の単純パス上の辺重み総和を管理する。
辺番号はコンストラクタへ渡した順番で固定される。

## 型

```cpp
DynamicTreeEdgePathSum<T>
```

`T` は `T{}`、加算、減算を提供する必要がある。

## コンストラクタ

```cpp
DynamicTreeEdgePathSum(
    int vertex_count,
    const vector<tuple<int, int, T>>& edges,
    int root = 0
)
```

`vertex_count` 頂点の木を構築する。`edges[i]` は辺 `i` の両端点と初期重みである。
`root` は内部の木分解にのみ使われ、各APIの結果には影響しない。

## `size`

```cpp
int size() const
```

頂点数を返す。

## `edge_count`

```cpp
int edge_count() const
```

辺数を返す。

## `edge_value`

```cpp
const T& edge_value(int edge) const
```

辺 `edge` の現在の重みへの参照を返す。

## `set_edge`

```cpp
void set_edge(int edge, const T& value)
```

辺 `edge` の重みを `value` に置き換える。

## `path_sum`

```cpp
T path_sum(int u, int v) const
```

単純パス `u-v` 上にある辺重みの総和を返す。`u == v` なら `T{}` を返す。

## 時間計算量

- コンストラクタ: $O(N\log(N+1))$
- `size`, `edge_count`, `edge_value`: $O(1)$
- `set_edge`: $O(\log(N+1))$
- `path_sum`: $O(\log^2(N+1))$

## 空間計算量

木、HL分解、Fenwick索引を含めて $O(N)$。

## 注意点

- 非空木では `edges.size() == vertex_count - 1` であり、辺集合は連結かつ非巡回でなければならない。
- 頂点番号は `0` 以上 `size()` 未満、辺番号は `0` 以上 `edge_count()` 未満でなければならない。
- 木・添字・根の前提違反では `runtime_error`。
- 加減算のoverflowは `T` の演算規則に従う。
