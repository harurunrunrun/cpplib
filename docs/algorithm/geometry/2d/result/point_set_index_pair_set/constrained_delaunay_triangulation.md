---
title: Constrained Delaunay Triangulation (制約付きDelaunay三角形分割)
documentation_of: ../../../../../../src/algorithm/geometry/2d/result/point_set_index_pair_set/constrained_delaunay_triangulation.hpp
---

点集合を、指定した非交差線分を必ず辺に持つ局所Delaunay三角形分割へ変換する。

## API

`constrained_delaunay_triangulation(points, constraints)` は `triangles`, `edges`,
`constraints` を返す。constraint上にsiteがある場合は、幾何順に隣接site間へ分割して返す。

## 時間計算量

点数を $N$、分割後constraint数を $C$、三角形数を $T=O(N)$ とする。
constraint分割は $O(NC)$、交差検査は $O(C^2)$。constraint回復は各constraintで
最大 $T^2$ 回flipし、各回のadjacency再構築が $O(T\log T)$ なので
$O(CT^3\log T)$。後段の局所Delaunay修復は最大 $T^2$ passで、各passに
最大 $T$ 辺とprotected constraint判定を行うため
$O(T^3(C+\log T))$。全体は
$O(NC+C^2+CT^3\log T+T^3(C+\log T))$ 時間、$O(T+C)$ 空間。

## 注意点

constraintは交差してはならないが、T字接続、同一直線上の重複・逆向き指定は許す。
同一座標siteは拒否する。全点共線では三角形を空とし、隣接site chainと分割constraintを返す。
