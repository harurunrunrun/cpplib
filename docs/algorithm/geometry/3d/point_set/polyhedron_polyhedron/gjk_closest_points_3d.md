---
title: GJK Closest Points 3D (三次元 GJK 最近点)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point_set/polyhedron_polyhedron/gjk_closest_points_3d.hpp
---

## API

- `gjk_closest_points_3d(first, second, tolerance = 1e-12L, max_iterations = 96)`: GJK 法で、第 1・第 2 の凸多面体上の最近点をこの順の `std::pair<Point3, Point3>` で返す。内部では `gjk_query_3d` を呼び、`point_on_first` と `point_on_second` を取り出す。

## API 別の時間計算量・空間計算量

反復回数を $I$（`max_iterations` 以下）、頂点数を $V_1, V_2$ とすると、時間 $O(I(V_1+V_2))$、追加領域 $O(1)$。

## 注意点

- 交差しているときは共通点を表す 1 組を返す。最近点が一意でない場合、返る組も一意ではない。
- `max_iterations` 回で収束しなかった場合も、例外を送出せずその時点の近似解を返す。
- 返す点は入力と同じ座標系である。共通の平行移動と一様なスケーリングに対して不変になるよう正規化して計算する。
- 空の多面体・非有限な頂点、正の有限値でない `tolerance`、$0$ の `max_iterations` には `std::invalid_argument` を送出する。復元した点が `long double` で表現できない場合は `std::overflow_error` を送出する。
