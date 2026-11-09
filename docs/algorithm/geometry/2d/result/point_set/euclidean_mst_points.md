---
title: Euclidean MST from Integer Points (整数点列のユークリッド最小全域木)
documentation_of: ../../../../../../src/algorithm/geometry/2d/result/point_set/euclidean_mst_points.hpp
---

2 次元の整数座標点を頂点とする完全グラフについて、ユークリッド距離による最小全域木を求める。

## API

```cpp
template<std::integral Coordinate>
EuclideanMstResult euclidean_mst(
    const std::vector<std::pair<Coordinate, Coordinate>>& points
);
```

整数演算による Delaunay 三角形分割の辺を候補とし、平方距離を重みとして Kruskal 法を行う。同一座標の点も別の頂点として扱い、重み 0 の辺で結ぶ。返り値の `edges` は `EuclideanMstEdge{from, to, squared_distance}` の列で、平方距離の昇順（同率なら頂点対の辞書順）に採用した順に並ぶ。`squared_distance` の型は `__uint128_t` である。

## 時間計算量

入力点数を $N$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `euclidean_mst(points)` | $O(N \log N)$ | $O(N)$ |

## 注意点

- `Coordinate` は C++ の整数型でなければならない。
- 向き判定と外接円判定の中間値は 128 bit 符号付き整数に収まる必要がある。
- 座標差の二乗和は 128 bit 符号なし整数に収まる必要がある。
- 空入力では空の辺列を返す。

## 使用例

```cpp
std::vector<std::pair<long long, long long>> points{
    {0, 0}, {3, 0}, {0, 4},
};
const EuclideanMstResult result = euclidean_mst(points);
```
