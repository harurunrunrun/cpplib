---
title: Signed Distance to 3D Plane (三次元平面への符号付き距離)
documentation_of: ../src/algorithm/geometry/3d/scalar/signed_distance.hpp
---

## API

- `signed_distance(plane, point)`: 入力法線の向きを正とする平面への符号付き距離を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

零法線または非有限入力には `std::invalid_argument` を送出する。点と平面anchorの差を
局所scaleへ正規化し、安全に単位化した法線との内積から距離を復元する。結果を
`long double` で表現できない場合は `std::overflow_error` を送出する。
