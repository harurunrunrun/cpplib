---
title: Segment3 Finite Check (三次元線分の有限値判定)
documentation_of: ../../../../src/algorithm/geometry/3d/segment3_is_finite.hpp
---

## API

- `geometry3d_is_finite(segment)` は `Segment3` の両端点の全座標が有限なら `true` を返す。

## API別の時間計算量・空間計算量

- 時間計算量: $O(1)$
- 追加空間計算量: $O(1)$

## 注意点

長さ0の線分も、座標が有限なら `true` になる。
