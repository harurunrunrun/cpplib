---
title: GJK Query 3D (3次元GJK問い合わせ)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/polyhedron_polyhedron/gjk_query_3d.hpp
---

GJK 法により、2 つの凸多面体の交差判定・距離・最近点をまとめて求める。

## API

- `gjk_query_3d(first, second, tolerance = 1e-12L, max_iterations = 96)`: サポート写像と単体の縮約を用いて、`GJKResult3` を返す。結果は交差の有無 `intersects`、距離 `distance`、両形状上の最近点 `point_on_first`, `point_on_second`、法線 `normal`、終了時の単体 `simplex`、反復回数 `iterations`、収束したか `converged` を持つ。

## API別の時間計算量・空間計算量

- 頂点数を $V_1, V_2$、実際の反復回数を $I$ とすると、時間 $O(I(V_1+V_2))$。追加空間は入力を正規化したコピーの分 $O(V_1+V_2)$（面の配列も複製される）で、返す単体は高々 4 点である。

## 注意点

- 有限な頂点を持つ空でない凸形状を前提とする。空の形状、非有限な頂点、非正または非有限の `tolerance`、反復上限 $0$ には `std::invalid_argument` を送出する。反復上限に達した場合は `converged = false` となる。
- 両入力に共通の平行移動を施した後、全頂点の広がりで無次元化してから本体の計算を行う。`tolerance` はこの正規化座標に対する相対許容誤差であり、一様な拡大縮小と共通の平行移動に依存しない。同符号の巨大な座標どうしは除算の前に差を取るため、有効な下位ビットを保持できる。
- 距離・最近点・終了時の単体は入力と同じ座標系に戻して返す。真の距離・Minkowski 差・最近点が `long double` で表現できない場合は `std::overflow_error` を送出する。交差しない場合の `normal` は有限な単位ベクトルである。
