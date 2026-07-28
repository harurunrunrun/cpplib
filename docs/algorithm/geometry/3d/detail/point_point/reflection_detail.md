---
title: 3D Reflection Detail (三次元鏡映内部処理)
documentation_of: ../../../../../../src/algorithm/geometry/3d/detail/point_point/reflection_detail.hpp
---

## API

- `geometry3d_reflection_detail::reflect_from_projection(point, projected)`: 元の点と射影点から鏡映点を復元する内部関数。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

公開APIではない。座標差を正規化してから復元し、中間計算のoverflowを避ける。
