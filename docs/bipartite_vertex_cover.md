---
title: Bipartite Vertex / Independent / Edge Cover
documentation_of: ../src/algorithm/matching/bipartite_vertex_cover.hpp
---

`BipartiteMatching::solve()` の最大マッチングから、Kőnigの定理に基づく頂点集合と最小辺被覆を構成する。

## `BipartiteVertexSet`

```cpp
struct BipartiteVertexSet {
    int size;
    vector<char> left;
    vector<char> right;
};
```

`left[v]`、`right[v]` は頂点が集合に含まれるかを表す。`size` は両側を合わせた要素数。

## `bipartite_konig_decomposition`

```cpp
BipartiteKonigDecomposition bipartite_konig_decomposition(
    const BipartiteMatching& graph,
    const BipartiteMatchingResult& maximum_matching
)
```

戻り値は次の2集合を持つ。

- `minimum_vertex_cover`: すべての辺の少なくとも一端を含む最小頂点被覆
- `maximum_independent_set`: 内部に辺を持たない最大独立集合

未matchの左頂点から、非match辺を左から右へ、match辺を右から左へたどる。到達した左集合を $Z_L$、右集合を $Z_R$ とすると、最小頂点被覆は $(L\setminus Z_L)\cup Z_R$、最大独立集合はその補集合。

`maximum_matching` は同じ `graph` に対する最大マッチングでなければならない。対応列の不整合、存在しないmatch辺、増加路が見つかる非最大matchingには `invalid_argument`。

## `minimum_bipartite_edge_cover`

```cpp
optional<vector<pair<int, int>>> minimum_bipartite_edge_cover(
    const BipartiteMatching& graph,
    const BipartiteMatchingResult& maximum_matching
)
```

すべての頂点を被覆する辺集合を返す。各pairは `(left, right)`。孤立頂点が1つでもあれば辺被覆は存在しないため `nullopt`。頂点が0個なら空の辺列。

最大マッチングの辺を入れ、まだmatchされていない各頂点へ接続辺を1本ずつ加える。辺被覆が存在する場合、その大きさは $|L|+|R|-|M|$。

## 計算量

$V=|L|+|R|$、$E$ を辺数とする。

| API | 時間計算量 | 追加領域 |
| --- | --- | --- |
| `bipartite_konig_decomposition` | $O(V+E)$ | $O(V)$ |
| `minimum_bipartite_edge_cover` | $O(V+E)$ | $O(V+E)$（戻り値を含む） |
