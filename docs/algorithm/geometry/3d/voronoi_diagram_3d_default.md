---
title: 3D Voronoi Diagram with Default Seed (固定seedによる三次元Voronoi図)
documentation_of: ../../../../src/algorithm/geometry/3d/voronoi_diagram_3d_default.hpp
---

固定seedのDelaunay四面体分割の双対として三次元Voronoi図を構築する。

## API

- `voronoi_diagram_3d(points)`: 重複siteを除き、有限頂点、cell、
  ridge、edge、無限rayとincidenceを持つ `VoronoiDiagram3` を返す。

## API別の時間計算量・空間計算量

site数を $N$、Delaunay構築量を $D,C$、出力・incidence総数を $K$ とする。
期待hash計算量の下で時間 $O(N\log N+D+C+K)$、追加領域
$O(N+D+K)$。三次元出力は最悪 $O(N^2)$。

## 注意点

アフィン次元3未満ならsiteと空cell recordだけを返す。固定seedに敵対した
入力ではDelaunay構築の期待上界を契約しない。外心が有限範囲を超えると
`std::overflow_error`。edge・ridgeの格納順は契約に含まれず、添字fieldで参照する。
