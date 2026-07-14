---
title: 2D Polygon Perimeter (二次元多角形の周長)
documentation_of: ../src/algorithm/geometry/2d/polygon_perimeter.hpp
---

## API

`long double polygon_perimeter(polygon)`

## 引数

周回順の頂点列を渡す。

## 戻り値

隣接頂点間距離と末尾・先頭間距離の総和。

## API別の時間計算量・空間計算量

- `polygon_perimeter`: 頂点数を $N$ として時間 $O(N)$、追加領域 $O(1)$。

## 注意点

頂点数0または1では `0`。
