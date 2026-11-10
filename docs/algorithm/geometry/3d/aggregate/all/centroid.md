---
title: 3D Centroid (三次元重心)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/centroid.hpp
---

## API

- `centroid(triangle)`: 3 頂点の平均を `Point3` で返す。
- `centroid(tetrahedron)`: 4 頂点の平均を `Point3` で返す。

## API 別の時間計算量・空間計算量

- `centroid(triangle)`: 時間・追加空間ともに $O(1)$。
- `centroid(tetrahedron)`: 時間・追加空間ともに $O(1)$。

## 注意点

- 頂点はすべて有限でなければならない。違反時は `std::invalid_argument` を送出する。
- 座標成分ごとに最大絶対値で正規化してから平均をとるため、3 点・4 点を先に加算する実装で起こる途中のオーバーフローを避けられる。
- 退化した図形に対しても算術平均を返し、退化の検査は行わない。
