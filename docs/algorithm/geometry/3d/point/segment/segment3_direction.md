---
title: 3D Segment Direction (三次元線分の方向)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point/segment/segment3_direction.hpp
---

## API

- `segment3_direction(segment)`: 方向ベクトル `segment.b - segment.a` を返す。

## API別の時間計算量・空間計算量

- 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- `is_zero_vector`（デフォルトの許容誤差）で零とみなされる退化線分には方向がないため、`std::invalid_argument` を送出する。方向ベクトルが非有限の場合も `std::invalid_argument` を送出する。
- 戻り値は単位化されない。
