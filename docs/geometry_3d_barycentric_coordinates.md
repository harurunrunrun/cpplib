---
title: 3D Barycentric Coordinates (三次元重心座標)
documentation_of: ../src/algorithm/geometry/3d/query/barycentric_coordinates.hpp
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
- 先頭頂点をanchorとし、辺差分から求めた局所scaleで正規化して計算する。
  このため一様な`1e-2000L` / `1e2000L` scaleだけでなく、絶対座標が
  `1e3000L`で局所辺が`1e-3000L`のように異なる座標軸へ巨大な共通平行移動を
  含む場合も、表現可能な辺を失わない。
- `point - anchor`を辺scaleで表現できない場合も`std::overflow_error`を送出する。
- 重みが `long double` で表現できない場合は `std::overflow_error` を送出する。
- 入力座標自体で失われた差は復元できない。頂点は互いに異なる値として
  `long double` に格納されていなければならない。
