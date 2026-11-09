---
title: Line-Point Projection (直線への点の射影)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point/line_point/line_point_projection.hpp
---

## API

- `projection(line, point)`: `point` から直線へ下ろした垂線の足を返す。

## API別の時間計算量・空間計算量

- 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- 直線の 2 点は相異なり、全座標は有限値でなければならない。違反時は `std::invalid_argument` を送出する。
- 差分を正規化して計算するため、座標が大きくても中間値はオーバーフローしにくい。結果を表現できない場合は `std::overflow_error` を送出することがある。
