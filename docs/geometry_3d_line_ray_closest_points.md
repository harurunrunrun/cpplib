---
title: Closest Points between 3D Line and Ray (三次元直線と半直線の最近点対)
documentation_of: ../src/algorithm/geometry/3d/line_ray_closest_points.hpp
---

## API

- `closest_points(first, second)`: `Line3` と `Ray3` 上の最近点を引数と同じ順のpairで返す。異種型では逆順overloadも提供する。

## API別の時間計算量・空間計算量

- 全overload: 時間・追加領域 $O(1)$。

## 注意点

最近点対が複数ある場合はそのうち1組を返す。退化した直線・半直線・非有限入力には
`std::invalid_argument` を送出する。共通anchorの局所座標とparameter区間で解き、
結果を表現できない場合は `std::overflow_error` を送出する。
