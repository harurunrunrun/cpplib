---
title: Polygon Medial Axis (多角形中軸)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/polygon_medial_axis.hpp
---

単純多角形の境界に対する線分 Voronoi 図を内部へ解析的に clip し、異なる最近傍点を2個以上持つ点の集合を構築する。

## API

- `MedialAxisLocation::radius` は境界までの距離、`nearest_edges` は最短となる全境界辺番号、`nearest_points` は重複を除いた境界上の最近点列。
- `MedialAxisBranch::curve` は `curves()` の carrier 番号、`first_boundary_edge`, `second_boundary_edge` は等距離となる元境界辺番号、`first_parameter`, `second_parameter` は閉区間。
- `PolygonMedialAxis(polygon)` は中軸を構築する。
- `locate(point)` は境界までの距離・最近傍辺・異なる最近傍点を返す。
- `on_axis(point)` と互換名 `contains(point)` は点が多角形の厳密内部かつ中軸上か判定する。
- `on_branch(branch, point)` は点が指定 branch の閉区間上か判定する。
- `polygon()`, `curves()`, `branches()` は正規化済み境界、解析 carrier、公開 branch 列への const 参照を返す。

## 時間計算量

頂点数を $N$ とする。線分 Voronoi 構築と全 carrier の境界 clip・内部判定を含め、構築は最悪
$O(N^5)$ 時間、$O(N^4)$ 空間。`locate` と `on_axis` は $O(N)$、`on_branch` は $O(N)$、member 参照は $O(1)$。

## 注意点

入力は自己交差しない非退化単純多角形。共有頂点に接する複数辺が同じ最近点を与えるだけでは中軸と判定しない。
branch は直線または放物線の parameter 区間であり polyline 近似ではない。点引数と入力座標は有限値でなければならない。
