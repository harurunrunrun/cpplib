---
title: Triangle Contains Point (三角形の点包含判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/triangle_point/triangle_contains_point.hpp
---

## API

- `contains(triangle, point)`: 点が三角形の内部または境界にあれば`true`を返す。

## API別の時間計算量・空間計算量

- 全API: 時間・追加領域 $O(1)$。

## 注意点

点が三角形の平面上にない場合は`false`。退化三角形では例外を送出する。
