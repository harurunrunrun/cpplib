---
title: 3D Barycentric Coordinates (三次元重心座標)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/barycentric_coordinates.hpp
---

## API

- `barycentric_coordinates(triangle, point)`: `std::array<long double, 3>` として `{wa, wb, wc}` を返す。
- `barycentric_coordinates(tetrahedron, point)`: `std::array<long double, 4>` として `{wa, wb, wc, wd}` を返す。

重みの和は 1 であり、各頂点を重みで加重した和が `point` に一致する。

## API 別の時間計算量・空間計算量

- `barycentric_coordinates(triangle, point)`: 時間・追加空間ともに $O(1)$。
- `barycentric_coordinates(tetrahedron, point)`: 時間・追加空間ともに $O(1)$。

## 注意点

- すべての頂点と `point` は有限でなければならない。違反時は `std::invalid_argument` を送出する。
- 三角形版は、平面外の点に対しても平面への射影に相当する座標を返す。
- 退化した三角形・四面体には `std::invalid_argument` を送出する。
- 先頭の頂点を基準点とし、辺の差分から求めた局所スケールで正規化して計算する。このため、`1e-2000L` や `1e2000L` のような一様なスケールだけでなく、絶対座標が `1e3000L` で局所的な辺の長さが `1e-3000L` のように、座標軸ごとに異なる巨大な共通平行移動を含む場合でも、表現可能な辺の情報を失わない。
- `point` と基準点の差を辺のスケールで表現できない場合は `std::overflow_error` を送出する。
- 重みが `long double` で表現できない場合も `std::overflow_error` を送出する。
- 入力座標の時点で失われた差は復元できない。頂点は `long double` として互いに異なる値で格納されていなければならない。
