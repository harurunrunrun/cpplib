---
title: 2D Triangle Centroid (二次元三角形の重心)
documentation_of: ../../../../../../src/algorithm/geometry/2d/point/point_point_point/triangle_centroid.hpp
---

## API

`Point triangle_centroid(first, second, third)`

## 引数

3 頂点を渡す。

## 戻り値

3 頂点の座標の平均を返す。

## API別の時間計算量・空間計算量

- `triangle_centroid`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

退化した 3 点に対しても座標の平均を返す（例外は送出しない）。
最初の頂点からの 2 つの差分をそれぞれ 3 で割って加えるので、3 頂点の絶対座標の和を中間計算することはない。
