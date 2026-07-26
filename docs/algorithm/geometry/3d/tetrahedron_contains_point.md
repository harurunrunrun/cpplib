---
title: Tetrahedron Contains Point (四面体の点包含判定)
documentation_of: ../../../../src/algorithm/geometry/3d/tetrahedron_contains_point.hpp
---

## API

- `contains(tetrahedron, point)`: 点が四面体の内部または境界にあれば`true`を返す。

## API別の時間計算量・空間計算量

- 全API: 時間・追加領域 $O(1)$。

## 注意点

退化四面体または非有限入力では例外を送出する。
