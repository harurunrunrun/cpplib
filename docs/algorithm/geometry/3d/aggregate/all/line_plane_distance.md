---
title: Distance between 3D Line and Plane (三次元直線と平面の距離)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/line_plane_distance.hpp
---

## API

- `distance(first, second)`: `Line3` と `Plane3` の集合としての距離を `long double` で返す。引数の型の順序を入れ替えたオーバーロードも提供する。
- この互換用集約ヘッダは `(Line3, Plane3)` 版と `(Plane3, Line3)` 版の両方の個別ヘッダを提供する。

## API 別の時間計算量・空間計算量

- 全オーバーロード: 時間・追加空間ともに $O(1)$。

## 注意点

- 交差する場合、または平面に含まれる場合は 0 を返す。
- 退化した直線、法線が零ベクトルの平面、非有限な入力には `std::invalid_argument` を送出する。
