---
title: 3D Linear Intersection Result (三次元線形図形の交差集合型)
documentation_of: ../src/algorithm/geometry/3d/linear_intersection3.hpp
---

## API

- `LinearIntersection3`: `std::variant<std::monostate, Point3, Segment3, Ray3, Line3>`。順に空集合、1点、閉線分、閉半直線、無限直線を表す。

## API別の時間計算量・空間計算量

- 構築・判別・取得は時間・追加領域 $O(1)$。

## 注意点

交差集合の次元と非有界性を型で保持する。`std::get_if` または `std::holds_alternative` で安全に判別する。
