---
title: Randomized 3D Voronoi Diagram (乱択三次元Voronoi図)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/point_set/voronoi_diagram_3d_randomized.hpp
---

指定seedのDelaunay四面体分割の双対として三次元Voronoi図を構築する。

## API

- `voronoi_diagram_3d_randomized(points, seed)`: 増分順序をseedで指定し、
  有限頂点、cell、ridge、edge、無限rayとincidenceを返す。

## API別の時間計算量・空間計算量

site数を $N$、Delaunay構築量を $D$、出力・incidence総数を $K$ とする。
seedを入力と独立に選ぶ一般位置入力では、期待hash計算量の下で期待時間
$O(N\log N+D+K)$、期待追加領域 $O(N+D+K)$。出力は最悪 $O(N^2)$。

## 注意点

同じ入力とseedには同じ結果を返し、seedは幾何学的なVoronoi領域を変えない。
アフィン次元3未満ならsiteと空cell recordだけを返す。外心が有限範囲を超えると
`std::overflow_error`。格納順ではなくincidence添字を使って参照する。
