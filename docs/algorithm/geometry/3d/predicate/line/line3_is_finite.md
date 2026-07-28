---
title: Line3 Finite Check (三次元直線の有限値判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/line/line3_is_finite.hpp
---

## API

- `geometry3d_is_finite(line)` は `Line3` の二点の全座標が有限なら `true` を返す。

## API別の時間計算量・空間計算量

- 時間計算量: $O(1)$
- 追加空間計算量: $O(1)$

## 注意点

二点の一致は検査しない。直線として非退化かは利用側APIが検査する。
