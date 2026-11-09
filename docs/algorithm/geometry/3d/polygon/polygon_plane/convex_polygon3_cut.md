---
title: Convex Polygon3 Cut (三次元凸多角形の半空間切断)
documentation_of: ../../../../../../src/algorithm/geometry/3d/polygon/polygon_plane/convex_polygon3_cut.hpp
---

## API

- `convex_polygon3_cut(polygon, halfspace)`: 凸な共面多角形 `polygon` のうち、平面 `halfspace` の法線側の閉半空間（符号付き距離が $0$ 以上の側）に残る部分を、元の頂点順で返す。

## API 別の時間計算量・空間計算量

頂点数を $N$ として、時間・戻り値の領域ともに $O(N)$。

## 注意点

- 入力は凸な共面多角形でなければならない（検査はしない）。
- 側の判定は `signed_distance` と許容誤差付きの符号判定で行い、境界をまたぐ辺では線形補間した交点を加える。
- 3 頂点未満の多角形、零法線の平面、非有限な頂点には `std::invalid_argument` を送出する。符号付き距離が表現できない場合は `std::overflow_error` を送出する。
