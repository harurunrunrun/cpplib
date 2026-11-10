---
title: Point3 Finite Check (三次元点の有限値判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/point/point3_is_finite.hpp
---

## API

- `geometry3d_is_finite(point)`: `Point3` の全座標が有限なら `true` を返す（`noexcept`）。

## API 別の時間計算量・空間計算量

時間・追加領域ともに $O(1)$。

## 注意点

座標値が有限かだけを判定し、点の用途に固有な退化条件は検査しない。
