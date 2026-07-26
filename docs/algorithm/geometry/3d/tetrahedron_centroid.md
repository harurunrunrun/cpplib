---
title: Tetrahedron Centroid (四面体の重心)
documentation_of: ../../../../src/algorithm/geometry/3d/tetrahedron_centroid.hpp
---

## API

- `centroid(tetrahedron)`: 四頂点の算術平均を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

頂点は有限でなければならず、違反時は `std::invalid_argument` を送出する。退化四面体にも算術平均を返す。
