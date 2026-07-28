---
title: Line-Point Projection (直線への点の射影)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point/line_point/line_point_projection.hpp
---

## API

- `projection(line, point)`: 点から直線へ下ろした垂線の足を返す。

## API別の時間計算量・空間計算量

- 全API: 時間・追加領域 $O(1)$。

## 注意点

直線の二点は相異なり、全座標は有限値でなければならない。
