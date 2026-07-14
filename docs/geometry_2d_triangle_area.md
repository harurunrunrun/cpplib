---
title: 2D Triangle Area (二次元三角形の面積)
documentation_of: ../src/algorithm/geometry/2d/triangle_area.hpp
---

## API

`long double triangle_area(first, second, third)`

## 引数

三角形の3頂点を渡す。

## 戻り値

向きによらない非負面積。

## API別の時間計算量・空間計算量

- `triangle_area`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

共線な3点では `0` を返す。
