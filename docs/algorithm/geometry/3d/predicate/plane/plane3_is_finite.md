---
title: Plane3 Finite Check (三次元平面の有限値判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/plane/plane3_is_finite.hpp
---

## API

- `geometry3d_is_finite(plane)`: `Plane3` の基準点と法線の全座標が有限なら `true` を返す（`noexcept`）。

## API 別の時間計算量・空間計算量

時間・追加領域ともに $O(1)$。

## 注意点

法線が零ベクトルかどうかは検査しない。
