---
title: Polygon3 Point Containment (三次元多角形の点包含)
documentation_of: ../../../../../../src/algorithm/geometry/3d/integer/polygon_point/polygon3_contains.hpp
---

## API

- `polygon3_contains(polygon, point)`: 点が多角形の外部なら `0`、境界上なら `1`、内部なら `2` を返す。多角形の平面上にない点は外部とする。

## API別の時間計算量・空間計算量

- 頂点数を $N$ として、時間・追加領域ともに $O(N)$。

## 注意点

単純な共面多角形を与える。頂点数が 3 未満、または面積が 0（許容誤差 `GEOMETRY3D_EPS` 以下）の多角形には `std::invalid_argument` を送出する。
平面上にあるかどうかは、平面からの高さが $\varepsilon \cdot \max(1, d)$（$\varepsilon$ は `GEOMETRY3D_EPS`、$d$ は多角形の基準頂点からの距離）以下かどうかで判定し、平面上の点は二次元に射影して包含判定する。
