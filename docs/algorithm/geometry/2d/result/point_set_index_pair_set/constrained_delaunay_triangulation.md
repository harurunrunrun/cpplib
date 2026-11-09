---
title: Constrained Delaunay Triangulation (制約付き Delaunay 三角形分割)
documentation_of: ../../../../../../src/algorithm/geometry/2d/result/point_set_index_pair_set/constrained_delaunay_triangulation.hpp
---

点集合の Delaunay 三角形分割を、指定した互いに交差しない線分（制約辺）を必ず辺に持つ局所 Delaunay な三角形分割へ変換する。

## API

```cpp
ConstrainedDelaunayTriangulationResult constrained_delaunay_triangulation(
    const std::vector<Point>& points,
    std::vector<std::pair<std::size_t, std::size_t>> constraints
);
```

`constraints` は点の添字の組で制約辺を指定する。返り値は三角形 `triangles`（ソート済み）、無向辺 `edges`、制約辺 `constraints` を持つ。制約辺の上に他の site がある場合は、その制約辺を線分上の順に隣接 site 間の辺へ分割し、`(小さい添字, 大きい添字)` の形に揃え、重複を除いて返す。

## 時間計算量

点数を $N$、分割後の制約辺の数を $C$、三角形の数を $T = O(N)$ とする。

- 制約辺の分割: $O(NC)$
- 制約辺どうしの交差検査: $O(C^2)$
- 制約辺の回復: 各制約辺について最大 $T^2$ 回 flip し、flip のたびに隣接関係を $O(T \log T)$ で再構築するので $O(C T^3 \log T)$
- 局所 Delaunay 性の修復: 最大 $T^2$ パスで、各パスで最大 $T$ 本の辺について保護された制約辺との交差判定を行うので $O(T^3 (C + \log T))$

全体で時間計算量は $O(NC + C^2 + C T^3 \log T + T^3 (C + \log T))$、空間計算量は $O(T + C)$ である。

## 注意点

- 制約辺どうしは交差してはならないが、T 字型の接続、同一直線上での重複、逆向きの指定は許す。
- 次の場合は `std::invalid_argument` を送出する: 座標が有限でない、添字が範囲外または両端が同じ添字、両端が同一座標、制約辺どうしが交差する、同一座標の site が複数ある、制約辺を回復できない。
- 全点が共線の場合は三角形を空とし、隣接 site を結ぶ辺の列と分割後の制約辺を返す。このとき site の列に含まれない制約辺があれば `std::invalid_argument` を送出する。
