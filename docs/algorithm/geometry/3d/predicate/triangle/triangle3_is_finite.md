---
title: Triangle3 Finite Check (三次元三角形の有限値判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/triangle/triangle3_is_finite.hpp
---

## API

- `geometry3d_is_finite(triangle)` は `Triangle3` の三頂点の全座標が有限なら `true` を返す。

## API別の時間計算量・空間計算量

- 時間計算量: $O(1)$
- 追加空間計算量: $O(1)$

## 注意点

面積0かどうかは検査しない。
