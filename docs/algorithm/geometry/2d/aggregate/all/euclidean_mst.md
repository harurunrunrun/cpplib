---
title: Euclidean Minimum Spanning Tree Aggregator (ユークリッド最小全域木集約ヘッダ) [euclidean_mst]
documentation_of: ../../../../../../src/algorithm/geometry/2d/aggregate/all/euclidean_mst.hpp
---

整数座標点のユークリッド最小全域木に関する型と関数をまとめて読み込む
後方互換集約ヘッダ。

## 構成

| leaf header | 提供するAPI |
| --- | --- |
| `euclidean_mst_edge.hpp` | `EuclideanMstEdge` |
| `euclidean_mst_result.hpp` | `EuclideanMstResult` |
| `euclidean_mst_points.hpp` | `euclidean_mst(points)` |

必要なleafだけを直接includeできる。従来の `euclidean_mst.hpp` は引き続き
全APIを提供する。

## 集約されるAPI

二次元整数座標点の完全グラフについて、辺重みをユークリッド距離とした最小全域木を求める。
Delaunay三角形分割の辺だけを候補にしてKruskal法を行う。座標が一致する点も別頂点として扱う。

## `euclidean_mst`

```cpp
EuclideanMstResult result = euclidean_mst(points);
```

`points`は`std::vector<std::pair<Coordinate, Coordinate>>`で、`Coordinate`は整数型とする。
空配列では辺を返さず、$N\geq1$では`result.edges`の要素数は$N-1$である。

向き判定、外接円判定、平方距離を整数演算で行う。外接円判定の中間値が
128-bit符号付き整数に、座標差の二乗和が128-bit符号なし整数に収まる必要がある。

## `EuclideanMstResult` / `EuclideanMstEdge`

```cpp
struct EuclideanMstEdge {
    std::size_t from;
    std::size_t to;
    __uint128_t squared_distance;
};

struct EuclideanMstResult {
    std::vector<EuclideanMstEdge> edges;
};
```

`from`, `to`は入力点の添字である。`squared_distance`はその辺のユークリッド距離の二乗である。

## API別の時間計算量・空間計算量

$N$を点数とする。

| API・操作 | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `euclidean_mst` | $O(N\log N)$ | $O(N)$ |
| `result.edges.size()` / 各memberの参照 | $O(1)$ | $O(1)$ |
| 全辺の列挙 | $O(N)$ | $O(1)$（返り値を除く） |

## 注意点

`Coordinate` は整数型でなければならない。向き・外接円判定は128-bit符号付き整数、
平方距離は128-bit符号なし整数の範囲に収まる必要がある。
