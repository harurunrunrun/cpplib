---
title: Dominant Axis from Point3 (三次元ベクトルの支配軸選択)
documentation_of: ../../../../src/algorithm/geometry/3d/dominant_axis_from_point3.hpp
---

## API

- `dominant_axis(normal)`: `Point3` で表したベクトルについて、絶対値が最大の成分に対応する `DominantAxis3` を返す。

## API別の時間計算量・空間計算量

- `dominant_axis`: 時間・追加領域 $O(1)$。

## 注意点

最大値が同じなら $x$, $y$, $z$ の順で優先する。零ベクトルには `std::invalid_argument` を送出する。
