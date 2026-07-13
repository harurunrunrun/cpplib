---
title: Half-Plane Contains (半平面包含判定)
documentation_of: ../src/algorithm/geometry/advanced/half_plane_contains.hpp
---

## API

- `half_plane_contains(half_plane, point)`: 有向直線 `a -> b` の左側の閉半平面に点があるか判定する。

## API別の時間計算量・空間計算量

- 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

境界を含む。退化した境界線には `std::invalid_argument` を送出する。
