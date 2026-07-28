---
title: Hemisphere Containment Internal Detail (半球包含判定内部実装)
documentation_of: ../../../../../../src/algorithm/geometry/3d/detail/point_set/points_in_hemisphere_detail.hpp
---

二つの公開leafが共有する二変数線形実行可能性問題の内部実装を保持する。

## API

- `points_in_hemisphere_detail::feasible_point_2d`: ランダム順の線形制約から
  実行可能点を求める内部関数。
- `points_in_hemisphere_detail::fixed_coordinate_candidate`: 法線の一座標を
  $+1$ または $-1$ に固定した候補を復元する内部関数。
- その他のnamespace内要素: 境界上の一次元制約処理と内部型。

## API別の時間計算量・空間計算量

一つの二変数問題はランダム順で期待 $O(N)$、最悪 $O(N^2)$ 時間、
$O(N)$ 追加領域。公開APIは定数個の問題を解くため同じ漸近量となる。

## 注意点

内部実装用headerであり、名前・引数・返り値の互換性は公開APIではない。
利用側はdefaultまたはseed指定leaf、あるいは互換aggregatorをincludeする。
