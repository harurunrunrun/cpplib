---
title: Closest Point on 3D Primitive (三次元図形上の最近点)
documentation_of: ../src/algorithm/geometry/3d/closest_point.hpp
---

## API

- `closest_point(ray, point)`: 半直線上の最近点。
- `closest_point(segment, point)`: 閉線分上の最近点。
- `closest_point(triangle, point)`: 閉三角形上の最近点。

## API別の時間計算量・空間計算量

- 各overload: 時間・追加領域 $O(1)$。

## 注意点

退化線分には対応する。退化半直線・退化三角形・非有限入力には
`std::invalid_argument` を送出する。半直線・線分は共通の局所座標ソルバ、三角形は
辺scaleで正規化したVoronoi領域判定を使い、FMAで最近点を復元する。結果を表現できない
場合は `std::overflow_error` を送出する。
