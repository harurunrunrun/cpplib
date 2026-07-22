---
title: 2D Triangle Signed Area (二次元三角形の符号付き面積)
documentation_of: ../src/algorithm/geometry/2d/scalar/triangle_signed_area.hpp
---

## API

`long double triangle_signed_area(first, second, third)`

## 引数

三角形の3頂点を周回順に渡す。

## 戻り値

反時計回りなら正、時計回りなら負の面積。

## API別の時間計算量・空間計算量

- `triangle_signed_area`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

共線な3点では `0` を返す。
