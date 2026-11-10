---
title: Polygon Medial Axis (多角形中軸)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/polygon_medial_axis.hpp
---

単純多角形の境界辺に対する線分 Voronoi 図を多角形の内部へ解析的にクリップし、境界上の相異なる最近点を 2 個以上もつ点の集合（中軸）を構築する。

## API

- `MedialAxisLocation` の `radius` は境界までの距離、`nearest_edges` は距離が最短となるすべての境界辺の番号、`nearest_points` は重複を除いた境界上の最近点の列。
- `MedialAxisBranch` の `curve` は `curves()` における曲線（carrier）の番号、`first_boundary_edge`, `second_boundary_edge` は等距離となる元の境界辺の番号、`first_parameter`, `second_parameter` は閉じたパラメータ区間の両端。
- `explicit PolygonMedialAxis(std::vector<Point> polygon)` は中軸を構築する。
- `locate(point)` は境界までの距離・最近の辺・相異なる最近点を返す。
- `on_axis(point)` と互換用の別名 `contains(point)` は、点が多角形の厳密な内部にあり、かつ中軸上にあるかを判定する。
- `on_branch(branch, point)` は点が指定した枝の閉区間上にあるかを判定する。`branch` が範囲外なら `std::out_of_range` を送出する。
- `polygon()`, `curves()`, `branches()` は、正規化済みの境界、解析的な曲線の列、公開する枝の列への const 参照を返す。

## 時間計算量

頂点数を $N$ とする。線分 Voronoi 図の構築と、全曲線の境界によるクリップ・内部判定を含め、構築は最悪
$O(N^5)$ 時間、$O(N^4)$ 空間。`locate`, `on_axis`, `on_branch` は $O(N)$、メンバの参照は $O(1)$。

## 注意点

- 入力は自己交差しない非退化な単純多角形とする。入力座標と点の引数は有限値でなければならない。
- 共有頂点に接する複数の辺が同じ最近点を与えるだけでは、中軸上とは判定しない。
- 枝は直線または放物線のパラメータ区間であり、折れ線による近似ではない。
