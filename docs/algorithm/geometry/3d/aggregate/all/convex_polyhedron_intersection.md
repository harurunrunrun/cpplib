---
title: Convex Polyhedron Intersection (凸多面体同士の共通部分)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/convex_polyhedron_intersection.hpp
---

外向きの三角形面で表された 2 つの凸多面体の共通部分を `ConvexPolyhedron3` として返す。

このヘッダは汎用版・半空間版・狭義内点付き半空間版の個別ヘッダをまとめて include する、後方互換のための集約ヘッダである。新規コードでは必要な構築法の個別ヘッダだけを include すればよい。

## API

```cpp
ConvexPolyhedron3 convex_polyhedron_intersection(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second
);

ConvexPolyhedron3 convex_polyhedron_intersection_via_halfspaces(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second
);

ConvexPolyhedron3
convex_polyhedron_intersection_via_halfspaces_with_interior_point(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second,
    const Point3& strict_interior_point
);
```

- `convex_polyhedron_intersection`: 空集合や低次元の接触も追加の前提なしで扱う汎用版。
  - 両入力が `affine_dimension == 3` のときは、まず `convex_polyhedron_intersects_exact` で交差を厳密に判定し、交差しなければ空の多面体を返す。交差する場合は半空間版と同じ極双対による高速経路を試み、共通部分が正の体積を持てばその結果を返す。
  - 高速経路が使えない場合（低次元の入力、低次元の接触、途中の構築で `std::overflow_error` / `std::domain_error` が生じた場合）は、相手に含まれる入力頂点と、相手でクリップした全入力辺の端点を候補点として列挙し、その凸包を `convex_hull_3d` で返す。三次元の入力に対しては `ConvexPolyhedronQueryHierarchy3D` を構築して包含判定・線分クリップを行う。
- `convex_polyhedron_intersection_via_halfspaces`: 両入力の外向きの面を半空間 $n \cdot (x - p) \le 0$（$p$ は面上の点、$n$ は外向き法線）に変換し、極双対凸包から共通部分を構築する。正の体積を持つ共通部分では高速経路を使う。空集合なら空の多面体を返し、低次元の接触も返すが、この 2 種類は退化処理を使う。どちらかの頂点列が空なら空の多面体を返す。
- `convex_polyhedron_intersection_via_halfspaces_with_interior_point`: 共通部分の狭義内点を呼び出し側が与える高速版。

## API 別の時間計算量・空間計算量

$V_i, E_i, F_i$ を各入力の頂点数・辺数・三角形面数、$C$ を汎用版が列挙する候補点の数、$H = F_1 + F_2$、$K$ を返す頂点と三角形面の総数とする。厳密な整数演算は 1 回と数える。

- `convex_polyhedron_intersection`:
  - 両入力が三次元で、共通部分が正の体積を持つ場合: 半空間版と同じく期待時間 $O(V_1 + V_2 + H \log H + K)$、期待空間 $O(V_1 + V_2 + H + K)$。
  - 候補点を列挙する経路: 候補の列挙に最悪 $O(V_1 F_2 + V_2 F_1 + E_1 F_2 + E_2 F_1)$ 時間（三次元の入力では、クエリ階層の構築 $O((V_i + F_i) \log (V_i + F_i))$ の後、各包含判定が $O(\log V)$、各線分クリップが最悪 $O(F)$）、候補点の凸包に最悪 $O(C \log C + K)$ 時間。追加空間は $O(V_1 + V_2 + F_1 + F_2 + C + K)$。
- `convex_polyhedron_intersection_via_halfspaces`: 厳密な空判定は期待時間 $O(V_1 + V_2 + H)$、固定された制約順に対する最悪時間は $O(V_1 + V_2 + H^3)$ で、空ならこの時点で終了する。共通部分が三次元の場合は、その後の内点探索と構築を含めて期待時間 $O(H \log H + K)$、期待空間 $O(H + K)$。低次元の場合や途中の射影を表現できない場合の退化処理は最悪 $O(H^4 + K)$ 時間、空間 $O(H + K)$。
- `convex_polyhedron_intersection_via_halfspaces_with_interior_point`: 期待時間 $O(H \log H + K)$、期待空間 $O(H + K)$。固定シードに対する決定的な最悪時間は $O(H^2 \log H + K)$、最悪空間は $O(H^2 + K)$。

## 注意点

- 半空間版の両入力は `affine_dimension == 3` で、空でない `vertices` と、外向きで非退化な `faces` を持たなければならない。違反時は `std::invalid_argument` を送出する。
- 面の頂点添字が範囲外なら `std::invalid_argument` を送出する。
- 内点付き版の `strict_interior_point` は、すべての入力面を狭義に満たす有限な点でなければならない。違反時は `std::invalid_argument` を送出する。
- 半空間版で、空でなく非有界と判定された場合は `std::domain_error` を送出する。正しい有界凸多面体を入力すれば、共通部分も有界である。
- 同一平面を分割した複数の三角形面は、重複した制約として安全に扱われる。

## 実装上の位置づけ

`convex_polyhedron_intersection_via_halfspaces` は、空集合を厳密な三次元線形計画で早期に判定し、空でない場合は極双対による半空間の共通部分を構築する。Dobkin--Kirkpatrick 階層、witness triple、conflict list を用いる Chan の出力依存アルゴリズムそのものではないため、一般の空でない入力に対して $O(V_1 + V_2 + F_1 + F_2 + K)$ は保証しない。

## 参考文献

- Timothy M. Chan, *A Simpler Linear-Time Algorithm for Intersecting Two Convex Polyhedra in Three Dimensions*, SoCG 2015, [DOI: 10.4230/LIPIcs.SOCG.2015.249](https://doi.org/10.4230/LIPIcs.SOCG.2015.249)
