---
title: Closest Point on 3D Sphere (三次元球面上の最近点)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point/sphere_point/sphere_closest_point.hpp
---

## API

- `closest_point(sphere, point)`: 球面上で `point` に最も近い点を返す。

## API別の時間計算量・空間計算量

- 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- `Sphere3` は中心・半径が有限で、半径が非負でなければならない。違反時は `std::invalid_argument` を送出する。 `point` が非有限の場合も同様である。
- 半径 $0$ では中心を返す。
- 正半径で `point` が（許容誤差の範囲で）球の中心にある場合は、最近点が一意でないため `std::domain_error` を送出する。
- 中心からの差分を正規化して計算し、中間値のオーバーフローを避ける。結果を `long double` で表現できない場合は `std::overflow_error` を送出する。
