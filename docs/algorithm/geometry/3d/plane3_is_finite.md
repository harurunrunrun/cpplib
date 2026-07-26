---
title: Plane3 Finite Check (三次元平面の有限値判定)
documentation_of: ../../../../src/algorithm/geometry/3d/plane3_is_finite.hpp
---

## API

- `geometry3d_is_finite(plane)` は `Plane3` の基準点と法線の全座標が有限なら `true` を返す。

## API別の時間計算量・空間計算量

- 時間計算量: $O(1)$
- 追加空間計算量: $O(1)$

## 注意点

零法線かどうかは検査しない。
