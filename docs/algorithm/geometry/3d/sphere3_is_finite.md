---
title: Sphere3 Finite Check (三次元球の有限値判定)
documentation_of: ../../../../src/algorithm/geometry/3d/sphere3_is_finite.hpp
---

## API

- `geometry3d_is_finite(sphere)` は `Sphere3` の中心座標と半径がすべて有限なら `true` を返す。

## API別の時間計算量・空間計算量

- 時間計算量: $O(1)$
- 追加空間計算量: $O(1)$

## 注意点

半径の符号は検査しない。契約検査には `geometry3d_validate` を使う。
