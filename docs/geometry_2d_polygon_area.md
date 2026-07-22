---
title: 2D Polygon Area (二次元多角形の面積)
documentation_of: ../src/algorithm/geometry/2d/scalar/polygon_area.hpp
---

## API

`long double polygon_area(polygon)`

## 引数

周回順の頂点列を渡す。

## 戻り値

頂点順によらない非負面積。

## API別の時間計算量・空間計算量

- `polygon_area`: 頂点数を $N$ として時間 $O(N)$、追加領域 $O(1)$。

## 注意点

自己交差多角形では符号付き面積の絶対値となる。
