---
title: 2D Polygon Centroid (二次元多角形の面積重心)
documentation_of: ../src/algorithm/geometry/2d/polygon_centroid.hpp
---

## API

`Point polygon_centroid(polygon)`

## 引数

単純多角形の周回順頂点列を渡す。

## 戻り値

一様な面密度を持つ多角形の重心。

## API別の時間計算量・空間計算量

- `polygon_centroid`: 頂点数を $N$ として時間 $O(N)$、追加領域 $O(1)$。

## 注意点

符号付き面積が0なら `std::invalid_argument`。時計回り入力にも対応する。
