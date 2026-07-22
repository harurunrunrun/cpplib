---
title: Point on 3D Plane (三次元平面上判定)
documentation_of: ../src/algorithm/geometry/3d/predicate/on_plane.hpp
---

## API

- `on_plane(plane, point)`: 点が平面上にあるか判定する。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

零法線または非有限入力には `std::invalid_argument` を送出する。法線と
`point-plane.point` の内積は入力 `long double` のexact dyadic値で評価し、
巨大な共通移動量や異方的な法線でも中間overflow・underflowを起こさない。
