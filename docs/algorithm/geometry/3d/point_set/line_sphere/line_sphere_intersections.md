---
title: 3D Line-Sphere Intersections (三次元直線と球面の交点)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point_set/line_sphere/line_sphere_intersections.hpp
---

## API

- `line_sphere_intersections(line, sphere)`: 無限直線と球面の相異なる交点を、直線の向き（`line.a` から `line.b` への向き）の順に $0$ 個から $2$ 個の `std::vector<Point3>` で返す。

## API別の時間計算量・空間計算量

- 時間 $O(1)$、追加領域 $O(1)$（出力を含む）。

## 注意点

- `Sphere3` は中心・半径が有限で、半径が非負でなければならない。違反時は `std::invalid_argument` を送出する。
- 退化直線（`line.a == line.b`）や非有限の直線にも `std::invalid_argument` を送出する。
- 接する場合は 1 点だけ返す。
- 差分を正規化して計算し、中間値のオーバーフローを避ける。結果を `long double` で表現できない場合は `std::overflow_error` を送出する。
