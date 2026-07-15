---
title: 3D Least-Squares Plane (三次元最小二乗平面)
documentation_of: ../src/algorithm/geometry/3d/least_squares_plane.hpp
---

点から平面までの直交距離の二乗和を最小化するtotal least-squares平面を求める。
平面は点集合の重心を通り、共分散行列の最小固有値に対応する方向を法線とする。

## API

- `least_squares_plane(points, relative_epsilon)`: 最適な `Plane3` を返す。
  3点未満、または点集合のaffine dimensionが2未満なら `std::nullopt`。

## 時間計算量

点数を $N$ とすると $O(N)$ 時間、戻り値を除く追加領域 $O(1)$。

## 注意点

- 通常の $z=ax+by+c$ 回帰ではなく、全座標を対等に扱う直交回帰である。
- 非有限点、不正な許容誤差には `std::invalid_argument`、表現不能な分散には
  `std::overflow_error` を送出する。
- 法線の符号は幾何学的には同じ平面を表す。
