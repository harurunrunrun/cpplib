---
title: 3D Barycentric Coordinates (三次元重心座標)
documentation_of: ../src/algorithm/geometry/3d/barycentric_coordinates.hpp
---

## API

- `barycentric_coordinates(triangle, point)`: `{wa, wb, wc}` を返す。
- `barycentric_coordinates(tetrahedron, point)`: `{wa, wb, wc, wd}` を返す。

各重みの和は1で、頂点の重み付き和が `point` となる。

## API別の時間計算量・空間計算量

- `barycentric_coordinates(triangle, point)`: 時間・追加領域 $O(1)$。
- `barycentric_coordinates(tetrahedron, point)`: 時間・追加領域 $O(1)$。

## 注意点

- 全頂点と `point` は有限でなければならず、違反時は
  `std::invalid_argument` を送出する。
- 三角形版は平面外の点も射影に相当する座標を返す。退化した三角形・
  四面体には `std::invalid_argument` を送出する。
- 座標差を局所最大絶対値で正規化してから計算する。このため一様な
  `1e-2000L` / `1e2000L` scaleや、巨大な共通平行移動を含む入力でも、
  表現可能な辺を固定絶対epsilonだけで退化とは判定しない。
- 重みが `long double` で表現できない場合は `std::overflow_error` を送出する。
- 入力座標自体で失われた差は復元できない。頂点は互いに異なる値として
  `long double` に格納されていなければならない。
