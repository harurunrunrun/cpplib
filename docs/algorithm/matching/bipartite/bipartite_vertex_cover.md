---
title: Bipartite Vertex Cover / Independent Set / Edge Cover (二部グラフの頂点被覆・独立集合・辺被覆)
documentation_of: ../../../../src/algorithm/matching/bipartite/bipartite_vertex_cover.hpp
---

`BipartiteMatching::solve()` で得た最大マッチングから、Kőnig の定理に基づいて最小頂点被覆・最大独立集合と最小辺被覆を構成する。

## `BipartiteVertexSet`

```cpp
struct BipartiteVertexSet {
    int size;
    vector<char> left;
    vector<char> right;
};
```

`left[v]`、`right[v]` は頂点が集合に含まれるかを表す。`size` は左右を合わせた要素数である。

## `bipartite_konig_decomposition`

```cpp
BipartiteKonigDecomposition bipartite_konig_decomposition(
    const BipartiteMatching& graph,
    const BipartiteMatchingResult& maximum_matching
)
```

戻り値は次の 2 つの集合を持つ。

- `minimum_vertex_cover`: すべての辺の少なくとも一方の端点を含む最小頂点被覆
- `maximum_independent_set`: 内部に辺を持たない最大独立集合

未マッチの左頂点から、非マッチ辺を左から右へ、マッチ辺を右から左へたどる。到達した左頂点の集合を $Z_L$、右頂点の集合を $Z_R$ とすると、最小頂点被覆は $(L \setminus Z_L) \cup Z_R$ であり、最大独立集合はその補集合である。

`maximum_matching` は同じ `graph` に対する最大マッチングでなければならない。対応列の大きさや相互対応の不整合、存在しない辺をマッチ辺とする場合、`size` の不一致、増加路が見つかる（最大でない）マッチングに対しては `invalid_argument` を送出する。

## `minimum_bipartite_edge_cover`

```cpp
optional<vector<pair<int, int>>> minimum_bipartite_edge_cover(
    const BipartiteMatching& graph,
    const BipartiteMatchingResult& maximum_matching
)
```

すべての頂点を被覆する最小の辺集合を返す。各要素は `(left, right)` の組である。孤立頂点が 1 つでもあれば辺被覆は存在しないため `nullopt` を返す。頂点が 0 個なら空の列を返す。`maximum_matching` の検査は `bipartite_konig_decomposition` と同じで、不正なら `invalid_argument` を送出する。

最大マッチングの辺をすべて入れ、マッチしていない各頂点について接続する辺を 1 本ずつ加える。辺被覆が存在する場合、その大きさは $\lvert L \rvert + \lvert R \rvert - \lvert M \rvert$ である。

## 時間計算量・空間計算量

$V = \lvert L \rvert + \lvert R \rvert$、$E$ を辺数とする。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `bipartite_konig_decomposition` | $O(V + E)$ | $O(V)$ |
| `minimum_bipartite_edge_cover` | $O(V + E)$ | $O(V)$（戻り値を含む） |
