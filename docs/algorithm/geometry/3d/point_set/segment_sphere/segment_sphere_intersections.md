---
title: 3D Segment-Sphere Intersections (三次元線分と球面の交点)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point_set/segment_sphere/segment_sphere_intersections.hpp
---

## API

- `segment_sphere_intersections(segment, sphere)`: 閉線分と球面の相異なる交点を、端点 `segment.a` に近い順に $0$ 個から $2$ 個返す。

## API 別の時間計算量・空間計算量

時間・出力を含む追加領域ともに $O(1)$。

## 注意点

- 退化した線分（両端点が一致）は 1 点として扱い、その点が球面上にあればその点を返す。
- `Sphere3` は中心・半径が有限かつ半径が非負でなければならない。違反時は `std::invalid_argument` を送出する。非有限な線分にも `std::invalid_argument` を送出する。
- 交点計算は共通の平行移動量を保つ正規化を行い、中間計算のオーバーフローを避ける。交点が `long double` で表現できない場合は `std::overflow_error` を送出する。
