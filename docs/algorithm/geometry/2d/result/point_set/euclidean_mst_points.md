---
title: Euclidean MST from Integer Points (整数点列のユークリッド最小全域木)
documentation_of: ../../../../../../src/algorithm/geometry/2d/result/point_set/euclidean_mst_points.hpp
---

二次元整数座標点の完全グラフに対するユークリッド最小全域木を求める。

## API

```cpp
template<std::integral Coordinate>
EuclideanMstResult euclidean_mst(
    const std::vector<std::pair<Coordinate, Coordinate>>& points
);
```

Delaunay三角形分割の辺を候補とし、平方距離を重みとしてKruskal法を行う。
同一座標の点も別頂点として扱う。

## 時間計算量

入力点数を $N$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `euclidean_mst(points)` | $O(N\log N)$ | $O(N)$ |

## 注意点

- `Coordinate` はC++の整数型でなければならない。
- 向きと外接円判定の中間値は128-bit符号付き整数に収まる必要がある。
- 座標差の二乗和は128-bit符号なし整数に収まる必要がある。
- 空入力では空の辺列を返す。

## 使用例

```cpp
std::vector<std::pair<long long, long long>> points{
    {0, 0}, {3, 0}, {0, 4},
};
const EuclideanMstResult result = euclidean_mst(points);
```
