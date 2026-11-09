---
title: Sphere3 Finite Check (三次元球の有限値判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/sphere/sphere3_is_finite.hpp
---

## API

- `geometry3d_is_finite(sphere)`: `Sphere3` の中心座標と半径がすべて有限なら `true` を返す。

## API別の時間計算量・空間計算量

- 時間計算量: $O(1)$
- 追加空間計算量: $O(1)$

## 注意点

- 例外は送出しない（`noexcept`）。
- 半径の符号は検査しない。契約の検査には `geometry3d_validate` を使う。
