---
title: Distance between 3D Segment and Plane (三次元線分と平面の距離)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/segment_plane_distance.hpp
---

## API

- `distance(first, second)`: `Segment3` と `Plane3` の集合としての距離を返す。異なる型の組なので、引数を逆順にしたオーバーロードも提供する。
- この互換用の集約ヘッダは、`segment_plane` 順と `plane_segment` 順の両方の個別ヘッダを読み込む。

## API別の時間計算量・空間計算量

- 全オーバーロード: 時間・追加領域ともに $O(1)$。

## 注意点

交わる場合（線分が平面に含まれる場合を含む）は 0 を返す。線分は退化していてもよい。法線が零ベクトルの平面・非有限な入力には `std::invalid_argument` を送出する。
