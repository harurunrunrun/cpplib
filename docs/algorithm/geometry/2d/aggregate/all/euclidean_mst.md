---
title: Euclidean Minimum Spanning Tree Aggregator (ユークリッド最小全域木集約ヘッダ) [euclidean_mst]
documentation_of: ../../../../../../src/algorithm/geometry/2d/aggregate/all/euclidean_mst.hpp
---

整数座標の点のユークリッド最小全域木に関する型と関数をまとめて読み込む後方互換の集約ヘッダである。

## 構成

| leaf header | 提供する API |
| --- | --- |
| `euclidean_mst_edge.hpp` | `EuclideanMstEdge` |
| `euclidean_mst_result.hpp` | `EuclideanMstResult` |
| `euclidean_mst_points.hpp` | `euclidean_mst(points)` |

必要な leaf header だけを直接 include することもできる。従来どおり `euclidean_mst.hpp` を include すれば全 API を使える。

## 集約される API

2 次元の整数座標の点からなる完全グラフについて、辺の重みをユークリッド距離とした最小全域木を求める。Delaunay 三角形分割の辺だけを候補として Kruskal 法を行う。座標が一致する点も別の頂点として扱う。

## `euclidean_mst`

```cpp
EuclideanMstResult result = euclidean_mst(points);
```

`points` は `std::vector<std::pair<Coordinate, Coordinate>>` で、`Coordinate` は整数型とする。空の配列では辺を返さず、$N \ge 1$ では `result.edges` の要素数は $N - 1$ である。

向き判定・外接円判定・平方距離はすべて整数演算で行う。

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

`from`, `to` は入力点の添字であり、`squared_distance` はその辺のユークリッド距離の二乗である。

## 計算量

$N$ を点の個数とする。

| API・操作 | 時間計算量 | 追加の空間計算量 |
| --- | --- | --- |
| `euclidean_mst` | $O(N\log N)$ | $O(N)$ |
| `result.edges.size()` / 各メンバの参照 | $O(1)$ | $O(1)$ |
| 全辺の列挙 | $O(N)$ | $O(1)$（返り値を除く） |

## 注意点

`Coordinate` は整数型でなければならない。外接円判定の中間値は 128 bit 符号付き整数に、座標差の二乗和は 128 bit 符号なし整数に収まる必要がある。
