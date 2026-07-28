---
title: 3D Barycentric Coordinate Detail (三次元重心座標内部処理)
documentation_of: ../../../../../../src/algorithm/geometry/3d/detail/point_point_point/barycentric_coordinates_detail.hpp
---

## API

- `edge_scale(points)`: 頂点列の安全な正規化尺度を得る。
- `normalized_difference(left, right, scale)`: 指定尺度で座標差を正規化する。
- `vector_scale(...)`: 二本または三本のベクトルの最大絶対座標を得る。

## API別の時間計算量・空間計算量

- 全API: 時間・追加領域 $O(1)$。

## 注意点

公開APIではない。入力点は有限値でなければならない。
