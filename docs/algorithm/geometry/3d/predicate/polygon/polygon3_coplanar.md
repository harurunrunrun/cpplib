---
title: Polygon3 Coplanarity (三次元多角形の共面判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/polygon/polygon3_coplanar.hpp
---

## API

- `polygon3_coplanar(polygon)`: 多角形 `polygon` の全頂点が同一平面上にあるかを判定する。

## API別の時間計算量・空間計算量

- 頂点数を $N$ として時間 $O(N)$、追加領域 $O(1)$。

## 注意点

- 頂点数が 3 未満なら `std::invalid_argument` を送出する。
- 全頂点が一直線上にあり法線が定まらない場合は、共面と判定する。
- 厳密判定ではなく許容誤差付きの判定である。先頭頂点 $p_0$ から最初に得られる非零の法線 $n$ について、各頂点 $p$ が $\lvert (p - p_0) \cdot n \rvert \le \varepsilon \max(1, \lVert n \rVert \lVert p - p_0 \rVert)$ を満たすかを調べる（$\varepsilon$ は `GEOMETRY3D_EPS`）。
