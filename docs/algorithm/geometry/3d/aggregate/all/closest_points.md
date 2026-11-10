---
title: Closest Points in 3D (三次元最近点対)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/closest_points.hpp
---

## API

- `closest_points(first_line, second_line)`: 2 本の無限直線上の最近点対を 1 組、`std::pair<Point3, Point3>` で返す。
- `closest_points(first_segment, second_segment)`: 2 本の閉線分上の最近点対を 1 組、`std::pair<Point3, Point3>` で返す。

## API 別の時間計算量・空間計算量

- 各オーバーロード: 時間・追加空間ともに $O(1)$。

## 注意点

- 平行な場合は、複数ある最近点対のうち 1 組を返す。
- 線分は退化していてもよいが、直線は非退化でなければならない。
- 共通の基準点、局所スケール、単位方向ベクトル、パラメータ区間に変換して解く。角度が小さい場合は $1 - (\text{dot})^2$ を作らず、外積の大きさを使う。
- 非有限な入力または退化した直線には `std::invalid_argument` を送出する。
- 最近点、または局所的に正規化した差分を `long double` で表現できない場合は `std::overflow_error` を送出する。
