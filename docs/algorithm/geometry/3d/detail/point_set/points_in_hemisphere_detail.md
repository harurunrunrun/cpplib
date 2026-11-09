---
title: Hemisphere Containment Internal Detail (半球包含判定内部実装)
documentation_of: ../../../../../../src/algorithm/geometry/3d/detail/point_set/points_in_hemisphere_detail.hpp
---

2 つの公開個別ヘッダが共有する、二変数の線形実行可能性問題の内部実装である。

## API

- `points_in_hemisphere_detail::feasible_point_2d(constraints, order, tolerance)`: 線形制約を `order` の順に処理する増分法（Seidel 型）で実行可能点を求める内部関数。実行不能なら `std::nullopt` を返す。
- `points_in_hemisphere_detail::fixed_coordinate_candidate(points, order, fixed_coordinate, fixed_value, tolerance)`: 法線の 1 座標を $+1$ または $-1$ に固定した二変数問題を解き、候補の法線を復元する内部関数。
- その他の名前空間内の要素: 境界上の一次元制約の処理と内部型。

## API別の時間計算量・空間計算量

1 つの二変数問題は、ランダムな順序なら期待時間 $O(N)$、最悪時間 $O(N^2)$、追加領域 $O(N)$。公開 API は定数個の問題を解くので、漸近的には同じ計算量となる。

## 注意点

内部実装用のヘッダであり、名前・引数・返り値の互換性は公開 API として保証しない。
利用側はデフォルト版またはシード指定版の個別ヘッダ、あるいは互換用の集約ヘッダを include すること。
