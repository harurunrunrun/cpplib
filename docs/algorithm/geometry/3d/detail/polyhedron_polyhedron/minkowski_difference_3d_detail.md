---
title: Minkowski Difference 3D Internal Detail (三次元Minkowski差内部実装)
documentation_of: ../../../../../../src/algorithm/geometry/3d/detail/polyhedron_polyhedron/minkowski_difference_3d_detail.hpp
---

default版とseed指定版が共有する頂点差候補の生成本体を保持する。

## API

- `minkowski_difference_3d_detail::candidate_points(first, second)`:
  二つの頂点集合の直積を走査し、全ての差 $a-b$ を返す内部関数。

## API別の時間計算量・空間計算量

$P=V_1V_2$ に対して時間・追加領域ともに $O(P)$。
公開API全体では、この後に凸包構築の計算量が加わる。

## 注意点

内部実装用headerであり、名前・引数・返り値の互換性は公開APIではない。
空入力では `std::invalid_argument`、候補数超過では
`std::length_error` を送出する。
