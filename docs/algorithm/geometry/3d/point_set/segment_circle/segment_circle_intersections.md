---
title: 3D Segment and Circle Intersections (三次元線分と円の交点)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point_set/segment_circle/segment_circle_intersections.hpp
---

## API

- `segment_circle_intersections(segment, circle)`: 閉線分と円周の相異なる交点を $0$ 個から $2$ 個返す。

## API 別の時間計算量・空間計算量

時間・出力以外の追加領域ともに $O(1)$。

## 注意点

- 退化した線分（両端点が一致）は 1 点として扱い、その点が円周上にあればその点を返す。
- `Circle3` は中心・法線・半径が有限、法線が非零、半径が非負でなければならない。違反時は `std::invalid_argument` を送出する。非有限な線分にも `std::invalid_argument` を送出する。
- 交点計算は共通の平行移動量を保つ正規化を行い、中間計算のオーバーフローを避ける。交点が `long double` で表現できない場合は `std::overflow_error` を送出する。
