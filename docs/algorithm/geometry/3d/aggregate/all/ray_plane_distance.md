---
title: Distance between 3D Ray and Plane (三次元半直線と平面の距離)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/ray_plane_distance.hpp
---

## API

- `distance(first, second)`: `Ray3` と `Plane3` の集合としての距離を返す。異なる型の組なので、引数を逆順にしたオーバーロード `distance(plane, ray)` も提供する。
- この互換用の集約ヘッダは、`ray_plane` 順と `plane_ray` 順の両方の個別ヘッダを読み込む。

## API別の時間計算量・空間計算量

- 全オーバーロード: 時間・追加領域ともに $O(1)$。

## 注意点

半直線と平面が交わる場合（半直線が平面に含まれる場合を含む）は 0 を返す。退化した半直線（始点と通過点が一致するもの）、法線が零ベクトルの平面、非有限な入力には `std::invalid_argument` を送出する。
