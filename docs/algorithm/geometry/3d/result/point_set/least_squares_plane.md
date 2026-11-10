---
title: 3D Least-Squares Plane (三次元最小二乗平面)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/point_set/least_squares_plane.hpp
---

各点から平面までの直交距離の二乗和を最小化する平面（全最小二乗平面）を求める。平面は点集合の重心を通り、共分散行列の最小固有値に対応する固有ベクトルを法線とする。

## API

- `least_squares_plane(points, relative_epsilon = GEOMETRY3D_EPS)`: 最適な平面を `std::optional<Plane3>` で返す。点が 3 個未満の場合、または点集合が平面を定めない（最大分散が $0$、または第 2 分散が `relative_epsilon` 倍の最大分散以下で、アフィン次元が 2 未満とみなされる）場合は `std::nullopt` を返す。

## 時間計算量

点数を $N$ とすると時間 $O(N)$、戻り値を除く追加空間 $O(1)$。

## 注意点

- 通常の $z = ax + by + c$ 型の回帰ではなく、全座標を対等に扱う直交回帰である。
- 非有限な点、または負・非有限の `relative_epsilon` には `std::invalid_argument` を、分散が表現できない場合は `std::overflow_error` を送出する。
- 法線の符号は定まらないが、どちらの符号でも幾何的には同じ平面を表す。
