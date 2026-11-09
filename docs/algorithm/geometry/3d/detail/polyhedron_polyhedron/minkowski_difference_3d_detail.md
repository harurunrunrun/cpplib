---
title: Minkowski Difference 3D Internal Detail (三次元Minkowski差内部実装)
documentation_of: ../../../../../../src/algorithm/geometry/3d/detail/polyhedron_polyhedron/minkowski_difference_3d_detail.hpp
---

デフォルト版とシード指定版が共有する、頂点の差の候補を生成する本体である。

## API

- `minkowski_difference_3d_detail::candidate_points(first, second)`: 2 つの頂点集合の直積を走査し、すべての差 $a-b$ を（重複を除かずに）返す内部関数。

## API別の時間計算量・空間計算量

頂点数を $V_1, V_2$、$P=V_1V_2$ として、時間・追加領域ともに $O(P)$。
公開 API 全体では、この後に凸包構築の計算量が加わる。

## 注意点

内部実装用のヘッダであり、名前・引数・返り値の互換性は公開 API として保証しない。
どちらかの頂点集合が空なら `std::invalid_argument`、候補数が `std::vector` の上限を超えるなら `std::length_error` を送出する。
