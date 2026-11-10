---
title: Point on 3D Line (三次元直線上判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/line_point/on_line.hpp
---

## API

- `on_line(line, point)`: 点が無限直線上にあるかを判定する。直線の方向と `point - line.a` が相対許容誤差 `GEOMETRY3D_EPS` の範囲で平行かで判定する。

## API 別の時間計算量・空間計算量

時間・追加領域ともに $O(1)$。

## 注意点

退化した直線、非有限な入力には `std::invalid_argument` を送出する。
