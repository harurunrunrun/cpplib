---
title: Tetrahedron Contains Point (四面体の点包含判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/tetrahedron_point/tetrahedron_contains_point.hpp
---

## API

- `contains(tetrahedron, point)`: 点 `point` が四面体 `tetrahedron` の内部または境界にあれば `true` を返す。

## API別の時間計算量・空間計算量

- `contains`: 時間・追加領域ともに $O(1)$。

## 注意点

- 重心座標を計算し、4 つの重みがすべて許容誤差の範囲で非負なら `true` とする。
- 退化した四面体や非有限な入力には `std::invalid_argument` を、重心座標が `long double` で表現できない場合は `std::overflow_error` を送出する。
