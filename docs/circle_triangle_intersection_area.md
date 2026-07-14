---
title: Circle-Triangle Intersection Area (円と三角形の共通部分面積)
documentation_of: ../src/algorithm/geometry/2d/circle_triangle_intersection_area.hpp
---

## API

- `circle_triangle_intersection_area(circle, first, second, third)`: 円盤と三角形の共通部分面積を返す。

## 引数

`circle` は非負半径の円、3点は三角形の頂点である。

## 戻り値

向きによらない非負面積を `long double` で返す。

## API別の時間計算量・空間計算量

時間・追加領域 $O(1)$。

## 注意点

退化三角形では0。負半径には `std::invalid_argument` を送出する。
