---
title: Triangle Contains Point (三角形の点包含判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/triangle_point/triangle_contains_point.hpp
---

## API

- `contains(triangle, point)`: 点 `point` が三角形 `triangle` の内部または境界にあれば `true` を返す。

## API別の時間計算量・空間計算量

- `contains`: 時間・追加領域ともに $O(1)$。

## 注意点

- 点が三角形を含む平面上にない場合は `false` を返す。平面上の点については、重心座標の 3 つの重みがすべて許容誤差の範囲で非負なら `true` とする。
- 退化した三角形や非有限な入力には `std::invalid_argument` を送出する。重心座標が `long double` で表現できない場合は `std::overflow_error` を送出する。
