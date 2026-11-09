---
title: Closest Point on 3D Tetrahedron (三次元四面体上の最近点)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point/tetrahedron_point/tetrahedron_closest_point.hpp
---

## API

- `closest_point(tetrahedron, point)`: 閉四面体上で `point` に最も近い点を返す。`point` が四面体の内部または境界上なら `point` 自身を返す。

## API別の時間計算量・空間計算量

- 4 面を調べるだけなので、時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- 退化四面体（`adaptive_orient3d` による厳密判定）や非有限入力には `std::invalid_argument` を送出する。
