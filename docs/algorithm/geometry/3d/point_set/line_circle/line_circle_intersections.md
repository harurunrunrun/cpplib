---
title: 3D Line and Circle Intersections (三次元直線と円の交点)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point_set/line_circle/line_circle_intersections.hpp
---

## API

- `line_circle_intersections(line, circle)`: 無限直線と円周の相異なる交点を、$0$ 個から $2$ 個の `std::vector<Point3>` で返す。

## API別の時間計算量・空間計算量

- 時間 $O(1)$、出力以外の追加領域 $O(1)$。

## 注意点

- `Circle3` は中心・法線・半径が有限、法線が非零、半径が非負でなければならない。違反時は `std::invalid_argument` を送出する。
- 退化直線（`line.a == line.b`）や非有限の直線にも `std::invalid_argument` を送出する。
- 接する場合は 1 点だけ返す。
- 直線と円を含む球との交点を求め、そのうち円の平面上にあるものを返す。差分を正規化して計算し、中間値のオーバーフローを避ける。結果を `long double` で表現できない場合は `std::overflow_error` を送出する。
