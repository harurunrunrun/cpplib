---
title: Point to 3D Circle Distance (点と三次元円の距離)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/circle_point/circle_point_distance.hpp
---

## API

- `distance(circle, point)`: 点と円周との最短距離を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域ともに $O(1)$。

## 注意点

- 円板ではなく円周への距離である。
- `Circle3` は中心・法線・半径が有限で、法線が非零、半径が非負でなければならない。違反時（負の半径、零法線を含む）や点が非有限の場合は `std::invalid_argument` を送出する。
- 点と中心の差を半径も含めた共通のスケールで正規化してから計算するため、中間計算でのオーバーフローを避けられる。
- 計算結果が `long double` で表現できない場合は `std::overflow_error` を送出する。
