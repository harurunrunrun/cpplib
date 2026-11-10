---
title: 3D Axis-Aligned Bounding Box (三次元軸平行境界箱)
documentation_of: ../../../../../../src/algorithm/geometry/3d/type/definition/aabb3.hpp
---

`AABB3` は閉区間の直積 $[x_{\min}, x_{\max}] \times [y_{\min}, y_{\max}] \times [z_{\min}, z_{\max}]$ で表す軸平行境界箱である（$x_{\min}$ は `minimum.x`、$x_{\max}$ は `maximum.x` などを表す）。kd-tree、Octree、三角形 BVH の枝刈りにも使う。

## API

- `AABB3{minimum, maximum}`: 2 つの隅から境界箱を構築する。
- `box.valid()`: 両隅の座標が有限で、全軸で `minimum <= maximum` なら `true` を返す。
- `box.contains(point)`: 点が箱の内部または境界上にあるか判定する。
- `box.intersects(other)`: 境界で接する場合も含め、2 つの箱が交差するか判定する。
- `box.distance(point)`: 点と箱のユークリッド距離を返す。箱の中なら $0$ を返す。
- `box.require_valid()`: 不正な箱なら `std::invalid_argument` を送出する。

## API別の時間計算量・空間計算量

すべての API は時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- `contains`、`intersects`、`distance` は箱を検査し、不正なら `std::invalid_argument` を送出する。`contains` と `distance` は点も有限でなければならず、違反時は同じく `std::invalid_argument` を送出する。
- 境界を含む閉集合として扱う。
- 距離は `std::hypot` で計算し、平方和による不要なオーバーフローを避ける。
