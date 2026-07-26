---
title: Circle3 Finite Check (三次元円の有限値判定)
documentation_of: ../../../../src/algorithm/geometry/3d/circle3_is_finite.hpp
---

## API

- `geometry3d_is_finite(circle)` は `Circle3` の中心・法線・半径がすべて有限なら `true` を返す。

## API別の時間計算量・空間計算量

- 時間計算量: $O(1)$
- 追加空間計算量: $O(1)$

## 注意点

法線の非零性や半径の符号は検査しない。契約検査には `geometry3d_validate` を使う。
