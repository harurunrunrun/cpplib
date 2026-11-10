---
title: Line3 Finite Check (三次元直線の有限値判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/line/line3_is_finite.hpp
---

## API

- `geometry3d_is_finite(line)`: `Line3` の 2 点の全座標が有限なら `true` を返す（`noexcept`）。

## API 別の時間計算量・空間計算量

時間・追加領域ともに $O(1)$。

## 注意点

2 点が一致するかは検査しない。直線として退化していないかは、利用する側の API が検査する。
