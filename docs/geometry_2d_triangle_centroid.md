---
title: 2D Triangle Centroid (二次元三角形の重心)
documentation_of: ../src/algorithm/geometry/2d/triangle_centroid.hpp
---

## API

`Point triangle_centroid(first, second, third)`

## 引数

3頂点を渡す。

## 戻り値

3頂点の座標平均を返す。

## API別の時間計算量・空間計算量

- `triangle_centroid`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

退化した3点にも座標平均を定義する。最初の頂点からの2差分を3等分して加えるため、3頂点の絶対座標和を中間計算しない。
