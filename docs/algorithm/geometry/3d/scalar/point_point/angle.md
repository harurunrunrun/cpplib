---
title: 3D Vector Angle (三次元ベクトル角)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/point_point/angle.hpp
---

## API

- `angle(first, second)`: 2 ベクトルのなす角を $[0, \pi]$ の範囲（ラジアン）で返す。

## API別の時間計算量・空間計算量

- 時間・追加領域ともに $O(1)$。

## 注意点

- 零ベクトルまたは非有限なベクトルには `std::invalid_argument` を送出する。
- 両ベクトルをオーバーフロー・アンダーフローしない方法で単位化し、$\operatorname{atan2}(\lVert a \times b \rVert, a \cdot b)$ から角を求める。
- そのため、一様に巨大または微小なスケールでも積のオーバーフローを避けられ、`acos` を使う場合のような定義域外への丸めも起きない。
