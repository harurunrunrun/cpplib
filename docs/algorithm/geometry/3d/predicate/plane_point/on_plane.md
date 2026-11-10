---
title: Point on 3D Plane (三次元平面上判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/plane_point/on_plane.hpp
---

## API

- `on_plane(plane, point)`: 点が平面上にあるかを判定する。法線 $n$ と $d$ = `point - plane.point` の内積 $n \cdot d$ の絶対値が、各項の絶対値の和 $\sum_i \lvert n_i d_i \rvert$ の `GEOMETRY3D_EPS` 倍以下なら `true` を返す。

## API 別の時間計算量・空間計算量

時間・追加領域ともに $O(1)$。

## 注意点

- 零法線または非有限な入力には `std::invalid_argument` を送出する。
- 内積と比較は、入力の `long double` を exact dyadic 数とみなした厳密演算で評価する。このため、巨大な共通の平行移動量や異方的な法線でも、中間計算のオーバーフロー・アンダーフローを起こさない。
