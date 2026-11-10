---
title: Signed Distance to 3D Plane (三次元平面への符号付き距離)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/plane_point/signed_distance.hpp
---

## API

- `signed_distance(plane, point)`: 入力された法線の向きを正として、点から平面への符号付き距離を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域ともに $O(1)$。

## 注意点

- 法線が零ベクトルの場合や、入力が非有限の場合は `std::invalid_argument` を送出する。
- 点と平面上の基準点 `plane.point` の差を局所的なスケールで正規化し、安全に単位化した法線との内積をとってから距離を復元する。
- 非零の結果を `long double` で表現できない場合（オーバーフローまたはアンダーフローで $0$ になる場合）は `std::overflow_error` を送出する。
